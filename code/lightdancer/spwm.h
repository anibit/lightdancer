/*
Copyright (c) 2013, Jon Wolfe, Anibit Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
	and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


/*
spwm.h - code to handle software- based pulse width modulation

This allows for very versatile, 'no fuss' PWM of all available IO pins.
This is quite CPU intensive, relative to hardware-based PWM, but it is very flexible
and easy to use.
 
 *
 * Created: 3/9/2013 11:43:54 PM
 *  Author: Jon Wolfe
 */ 

/*
Disclaimer:
This was written for an AtTiny85, built with AVRStudio 6.1
It will likely work with other AVR chips with little or perhaps
even no modification. It has not been tested however,
your millage may vary.
*/


#ifndef SPWM_H_
#define SPWM_H_

//Initialize the PWM system. This must be called at powerup
void spwm_init();

//must be called 100 times per 'period' of the PWM
void spwm_tick();

//sets the logical 'on' point for a given pin.
void spwm_set_count(int pin, int count);




#endif /* SPWM_H_ */