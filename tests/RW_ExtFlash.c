//******************************************************************************
//   UMass Moo read/write serial flash Demo - USCI_B1, SPI 3-Wire Master
//
//   Description: MCU talks to SST25WF040 flash using 3-wire mode. A data with 
//   any value is sent by the MCU if there is no write protection on the flash. 
//   Then the data is read into MCU from the SST25WF040. Received data is 
//   expected to be same as the previous written one.  USCI RX ISR is used to 
//   handle communication with the MCU. 
//   Initialization waits for DCO to stabilize against ACLK.
//   ACLK = 32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
//
//
//                    MSP430F261x
//                 -----------------
//             /|\|              XIN|-
//              | |                 |  32kHz xtal
//              --|RST          XOUT|-
//                |                 |
//                |             P5.1|-> Data Out (UCB1SIMO)
//                |                 |
//          LED <-|P4.2         P5.2|<- Data In (UCB1SOMI)
//                |                 |
//  Slave reset <-|RST         P5.3|-> Serial Clock Out (UCB1CLK)
//
//
//   Hong Zhang
//   PRISMS Lab, Computer Science, UMass, Amherst.
//   January 2011
//   Built with IAR Embedded Workbench Version: 4.21.2
//
//   (adapted from MSP430x261x_uscia0_spi_09.c from TI's MSP430F261x code
//    examples)
//******************************************************************************
#include  <msp430x26x.h>
#define CE    0x01
#define SIMO  0x02
#define SOMI  0x04
#define SCK   0x08

unsigned char MST_Data,SLV_Data, RCV_flag;

void init_spi()
{
  volatile unsigned int i;
  if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)                                     
  {  
    while(1); // If calibration constants erased do not load, trap CPU!!
  }  
  BCSCTL3 |= XCAP_3;                        // Configure load caps
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;

  // Wait for xtal to stabilize
  do
  {
    IFG1 &= ~OFIFG;                         // Clear OSCFault flag
    for (i = 0x47FF; i > 0; i--);           // Time for flag to set
  }
  while ((IFG1 & OFIFG));                   // OSCFault flag still set?

  for(i=2100;i>0;i--);                      // Now with stable ACLK, wait for
                                            // DCO to stabilize.
  P5OUT |= 0x01;                            // P5 setup for slave reset
  P5DIR |= 0x01;                            // P5.0 set output for CE#
  P5SEL |= 0x0E;                            // P5.3,2,1 option select
  UCB1CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    //3-pin, 8-bit SPI master
  UCB1CTL1 |= UCSSEL_2;                     // SMCLK
  UCB1BR0 = 0x02;                           // /2
  UCB1BR1 = 0;                              //
  UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UC1IE |= UCB1RXIE;                        // Enable USCI_B1 RX interrupt
  _BIS_SR(GIE);  //LPM4_bits + GIE, CPU off, enable interrupts

  for(i=50;i>0;i--);                        // Wait for slave to initialize
}

/************************************************************************/
/* PROCEDURE: Send_Byte							*/
/*            This procedure outputs a byte to flash			*/
/* Input: out   							*/
/* Output: byte output in TX buffer then at SIMO pin			*/
/************************************************************************/
void Send_Byte(unsigned char out)
{
  UCB1TXBUF = out;
  while (!(UC1IFG & UCB1TXIFG));  /* USCI_B1 TX buffer ready */
}
/************************************************************************/
/* PROCEDURE: Get_Byte							*/
/*	This procedure trigers a byte clock cycle by send a null byte,	*/
/* 	and get a byte from RX Buffer when the RX ISR is called.        */
/* Input:	Nothing							*/
/* Output:	in							*/
/************************************************************************/
unsigned char Get_Byte()
{
  volatile unsigned int i;

//  RCV_flag=0;
  Send_Byte(0x00);		// provide SCK for the read data
  for (i=5; i>0; i--);          // Add time delay between transmissions
//  while (!RCV_flag);        // USCI_B1 RX buffer ready

  return SLV_Data;
  
}
/************************************************************************/
/* PROCEDURE: CE_High							*/
/*            This procedure set CE = High.				*/
/* Input: None		        					*/
/* Output: CE	                					*/
/************************************************************************/
void CE_High() 
{
  P5OUT |= CE;			/* set CE high */
}

/************************************************************************/
/* PROCEDURE: CE_Low							*/
/*            This procedure drives the CE of the device to low. 	*/
/* Input: None		        					*/
/* Output: CE			        				*/
/************************************************************************/
void CE_Low() 
{	
  P5OUT &= ~CE;			/* clear CE low */
}

