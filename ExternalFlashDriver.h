/*Software Driver

SST25WF040 4 Mbit Serial Flash Memory

October 11, 2010, Rev. 1.0

ABOUT THE SOFTWARE
This application note provides software driver examples for SST25WF040,
Serial Flash. Extensive comments are included in each routine to describe
the function of each routine.  The interface coding uses polling method
rather than the SPI protocol to interface with these serial devices.  The
functions are differentiated below in terms of the communication protocols
(uses Mode 0) and specific device operation instructions. This code has been
designed to compile using the Keil compiler.


ABOUT THE SST25WF040

Companion product datasheets for the SST25WF040 should be reviewed in
conjunction with this application note for a complete understanding
of the device.


Device Communication Protocol(pinout related) functions:

Functions                    		Function
------------------------------------------------------------------
init					Initializes clock to set up mode 0.
Send_Byte				Sends one byte using SI pin to send and
					shift out 1-bit per clock rising edge
Get_Byte				Receives one byte using SO pin to receive and shift
					in 1-bit per clock falling edge
Poll_SO					Used in the polling for RY/BY# of SO during AAI
                        programming
CE_High					Sets Chip Enable pin of the serial flash to high
CE_Low					Clears Chip Enable of the serial flash to low
Reset_Hold_Low			Clears Hold/Reset pin to make serial flash hold	or reset
UnReset_Hold			Dereset/Unholds the serial flash
WP_Low					Clears WP pin to make serial flash write protected
UnWP					Disables write protection pin

Note:  The pin names of the SST25WF040 are used in this application note. The
associated test code will not compile unless these pinouts (SCK, SI, SO, SO, CE,
WP, Reset_Hold) are pre-defined on your software which should reflect your
hardware interfaced.


Device Operation Instruction functions:

Functions                    		Function
------------------------------------------------------------------
Read_Status_Register			Reads the status register of the serial flash
EWSR					Enables the Write Status Register
WRSR					Performs a write to the Status Register
WREN					Write enables the serial flash
WRDI					Write disables the serial flash
EBSY					Enable SO to output RY/BY# status during AAI programming
DBSY					Disable SO to output RY/BY# status during AAI
                        programming
Read_ID					Reads the manufacturer ID and device ID
Jedec_ID_Read				Reads the Jedec ID
Read					Reads one byte from the serial flash and returns byte
                        (max of 20 MHz CLK frequency)
Read_Cont				Reads multiple bytes(max of 20 MHz CLK frequency)
HighSpeed_Read			Reads one byte from the serial flash and returns byte
                        (max of 40 MHz CLK frequency)
HighSpeed_Read_Cont			Reads multiple bytes(max of 40 MHz CLK frequency)
Byte_Program				Program one byte to the serial flash
Auto_Add_IncA				Initial Auto Address Increment process
Auto_Add_IncB				Successive Auto_Address_Increment process after AAI
                            initiation
Auto_Add_IncA_EBSY			Initial Auto Address Increment process with EBSY
Auto_Add_IncB_EBSY			Successive Auto_Address_Increment process after AAI
                            initiation with EBSY
Chip_Erase				Erases entire serial flash
Sector_Erase				Erases one sector (4 KB) of the serial flash
Block_Erase_32K				Erases 32 KByte block memory of the serial flash
Block_Erase_64K				Erases 64 KByte block memory of the serial flash
Wait_Busy				Polls status register until busy bit is low
EHLD					Enables Hold# pin functionality of the RST#/HOLD# pin
*/


//"C" LANGUAGE DRIVERS

/********************************************************************/
/* Copyright Microchip Technology Inc.  1994-2010	                */
/* Example "C" language Driver of SST25WF040 Serial Flash	        */
/* Hardik Patel, Microchip Technology Inc.                          */
/*                                                                  */
/* Revision 1.0, October 11, 2010			  	                    */
/*                                                                  */
/*								                                    */
/********************************************************************/
#include "msp430x26x.h"
#define CE    0x01
#define SIMO  0x02
#define SOMI  0x04
#define SCK   0x08

/* Function Prototypes */

