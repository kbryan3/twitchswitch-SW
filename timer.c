/*
 * timer.c
 *
 *  This file contains a method to setup the timer as well as get the timer val
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources emlib functions were leveraged to set register values
 */

#include "timer.h"

static LETIMER_Init_TypeDef letimer = LETIMER_INIT_DEFAULT;

void initLETIMER0()
{
	//initialize the the underflow counter
	underflow_counter = 0;
	//setup prescaler
	CMU_ClockPrescSet(cmuClock_LETIMER0, (CMU_ClkPresc_TypeDef)PRESCALER_VALUE);

	//select the clock source for LETIMER0
#if (LOWEST_ENERGY_MODE == 3 && ENABLE_SLEEPING == 1)
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
#else
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
#endif



	//enable clock to LETIMER0
	CMU_ClockEnable(cmuClock_LETIMER0, true);

	//Set compare0 value for top of timer
	LETIMER_CompareSet(LETIMER0, 0, (uint32_t)VALUE_TO_LOAD);
	//Set compare1 value for interrupt
	//LETIMER_CompareSet(LETIMER0, 1, (uint32_t)COMPARE1_FLAG_VALUE);

	//compare0 value is set after underflow and initial top value is set
	letimer.comp0Top = true;
	letimer.topValue = (uint32_t)VALUE_TO_LOAD;


	//Set Interrupt Flags for LETIMER
	LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF);


	//Enable LETIMER
	LETIMER_Init(LETIMER0, &letimer);

}

void timerWaitUs(uint32_t delayInMicroseconds)
{
	uint16_t endTime, overrunTime;
	uint16_t startTime = getTimerVal();
	uint32_t delay = ((uint64_t)delayInMicroseconds * ACTUAL_CLK_FREQ)/1000000; //how many ticks to delay
	if(delay >= (uint32_t)VALUE_TO_LOAD)
	{
		//LOG() input is out of range, max delay is 3 seconds
		return;
	}
	else
	{
		//check if there is going to be an overrun in the Timer
		if((uint16_t)delay > startTime)
		{
			//if overrun calculate the overrun and subtract from the top value
			overrunTime = (uint16_t)0xFFFF -(uint16_t)(startTime-(uint16_t)delay);
			endTime = (uint16_t)((uint32_t)VALUE_TO_LOAD-overrunTime);
		}
		else
		{
			endTime = startTime-(uint16_t)delay;
		}
		//setup interrupt to occur when delay is met
		LETIMER_CompareSet(LETIMER0, 1, (uint32_t)endTime);
		LETIMER_IntEnable(LETIMER0, LETIMER_IF_COMP1);


		//while(getTimerVal()!=endTime);//block until time is reached
	}
}

uint16_t getTimerVal()
{
	return (uint16_t)LETIMER_CounterGet(LETIMER0);
}
