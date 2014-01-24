lightdancer
===========

This is code that animated light patterns on the 6 available I/O
pins of the Atmel ATtiny85 microcontroller.

It uses software PWM, and can also use pin PB5 that is the 
repurposed reset line. 

This is the source code used in [this](https://anibit.com/product/light-dancer-kit).


This was developed under Windows with Atmel Studio, but if you are willing to port 
to other systems, let me know, and I'll include the needed build scripts.

The included batch files assume that you have a copy of avrdude in .\tools\avrdude\  
and that you are using a usbtiny programming device. The easiest way to get a get 
of avrdude for Windows is to download the Arduino IDE.