/************************************************************************/
/* PROCEDURE: WREN							*/
/*            This procedure enables the Write Enable Latch.  It can 	*/
/*            also  be used to Enables Write Status Register.           */
/* Input:	None							*/
/* Returns:	Nothing							*/
/************************************************************************/
void WREN()
{
  CE_Low();			// enable device 
  Send_Byte(0x06);		// send WREN command 
  CE_High();			// disable device
}

/************************************************************************/
/* PROCEDURE:	Read							*/
/*            This procedure reads one address of the device.  It will 	*/
/*            return the byte read in variable byte.			*/
/* Input:	Dst:	Destination Address 000000H - 7ffffH		*/
/* Returns:	byte							*/
/************************************************************************/
unsigned char Read(unsigned long Dst) 
{
  CE_Low();                             // CE low, enable device
  Send_Byte(0x03);                      // read command 
  Send_Byte(((Dst & 0xFFFFFF) >> 16));	// send 3 address bytes 
  Send_Byte(((Dst & 0xFFFF) >> 8));
  Send_Byte(Dst & 0xFF);
  Get_Byte();
  CE_High();
  return SLV_Data;		

}

/************************************************************************/
/* PROCEDURE:	Byte_Program						*/
/*            This procedure programs one address of the device.	*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/* Input:	Dst:		Destination Address 000000H - 7ffffH	*/
/*		byte:		byte to be programmed           	*/
/* Returns:	Nothing							*/
/************************************************************************/
void Byte_Program(unsigned long Dst, unsigned char byte)
{
  WREN();
  CE_Low();
  Send_Byte(0x02); 			/* send Byte Program command */
  Send_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
  Send_Byte(((Dst & 0xFFFF) >> 8));
  Send_Byte(Dst & 0xFF);
  Send_Byte(byte);			/* send byte to be programmed */
  CE_High();
}

/************************************************************************/
/* PROCEDURE: Read_ID							*/
/* This procedure Reads the manufacturer's ID and device ID.  It will 	*/
/* use 90h or ABh as the command to read the ID (90h in this sample).   */
/* It is up to the user to give the last byte ID_addr to determine      */
/* whether the device outputs manufacturer's ID first, or device ID 	*/
/* first.  Please see the product datasheet for details.  Returns ID in */
/* variable byte.							*/
/*									*/
/* Input:	ID_addr							*/
/* Returns:	byte:	ID1(Manufacture's ID = BFh or Device ID = 04h)	*/
/************************************************************************/
unsigned char Read_ID(unsigned char ID_addr)
{
  CE_Low();                     // CE low, enable device 
  Send_Byte(0x90);		// send read ID command (90h or ABh)
  Send_Byte(0x00);		// send address 
  Send_Byte(0x00);		// send address
  Send_Byte(ID_addr);		// send address - either 00H or 01H 
  Get_Byte();
  CE_High();
  return SLV_Data;
}

/************************************************************************/
/* PROCEDURE: Read_Status_Register					*/
/*            This procedure readw the status register and returns it.	*/
/* Input:	None							*/
/* Returns:	byte							*/
/************************************************************************/
unsigned char Read_Status_Register()
{
  CE_Low();                     // CE low, enable device
  Send_Byte(0x05);		// send read status register command (05h)
  Get_Byte();
  CE_High();
  return SLV_Data;
}

/************************************************************************/
/* PROCEDURE: WRSR							*/
/*	      This procedure writes to the Status Register.		*/
/* Input:	byte							*/
/* Returns:	Nothing							*/
/************************************************************************/
void WRSR(unsigned char regv)
{
  volatile unsigned int i;
  WREN();
  CE_Low();
  Send_Byte(0x01);		// send write status register command (01h) 
  Send_Byte(regv);              // send new status register value
  CE_High();
}


void main(void)
{
  unsigned char rcv_data;

  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  init_spi();

  MST_Data = 0x01;                         // Initialize data values
  SLV_Data = 0x00;                         //

  while(1)
  {
    rcv_data = Read_Status_Register();
    if (rcv_data & 0x9C)
    { 
      WRSR(0x02);
    } 
    else
    {  
      Byte_Program(0x012349, 0x25);
      Read(0x012349);
    }
  }
}

#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCIB1RX_ISR (void)
{

  if (UC1IFG & UCB1RXIFG)
  {
    SLV_Data = UCB1RXBUF;
  }

}                                           
