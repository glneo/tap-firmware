# TAP
TAP Programmable LED Array

The TAP contains eight rows each made up of eight pixels which in turn consists of three individually controllable light emitting diodes, red, green and blue.

### The Display

Electrically, the LEDs are arranged in a matrix of 24 columns by 8 rows. The columns
are controlled by GPIO pins from the MSP430; a “1” on a column pin grounds its corresponding
column. A “0” on a row pin ties the row to power. The rows pings are addressed using a shift register,
at the start of a scan a "0" is shifted into the register and it is clocked once to send +5V to each subsequent row.

### The Display Buffer

A display buffer consists of a two dimensional array of the desired intensities of the LEDs, each one ranging from 0 to 31(FULL_INTENSITY). Colors are stored in the column index and are in the
order red, green, then blue. Generally, only a single display buffer is needed, namely
'display_buffer'. Constants are defined to facilitate direct access to the intensities. For
example, the green LED of column 3 on row 2 (all zero based) can be addressed as
follows: display_buffer[ROW2][COL3+GREEN].
The scanning engine is conceptually very simple. Each time scan_LEDs is called, it
performs one scan of the LEDs, using the intensity values in the display buffer to turn on
each LED for an appropriate amount of time.

### USB Support

To configure for building without the USB subsystem, edit main.h to comment out the
definition USB_SUPPORT.

### Serial Ports
The MSP430F5510 microcontroller contains two serial input and output ports.
TAP units have been designed to be plugged together to form a larger display panel. The
B input port receives data from the adjacent unit to the left; the B output port sends data
to the adjacent unit to the right. The A input port receives data from the adjacent unit
below; the A output port sends data to the adjacent unit above.

We currently initialize the serial ports for 115200 baud. The hardware is capable of
much higher data rates should it be needed for demanding amounts of binary image data.

### Packet Forwarding
In order to form a large display panel from individual TAP units, we use a packet message communication system. The firmware has
been designed to support assigning each unit a unique ID so that it can be directly
addressed. For this mechanism to work, each TAP must pass packets through its serial
ports. Packets are CRC checked.

### Learn Mode
The command packet definition has provision to direct information to a single specific
unit when several are connected together. Learn Mode is how to set this up.
Generally, the unit in the lower left corner of the arrangement is assigned the address
(0,0). The one to its right is (0,1), then (0,2), and so on. The unit above it is (1,0) and the
units to that one’s right is (1,1) and so on.

### Tap Synchronization
A special provision has been implemented in the standard firmware to allow easily
synchronizing timing across multiple TAP units when forming a larger single display.

### Source Code Organization

* channel.c - Channel coding, common to all communication types, forwards to the packet decoder.

* demo.c - Program containing the user program. (Put your application code here)

* display.c - Display related code, handles reading from the display buffer and outputting to the LEDs

* init.c - Manages the power-up reset process, setting up the clock and initializing the other subsystem.

* main.c - The foundation of the firmware. Contains the main loop.

* packet.c - Decoding and encoding of packets.

* photo.c - Phototransister related setup and communication handling.

* state.c - System state.

* uart.c - UART related setup and communication handling.

* usb.c - USB related setup and communication handling.

### Math Library
At least one of the demo files makes use of a TI Math Library called IQMATHLIB.
This library is not included in this repository.
You can download and install this library from TI here:
http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/IQmathLib/latest/index_FDS.html
It is linked to this project in the code composer studio configuration file in it's default install location which is ${TI_PRODUCTS_DIR}/msp430/IQMATHLIB_01_10_00_05/
If your install directory is different or if you wish not to use this library and the demo files that make use of it you will need to modify the code composer studio configuration file accordingly.
