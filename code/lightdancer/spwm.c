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
Disclaimer: 
This was written for an AtTiny85, built with AVRStudio 6.1
It will likely work with other AVR chips with little or perhaps 
even no modification. It has not been tested however, 
your millage may vary. 
*/

#include <avr/io.h>
#include "spwm.h"

#define NUM_PINS	6

static unsigned char counter = 0;
static unsigned char next_state;

static volatile unsigned char onCounts[NUM_PINS];

//private declarations

void spwm_init()
{
	for (unsigned char i = 0; i < NUM_PINS; ++i)
	{
		onCounts[i] = 0;
		PORTB |= _BV(i);
		DDRB |= _BV(i);
	}
}


void spwm_tick()
{
	PORTB = (PORTB & 0xC0) | next_state;
	next_state = 0x00;
	if (++counter > 99)
	{
		counter = 0;
	}
	for (int i = 0; i < NUM_PINS; ++i)
	{		
		if (counter < onCounts[i])
		{
			next_state |= _BV(i);
		}
	}
}

void spwm_set_count(int pin, int count)
{
	onCounts[pin] = count;
}


