# SASI
## Description
Short for simple ascii serial interface, sasi is a library for Arduino to exchange data from the Arduino board and the outside world over serial interface(UART).

It can be used for example to command servo motors, LEDs and to read buttons from a Python script running on a PC via the USB port of the Arduino.

Sasi has 256 registers, each of which can store a 16-bit value.

Each register can be written or read from a terminal via the serial interface of the Arduino (USB or TX/RX pins).

>Note: The Serial interface of the Arduino can not be used for other purposes anymore.

## Usage
Once implemented in a Arduino, sasi can be reached via two type of commands:
### Write command
A write command has always the same structure. 
~~~
W<address>V<data>[CR]
~~~
Address and data are coded in hexadecimal.
the address can be every value between "00" and "FF".
The data can be every value between "0000" and "FFFF".
CR is the carriage return.
<br>
The response for a write command is always a error code
~~~
X<error code>[LF]
~~~
LF is the line feed.
<br>
If the error code is 0 then is everything alright.

### Read command
A read command has always the same structure. 
~~~
R<address>[CR]
~~~
Address is coded in hexadecimal.
the address can be every value between "00" and "FF".
CR is the carriage return.
<br>
The response for a read command is the address of the register and its value. 
~~~
R<address>V<data>[LF]
~~~
LF is the line feed.
In case of an error, a similar response as for the write command will be given.
~~~
X<error code>[LF]
~~~
LF is the line feed.

### Example
|request  |response  |
|---------|----------|
|R05      |R05V0000  |
|W05V1138 |X0        |
|R05      |R05V1138  |
|H89V0000 |X2        |

### Error codes
|Response|explanation |
|--------|----------|
|X0      |No error  |
|X1      |Parameter are not hex|      |
|X2      |Wrong function (only R and W allowed)|
|X3      |Wrong delimiter (only V allowed)       |
|X4      |Not implemented yet|
## Installation
### Arduino IDE 2
Copy the SASI folder inside your Arduino libraries folder. The libraries folder is usually located in the same folder as the sketchbook folder of your project. <br>
Default location of the libraries folder:
~~~
~/Arduino/libraries
~~~
If the libraries folder is not already existing, create it.

## Implementation
### Arduino IDE 2
the sasi header need to be included.
~~~c++
#include <sasi.hpp>
~~~
An instance of the SASI class need to be created globaly.
~~~c++
SASI Sasi;
~~~ 
Sasi use the Serial object for the communication.
The Serial object need to be setted in the setup function.
~~~c++
void setup() {
    // ...
    Serial.begin(9600); 
    // ...
}
~~~
In the loop function the step method of sasi need to be called.
~~~c++
void loop() {
    // ...
    Sasi.step();
    // ...
}
~~~
To read the data at the needed register address use the get_register method.
~~~c++
// ...
uint8_t register_address = 0x0;
uint16_t register_value = Sasi.get_register(register_address);
// ...
~~~
To write data in a register use the set_register method.
~~~c++
// ...
uint8_t register_address = 0x0;
uint16_t register_data = 0x1138;
Sasi.set_register(register_address, register_data);
// ...
~~~