void init();
void Send_Byte(unsigned char out);
unsigned char Get_Byte();
void Poll_SO();
void CE_High();
void CE_Low();
void Reset_Hold_Low();
void UnReset_Hold();
void WP_Low();
void UnWP();
unsigned char Read_Status_Register();
void EWSR();
void WRSR(unsigned char byte);
void WREN();
void WRDI();
void EBSY();
void DBSY();
void EHLD();
unsigned char Read_ID(unsigned char ID_addr);
unsigned long Jedec_ID_Read();
unsigned char Read(unsigned long Dst);
void Read_Cont(unsigned long Dst, unsigned long no_bytes);
unsigned char HighSpeed_Read(unsigned long Dst);
void HighSpeed_Read_Cont(unsigned long Dst, unsigned long no_bytes);
void Byte_Program(unsigned long Dst, unsigned char byte);
void Auto_Add_IncA(unsigned long Dst, unsigned char byte1, unsigned char byte2);
void Auto_Add_IncB(unsigned char byte1, unsigned char byte2);
void Auto_Add_IncA_EBSY(unsigned long Dst, unsigned char byte1,
        unsigned char byte2);
void Auto_Add_IncB_EBSY(unsigned char byte1, unsigned char byte2);
void Chip_Erase();
void Sector_Erase(unsigned long Dst);
void Block_Erase_32K(unsigned long Dst);
void Block_Erase_64K(unsigned long Dst);
void Wait_Busy();
void Wait_Busy_AAI();



unsigned char upper_128[128];	/* global array to store read data */
					

/************************************************************************/
/* PROCEDURE: init							*/
/*									*/
/* This procedure initializes the SCK to low. Must be called prior to 	*/
/* setting up mode 0.							*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		SCK							*/
/************************************************************************/
void init()
{
	P5OUT &= ~SCK;	/* set clock to low initial state */
}

/************************************************************************/
/* PROCEDURE: Send_Byte							*/
/*									*/
/* This procedure outputs a byte shifting out 1-bit per clock rising	*/
/* edge on the the SI pin(LSB 1st).					*/
/*									*/
/* Input:								*/
/*		out							*/
/*									*/
/* Output:								*/
/*		SI							*/
/************************************************************************/
void Send_Byte(unsigned char out)
{
	
	unsigned char i = 0;
	for (i = 0; i < 8; i++)
	{
		
		if ((out & 0x80) == 0x80)	/* check if MSB is high */
			P5OUT |= SIMO;
		else
			P5OUT &= ~SIMO;		/* if not, set to low */
		P5OUT |= SCK;			/* toggle clock high */
		out = (out << 1);		/* shift 1 place for next bit */
		P5OUT &= ~SCK;			/* toggle clock low */
	}
}

/************************************************************************/
/* PROCEDURE: Get_Byte							*/
/*									*/
/* This procedure inputs a byte shifting in 1-bit per clock falling	*/
/* edge on the SO pin(LSB 1st).						*/
/*									*/
/* Input:								*/
/*		SO							*/
/*									*/
/* Output:								*/
/*		None							*/
/************************************************************************/
unsigned char Get_Byte()
{
	unsigned char i = 0, in = 0, temp = 0;
	for (i = 0; i < 8; i++)
	{
		in = (in << 1);		/* shift 1 place to the left or shift in 0 */
		temp = P5IN & SOMI;		/* save input */
		P5OUT |= SCK;		/* toggle clock high */
	if (temp == 1)			/* check to see if bit is high */
		in = in | 0x01;		/* if high, make bit high */

		P5OUT &= ~SCK;		/* toggle clock low */

	}
	return in;
}

/************************************************************************/
/* PROCEDURE: Poll_SO							*/
/*									*/
/* This procedure polls for the SO line during AAI programming  	*/
/* waiting for SO to transition to 1 which will indicate AAI programming*/
/* is completed								*/
/*									*/
/* Input:								*/
/*		SO							*/
/*									*/
/* Output:								*/
/*		None							*/
/************************************************************************/
void Poll_SO()
{
	unsigned char temp = 0;
	CE_Low();
       	while (temp == 0x00)	/* waste time until not busy */
		temp = P5IN & SOMI;
	CE_High();
}

/************************************************************************/
/* PROCEDURE: CE_High							*/
/*									*/
/* This procedure set CE = High.					*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		CE							*/
/*									*/
/************************************************************************/
void CE_High()
{
	P5OUT |= CE;			/* set CE high */
}

