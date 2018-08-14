******************************************************************************
******************* Processor Expert Demonstration Project *******************
******************************************************************************

Project: I2C

Short description: I2C communication with an external 3-axis accelerometer

Target platforms: TWR-KL05Z48M


Function Description
====================
This project demonstrates the use of the I2C component. An external
accelerometer is initialized. For setting up the mode register a one byte
write/read operation is used, for write/read the data registers a multibyte
write/read operation is used. Comments and test results are sent to 
ConsoleUART (Serial component) via a Tower Serial module(TWR-SER).
RS232 settings: 38400baud rate, 8-bit, 1-stop bit, no parity.

Component brief description
===========================
I2C (I2C) - I2C module for communication demostration
TIMER (TimerUnit) - Timeout counter
CsIO1 (ConsoleIO) - UART connected to the TWR-SER module for console output

Required Hardware and Settings
==============================
TWR-KL05Z48M board (with an accelerometer)
TWR-ELEV Tower elevator module (to interconnect TWR-KL05Z48M with a TWR-SER module)
TWR-SER Serial module (for console output)

Building The Project
====================
Build the project the usual way in CodeWarrior.
No special settings are required.

Running The Project
===================
Run the project the usual way. 

*** End Of File ***
