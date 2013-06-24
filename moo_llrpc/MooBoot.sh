#!/bin/sh

# The following script will start moo_concrete_app on boot.
nohup /home/micharu123/Desktop/moo_concrete_reader 192.168.1.171 &
sleep 2
nohup /home/micharu123/Desktop/moo_concrete_reader 192.168.1.75 &
sleep 2
nohup /home/micharu123/Desktop/moo_concrete_reader 192.168.1.86 &


###
# To add it on Ubuntu, do the following:
#  Place this script in /etc/init.d
#  chmod 775 MooBoot.sh
#  sudo update-rc.d boot_reader.sh defaults