/************************************************************************/
/* PROCEDURE: CE_Low							*/
/*									*/
/* This procedure drives the CE of the device to low.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		CE							*/
/*									*/
/************************************************************************/
void CE_Low()
{	
	P5OUT &= ~CE;			/* clear CE low */
}

/************************************************************************/
/* PROCEDURE: Reset_Hold_Low()							*/
/*									*/
/* This procedure clears the Reset_Hold pin to low.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		Reset_Hold							*/
/************************************************************************/
void Reset_Hold_Low()
{
//	Reset_Hold = 0;			/* clear Reset_Hold pin */
}

/************************************************************************/
/* PROCEDURE: UnReset_Hold()							*/
/*									*/
/* This procedure sets the Reset_Hold pin to high.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		Reset_Hold							*/
/************************************************************************/
void UnReset_Hold()
{
//	Reset_Hold = 1;			/* set Reset_Hold pin */
}

/************************************************************************/
/* PROCEDURE: WP()							*/
/*									*/
/* This procedure clears the WP pin to low.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		WP							*/
/************************************************************************/
void WP_Low()
{
//	WP = 0;				/* clear WP pin */
}

/************************************************************************/
/* PROCEDURE: UnWP()							*/
/*									*/
/* This procedure sets the WP pin to high.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		WP							*/
/************************************************************************/
void UnWP()
{
//	WP = 1;				/* set WP pin */
}

/************************************************************************/
/* PROCEDURE: Read_Status_Register					*/
/*									*/
/* This procedure read the status register and returns the byte.	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		byte							*/
/************************************************************************/
unsigned char Read_Status_Register()
{
	unsigned char byte = 0;
	CE_Low();			/* enable device */
	Send_Byte(0x05);		/* send RDSR command */
	byte = Get_Byte();		/* receive byte */
	CE_High();			/* disable device */
	return byte;
}



/************************************************************************/
/* PROCEDURE: EWSR							*/
/*									*/
/* This procedure Enables Write Status Register.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void EWSR()
{
	CE_Low();			/* enable device */
	Send_Byte(0x50);		/* enable writing to the status register */
	CE_High();			/* disable device */
}


/************************************************************************/
/* PROCEDURE: WRSR							*/
/*									*/
/* This procedure writes to the Status Register.			*/
/*									*/
/* Input:								*/
/*		byte							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void WRSR(unsigned char byte)
{
	CE_Low();			/* enable device */
	Send_Byte(0x01);		/* select write to status register */
	Send_Byte(byte);	
 	CE_High();			/* disable the device */
}

/************************************************************************/
/* PROCEDURE: WREN							*/
/*									*/
/* This procedure enables the Write Enable Latch.  It can also be used 	*/
/* to Enables Write Status Register.					*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void WREN()
{
	CE_Low();			/* enable device */
	Send_Byte(0x06);		/* send WREN command */
	CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: WRDI							*/
/*									*/
/* This procedure disables the Write Enable Latch.			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void WRDI()
{
	CE_Low();			/* enable device */
	Send_Byte(0x04);		/* send WRDI command */
	CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: EBSY							*/
/*									*/
/* This procedure enable SO to output RY/BY# status during AAI 		*/
/* programming.								*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void EBSY()
{
	CE_Low();			/* enable device */
	Send_Byte(0x70);		/* send EBSY command */
	CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: DBSY							*/
/*									*/
/* This procedure disable SO as output RY/BY# status signal during AAI	*/
/* programming.								*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void DBSY()
{
	CE_Low();			/* enable device */
	Send_Byte(0x80);		/* send DBSY command */
	CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: Read_ID							*/
/*									*/
/* This procedure Reads the manufacturer's ID and device ID.  It will 	*/
/* use 90h or ABh as the command to read the ID (90h in this sample).   */
/* It is up to the user to give the last byte ID_addr to determine      */
/* whether the device outputs manufacturer's ID first, or device ID 	*/
/* first.  Please see the product datasheet for details.  Returns ID in */
/* variable byte.							*/
/*									*/
/* Input:								*/
/*		ID_addr							*/
/*									*/
/* Returns:								*/
/*		byte:	ID1(Manufacture's ID = BFh or Device ID = 04h)	*/
/*									*/
/************************************************************************/
unsigned char Read_ID(unsigned char ID_addr)
{
	unsigned char byte;
	CE_Low();			/* enable device */
	Send_Byte(0x90);		/* send read ID command (90h or ABh) */
    	Send_Byte(0x00);		/* send address */
	Send_Byte(0x00);		/* send address */
	Send_Byte(ID_addr);		/* send address - either 00H or 01H */
	byte = Get_Byte();		/* receive byte */
	CE_High();			/* disable device */
	return byte;
}

