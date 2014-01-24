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
 * light_mgr.c
 *
 * Created: 3/10/2013 1:24:36 AM
 *  Author: jon
 */ 

#include "light_mgr.h"
#include <stdlib.h>

#include "spwm.h"

//#define DEBUG_MODE RandomPop

#ifdef DEBUG_MODE
#define IDLE_TIME	0
#define FADEOUT_TIME_MS 0
#else
#define IDLE_TIME					3000
#define FADEOUT_TIME_MS				3000
#endif
#define IDLE_MAX_S					30
#define CHASE_TIMEOUT_MS			20000
#define CHASE_CYCLE_TIME_MS			1000
#define PULSE_TIMEOUT_MS			4000
#define PULSE_CYCLE_TIME_MS			1000
#define EVEN_ODD_TIMEOUT_MS			20000
#define EVEN_ODD_CYCLE_TIME_MS		2000
#define CHASE2_TIMEOUT_MS			20000
#define CHASE2_CYCLE_TIME_MS		5000
#define RANDOM_POP_TIMEOUT_MS		20000
#define RANDOM_POP_PULSE_MS			500

enum modes
{
	Chase,
	Pulse,
	EvenOdd,
	Chase2,
	RandomPop, //modes after this wont be randomly selected!		
	Idle,
	Fadeout,
	Reset,
};

enum randompopstates
{
	rp_Init,
	rp_Popping,
	rp_Done	
};


//private functions/data

enum modes select_random_mode();
int update_fadeout();
int update_chase();
int update_pulse();
int update_even_odd();
int update_chase2();
int update_random_pop();

static unsigned long	counter_ms = 0;
static unsigned long	g_currentIdleDelay = 0;
static enum modes		g_current_mode;
static enum modes		g_pending_mode;
static unsigned long	g_subcounter_ms;
static int	g_current_light;


void light_mgr_init()
{
	g_current_mode = Reset;
	counter_ms = 0;
	g_current_light = -1;
}

void light_mgr_update(unsigned long delta_ms)
{
	counter_ms += delta_ms;		
	switch (g_current_mode)
	{

		case Chase:
		{
			if (update_chase())
			{
				counter_ms = 0;
				g_current_mode = Reset;
			}
			break;
		}
		case Pulse:
		{
			if (update_pulse())
			{
				counter_ms = 0;
				g_current_mode = Reset;
			}
			
			break;
		}
		case EvenOdd:
		{
			if (update_even_odd())
			{
				counter_ms = 0;
				g_current_mode = Reset;
			}
			break;
		}
		case Chase2:
		{
			if (update_chase2())
			{
				counter_ms = 0;
				g_current_mode = Reset;
			}
			break;
		}
		case RandomPop:
		{
			
			if (update_random_pop())
			{
				counter_ms = 0;
				g_current_mode = Reset;
			}
			
			break;
		}
		case Idle:
		{
			if (counter_ms > g_currentIdleDelay)
			{
				counter_ms = 0;
				g_pending_mode = select_random_mode();
				g_current_mode = Fadeout;
			}
			break;
		}
		case Fadeout:
		{
			if (update_fadeout())
			{
				counter_ms = 0;
				g_current_mode = g_pending_mode;
			}
			break;
		}
		case Reset:
		{
			for (int i = 0; i<6; ++i)
			{
				spwm_set_count(i, 100);
			}
			counter_ms = 0;
#ifdef DEBUG_MODE
			g_currentIdleDelay = 0;
#else			
			g_currentIdleDelay = ((rand() % IDLE_MAX_S) + 1)*1000L;
#endif
			g_current_mode = Idle;
			break;
		}		
		default:
		{
			counter_ms = 0;
			g_current_mode = Idle;
			break;
		}
	}
}


enum modes select_random_mode()
{
#ifdef DEBUG_MODE
	return DEBUG_MODE;
#else
	return rand() % (RandomPop + 1);
#endif
};

int update_fadeout()
{	
	for (int i = 0; i < 6; i++)
	{
		spwm_set_count(i, 100 - counter_ms*100/FADEOUT_TIME_MS);
	}
	return counter_ms > FADEOUT_TIME_MS;
}

int update_chase()
{
	unsigned long cycle_time = counter_ms % CHASE_CYCLE_TIME_MS;
	
	int cyclePercent = (int)((cycle_time * 100UL)/CHASE_CYCLE_TIME_MS);
	for (int i = 0; i < 6; i++)
	{
		int adjusted_cycle_percent = (200 + cyclePercent - i * 17) % 100;
		int light_amount = 100 - abs(100L - ((200L*adjusted_cycle_percent) /100));
		spwm_set_count(i, light_amount);
	}
	return counter_ms > CHASE_TIMEOUT_MS;
}


int update_pulse()
{
	unsigned long cycle_time = counter_ms % PULSE_CYCLE_TIME_MS;
	
	int light_amount = (cycle_time*200)/PULSE_CYCLE_TIME_MS;
	if (light_amount > 100)
	{		
		light_amount = 200 - light_amount;
	}
	
	for (int i = 0; i < 6; i++)
	{
		spwm_set_count(i, light_amount);		
	}
	return counter_ms > PULSE_TIMEOUT_MS;
		
}

int update_even_odd()
{
	unsigned long cycle_time = counter_ms % EVEN_ODD_CYCLE_TIME_MS;
	int half = cycle_time / (EVEN_ODD_CYCLE_TIME_MS/2);
	for (int i = 0; i < 3; ++i)
	{
		spwm_set_count(i*2 + half, 100);
		spwm_set_count(i*2 + ((half + 1)%2), 0);
	}
	return counter_ms > EVEN_ODD_TIMEOUT_MS;
}

int update_chase2()
{
	unsigned long adjusted_chase_cycle_period = CHASE2_CYCLE_TIME_MS - CHASE2_CYCLE_TIME_MS*counter_ms/CHASE2_TIMEOUT_MS;
	unsigned long cycle_time = counter_ms % adjusted_chase_cycle_period;
	int currentLight = (cycle_time*6L) / adjusted_chase_cycle_period;
	unsigned long light_cycle_time = cycle_time %(adjusted_chase_cycle_period/6);
	unsigned long adjusted_cycle_percent = 100L * light_cycle_time / (adjusted_chase_cycle_period/6);
	int light_amount = 100 - abs(100L - ((200L*adjusted_cycle_percent) /100));
	for (int i = 0; i < 6; ++i)
	{
		if (i == currentLight)
		{
			spwm_set_count(i, light_amount);			
		}
		else
		{			
			spwm_set_count(i, 0);
		}
	}
	return counter_ms > CHASE2_TIMEOUT_MS;
}


int update_random_pop()
{
	if (g_current_light == -1)
	{
		g_subcounter_ms = counter_ms;
		
		g_current_light = rand() % 6;
		for (int i = 0; i < 6; i++)
		{
			spwm_set_count(i, 0);
		}
		
	}
	
	unsigned long pulseTime_ms = counter_ms - g_subcounter_ms;
	unsigned long adjusted_pulse_period = RANDOM_POP_PULSE_MS - RANDOM_POP_PULSE_MS*counter_ms/RANDOM_POP_TIMEOUT_MS;

	if (pulseTime_ms > adjusted_pulse_period)
	{
		g_current_light = -1;
	}
	else
	{
		int lightAmount = 100 - abs(100L - ((200L*pulseTime_ms) /adjusted_pulse_period));
		spwm_set_count(g_current_light, lightAmount);		
	}
	
	int done = counter_ms > RANDOM_POP_TIMEOUT_MS;
	if (done)
	{
		g_current_light = -1;
	}
	
	return done;
}
