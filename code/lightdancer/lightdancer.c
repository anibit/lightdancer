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
 * lightdancer.c
 *
 * Created: 3/9/2013 11:35:27 PM
 *  Author: Jon Wolfe
 */ 



#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "spwm.h"
#include "light_mgr.h"


#define ENABLE_TIME_INT() 	TIMSK |= _BV(OCIE0A)//set timer0 overflow interrupt, we'll use this interrupt for time tracking in the system.
#define DISABLE_TIME_INT()  TIMSK &= ~_BV(OCIE0A)

#define TIMER_COMPARE_VALUE 100

#define TIME_PRESCALER		8 //NOTE this must match what is hard coded into the TCCR0B register!
#define TIME_TICK_FREQ		(F_CPU/TIMER_COMPARE_VALUE/TIME_PRESCALER)
#define TIME_KTICK_FREQ		(TIME_TICK_FREQ/1000)


static volatile int count = 0;

static volatile unsigned char ticks = 0;
static volatile unsigned char trip = false;
static unsigned char ticksAccum = 0;

ISR(TIM0_COMPA_vect)
{
	spwm_tick();
	ticks++;
	if (ticks == 0)
	{
		trip = true;
	}
	
}


//private functions
static unsigned long getDelta();

void setup_seed()
{
	unsigned char oldADMUX = ADMUX;
	ADMUX |=  _BV(MUX0); //choose ADC1 on PB2
	ADCSRA |= _BV(ADPS2) |_BV(ADPS1) |_BV(ADPS0); //set prescaler to max value, 128
	
	ADCSRA |= _BV(ADEN); //enable the ADC
	ADCSRA |= _BV(ADSC);//start conversion
	
	while (ADCSRA & _BV(ADSC)); //wait until the hardware clears the flag. Note semicolon!
	
	unsigned char byte1 = ADCL;
	
	ADCSRA |= _BV(ADSC);//start conversion
	
	while (ADCSRA & _BV(ADSC)); //wait again note semicolon!
	
	unsigned char byte2 = ADCL;
	
	unsigned int seed = byte1 << 8 | byte2;
	
	srand(seed);
	
	ADCSRA &= ~_BV(ADEN); //disable ADC

	ADMUX = oldADMUX;
}


int main(void)
{
	setup_seed();
	//disable interrupts globally
	cli();
	
	spwm_init();
	light_mgr_init();
	//Note the below code is AtTiny85/45 specific
	//setup timer 0
	TCCR0A |= _BV(WGM01); //"CTC" mode
	TCCR0A &= ~_BV(COM0A1); //disconnect timer output in pin
	TCCR0A &= ~_BV(COM0B1); //disconnect timer output in pin
	
	OCR0A = TIMER_COMPARE_VALUE;
	
	//enable timer 0
	TCCR0B = (TCCR0B & ~7) | _BV(CS01) /*| _BV(CS00)*/; //set prescaler to 8, 
	
	//enable interrupts globally
	sei();
	ENABLE_TIME_INT();
    while(1)
    {
		unsigned long delta = getDelta();	
		if (delta > 0)
		{
			light_mgr_update(delta);			
		}
    }
}

static unsigned long getDelta()
{
	cli();
	volatile unsigned char intTicks = ticks;
	ticks = 0;
	sei();
	ticksAccum += intTicks;
	
	//We're not keeping wall clock time here,
	//	this is going to be a crude estimate of time.
	unsigned long timeDelta_ms = (((unsigned long)ticksAccum) * 1000UL) / TIME_TICK_FREQ;
		
	if (timeDelta_ms > 0)
	{
		ticksAccum -= (timeDelta_ms * TIME_TICK_FREQ) / 1000UL;
	}
	return timeDelta_ms;
}