/************************************************************************/
/* PROCEDURE: Jedec_ID_Read						*/
/*									*/
/* This procedure Reads the manufacturer's ID (BFh), memory type (25h)  */
/* and device ID (04h).  It will use 9Fh as the JEDEC ID command.    	*/
/* Please see the product datasheet for details.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		IDs_Read:ID1(Manufacture's ID = BFh, Memory Type (25h), */
/*		 and Device ID (04h)					*/
/*									*/
/************************************************************************/
unsigned long Jedec_ID_Read()
{
	unsigned long temp;
	
	temp = 0;

	CE_Low();			 /* enable device */
	Send_Byte(0x9F);		 /* send JEDEC ID command (9Fh) */
    	temp = (temp | Get_Byte()) << 8; /* receive byte */
	temp = (temp | Get_Byte()) << 8;	
	temp = (temp | Get_Byte()); 	 /* temp value = 0xBF2504 */
	CE_High();			 /* disable device */

	return temp;
}

/************************************************************************/
/* PROCEDURE: EHLD							*/
/*									*/
/* This procedure enables Reset_Hold pin to function as a Hold pin	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void EHLD()
{
	CE_Low();			/* enable device */
	Send_Byte(0xaa);		/* send EHLD command */
	CE_High();			/* disable device */
}




/************************************************************************/
/* PROCEDURE:	Read							*/
/*									*/		
/* This procedure reads one address of the device.  It will return the 	*/
/* byte read in variable byte.						*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:	Destination Address 000000H - 7ffffH		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		byte							*/
/*									*/
/************************************************************************/
unsigned char Read(unsigned long Dst)
{
	unsigned char byte = 0;	

	CE_Low();			/* enable device */
	Send_Byte(0x03); 		/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	byte = Get_Byte();
	CE_High();			/* disable device */
	return byte;			/* return one byte read */
}

/************************************************************************/
/* PROCEDURE:	Read_Cont						*/
/*									*/		
/* This procedure reads multiple addresses of the device and stores	*/
/* data into 128 byte buffer. */
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*      	no_bytes	Number of bytes to read	(max = 128)	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Read_Cont(unsigned long Dst, unsigned long no_bytes)
{
	unsigned long i = 0;
	CE_Low();				/* enable device */
	Send_Byte(0x03); 			/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	for (i = 0; i < no_bytes; i++)		/* read until no_bytes is reached */
	{
		upper_128[i] = Get_Byte();	
	}
	CE_High();				/* disable device */

}

/************************************************************************/
/* PROCEDURE:	HighSpeed_Read						*/
/*									*/		
/* This procedure reads one address of the device.  It will return the 	*/
/* byte read in variable byte.						*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:	Destination Address 000000H - 7ffffH		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		byte							*/
/*									*/
/************************************************************************/
unsigned char HighSpeed_Read(unsigned long Dst)
{
	unsigned char byte = 0;	

	CE_Low();			/* enable device */
	Send_Byte(0x0B); 		/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(0xFF);		/*dummy byte*/
	byte = Get_Byte();
	CE_High();			/* disable device */
	return byte;			/* return one byte read */
}

/************************************************************************/
/* PROCEDURE:	HighSpeed_Read_Cont					*/
/*									*/		
/* This procedure reads multiple addresses of the device and stores	*/
/* data into 128 byte buffer. Maximum byte that can be read is 128 bytes*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*      	no_bytes	Number of bytes to read	(max = 128)	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void HighSpeed_Read_Cont(unsigned long Dst, unsigned long no_bytes)
{
	unsigned long i = 0;
	CE_Low();				/* enable device */
	Send_Byte(0x0B); 			/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(0xFF);			/*dummy byte*/
	for (i = 0; i < no_bytes; i++)		/* read until no_bytes is reached */
	{
		upper_128[i] = Get_Byte();	/* receive byte */
	}
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE:	Byte_Program						*/
/*									*/
/* This procedure programs one address of the device.			*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*		byte:		byte to be programmed			*/
/*      								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Byte_Program(unsigned long Dst, unsigned char byte)
{
	CE_Low();				/* enable device */
	Send_Byte(0x02); 			/* send Byte Program command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(byte);			/* send byte to be programmed */
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE:	Auto_Add_IncA						*/
/*									*/
/* This procedure programs consecutive addresses of 2 bytes of data into*/
/* the device:  1st data byte will be programmed into the initial 	*/
/* address [A23-A1] and with A0 = 0.  The 2nd data byte will be be 	*/
/* programmed into initial address [A23-A1] and with A0  = 1.  This  	*/
/* is used to to start the AAI process.  It should be followed by 	*/
/* Auto_Add_IncB.							*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/*									*/
/*									*/
/* Note: Only RDSR command can be executed once in AAI mode with SO  	*/
/* 	 disable to output RY/BY# status.  Use WRDI to exit AAI mode 	*/
/*	 unless AAI is programming the last address or last address of  */
/* 	 unprotected block, which automatically exits AAI mode.		*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*		byte1:		1st byte to be programmed		*/
/*      	byte1:		2nd byte to be programmed		*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Auto_Add_IncA(unsigned long Dst, unsigned char byte1, unsigned char byte2)
{
	CE_Low();				/* enable device */
	Send_Byte(0xAD);			/* send AAI command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(byte1);			/* send 1st byte to be programmed */	
	Send_Byte(byte2);			/* send 2nd byte to be programmed */
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE:	Auto_Add_IncB						*/
/*									*/
/* This procedure programs consecutive addresses of 2 bytes of data into*/
/* the device:  1st data byte will be programmed into the initial 	*/
/* address [A23-A1] and with A0 = 0.  The 2nd data byte will be be 	*/
/* programmed into initial address [A23-A1] and with A0  = 1.    This  	*/
/* is used after Auto_Address_IncA.					*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/*									*/
/* Note: Only WRDI and AAI command can be executed once in AAI mode 	*/
/*	 with SO enabled as RY/BY# status.  When the device is busy 	*/
/*	 asserting CE# will output the status of RY/BY# on SO.  Use WRDI*/
/* 	 to exit AAI mode unless AAI is programming the last address or */
/*	 last address of unprotected block, which automatically exits 	*/
/*	 AAI mode.							*/
/*									*/
/* Input:								*/
/*									*/
/*		byte1:		1st byte to be programmed		*/
/*		byte2:		2nd byte to be programmed		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Auto_Add_IncB(unsigned char byte1, unsigned char byte2)
{
	CE_Low();				/* enable device */
	Send_Byte(0xAD);			/* send AAI command */
	Send_Byte(byte1);			/* send 1st byte to be programmed */
	Send_Byte(byte2);			/* send 2nd byte to be programmed */
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE:	Auto_Add_IncA_EBSY					*/
/*									*/
/* This procedure is the same as procedure Auto_Add_IncA except that it */
/* uses EBSY and Poll_SO functions to check for RY/BY. It programs 	*/
/* consecutive addresses of the device.  The 1st data byte will be 	*/
/* programmed into the initial address [A23-A1] and with A0 = 0.  The 	*/
/* 2nd data byte will be programmed into initial address [A23-A1] and 	*/
/* with A0  = 1.  This is used to to start the AAI process.  It should  */
/* be followed by Auto_Add_IncB_EBSY.					*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/*									*/
/*									*/
/* Note: Only WRDI and AAI command can be executed once in AAI mode 	*/
/*	 with SO enabled as RY/BY# status.  When the device is busy 	*/
/*	 asserting CE# will output the status of RY/BY# on SO.  Use WRDI*/
/* 	 to exit AAI mode unless AAI is programming the last address or */
/*	 last address of unprotected block, which automatically exits 	*/
/*	 AAI mode.							*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*		byte1:		1st byte to be programmed		*/
/*      	byte1:		2nd byte to be programmed		*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Auto_Add_IncA_EBSY(unsigned long Dst, unsigned char byte1,
        unsigned char byte2)
{
	EBSY();					/* enable RY/BY# status for SO in AAI */	

	CE_Low();				/* enable device */
	Send_Byte(0xAD);			/* send AAI command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(byte1);			/* send 1st byte to be programmed */	
	Send_Byte(byte2);			/* send 2nd byte to be programmed */
	CE_High();				/* disable device */
	
	Poll_SO();				/* polls RY/BY# using SO line */

}

/************************************************************************/
/* PROCEDURE:	Auto_Add_IncB_EBSY					*/
/*									*/
/* This procedure is the same as Auto_Add_IncB excpet that it uses 	*/
/* Poll_SO to poll for RY/BY#.  It demonstrate on how to use DBSY after	*/
/* AAI programmming is completed.  It programs consecutive addresses of */
/* the device.  The 1st data byte will be programmed into the initial   */
/* address [A23-A1] and with A0 = 0.  The 2nd data byte will be 	*/
/* programmed into initial address [A23-A1] and with A0  = 1.  This is 	*/
/* used after Auto_Address_IncA.					*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/*									*/
/* Note: Only WRDI and AAI command can be executed once in AAI mode 	*/
/*	 with SO enabled as RY/BY# status.  When the device is busy 	*/
/*	 asserting CE# will output the status of RY/BY# on SO.  Use WRDI*/
/* 	 to exit AAI mode unless AAI is programming the last address or */
/*	 last address of unprotected block, which automatically exits 	*/
/*	 AAI mode.							*/
/*									*/
/* Input:								*/
/*									*/
/*		byte1:		1st byte to be programmed		*/
/*		byte2:		2nd byte to be programmed		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Auto_Add_IncB_EBSY(unsigned char byte1, unsigned char byte2)
{
	CE_Low();				/* enable device */
	Send_Byte(0xAD);			/* send AAI command */
	Send_Byte(byte1);			/* send 1st byte to be programmed */
	Send_Byte(byte2);			/* send 2nd byte to be programmed */
	CE_High();				/* disable device */

	Poll_SO();				/* polls RY/BY# using SO line */

//	WRDI(); 				/* Exit AAI before executing DBSY */
//	DBSY();					/* disable SO as RY/BY# output if in AAI */
}

/************************************************************************/
/* PROCEDURE: Chip_Erase						*/
/*									*/
/* This procedure erases the entire Chip.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Chip_Erase()
{						
	CE_Low();				/* enable device */
	Send_Byte(0x60);			/* send Chip Erase command (60h or C7h) */
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE: Sector_Erase						*/
/*									*/
/* This procedure Sector Erases the Chip.				*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Sector_Erase(unsigned long Dst)
{


	CE_Low();				/* enable device */
	Send_Byte(0x20);			/* send Sector Erase command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE: Block_Erase_32K						*/
/*									*/
/* This procedure Block Erases 32 KByte of the Chip.			*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Block_Erase_32K(unsigned long Dst)
{
	CE_Low();				/* enable device */
	Send_Byte(0x52);			/* send 32 KByte Block Erase command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	CE_High();				/* disable device */
}


/************************************************************************/
/* PROCEDURE: Block_Erase_64K						*/
/*									*/
/* This procedure Block Erases 64 KByte of the Chip.			*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 7ffffH	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Block_Erase_64K(unsigned long Dst)
{
	CE_Low();				/* enable device */
	Send_Byte(0xD8);			/* send 64 KByte Block Erase command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	CE_High();				/* disable device */
}
/************************************************************************/
/* PROCEDURE: Wait_Busy							*/
/*									*/
/* This procedure waits until device is no longer busy (can be used by	*/
/* Byte-Program, Sector-Erase, Block-Erase, Chip-Erase).		*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Wait_Busy()
{
	while ((Read_Status_Register()& 0x01) == 0x01)	/* waste time until !busy */
		Read_Status_Register();
}

/************************************************************************/
/* PROCEDURE: Wait_Busy_AAI						*/
/*									*/
/* This procedure waits until device is no longer busy for AAI mode.	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Wait_Busy_AAI()
{
	while ((Read_Status_Register()& 0x41) == 0x41)	/* waste time until !busy */
		Read_Status_Register();
}



