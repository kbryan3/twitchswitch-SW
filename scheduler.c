/*
 * scheduler.c
 *
 *  This file contains a method to set scheduler events and get events
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources emlib functions were leveraged to set register values
 */

#include "scheduler.h"
//variables

/*
 * scheduler_Events is a set of flags for different events
 *   up to 32(1 for each bit)
 *   bit 0: LETIMER0 UF Event
 */
//static uint32_t scheduler_Events;

void schedulerSetEvent(enum schedule_events event)
{
	//set bit 0 to flag an LETIMER0 UF Interrupt occurred
	if(event == LETIMER0_UF_EVENT)
	{
		gecko_external_signal(LETIMER0_UF_EVENT);
	}
	//set bit 1 to flag an LETIMER0 COMP1 Interrupt occurred
	else if(event == LETIMER0_WAIT_EXPIRED_EVENT)
	{
		gecko_external_signal(LETIMER0_WAIT_EXPIRED_EVENT);
	}
	//set bit 2 to flag an I2C Transfer Done Interrupt occurred
	else if(event == I2C_TRANSFER_DONE_EVENT)
	{
		gecko_external_signal(I2C_TRANSFER_DONE_EVENT);
	}
}

//not needed for assignment 5
/*bool events_present()
{
	return (bool)scheduler_Events;
}

uint32_t get_event()
{
	uint32_t event = 0;
	if(events_present())
	{
		if(scheduler_Events & LETIMER0_WAIT_EXPIRED_EVENT)
		{
			CORE_DECLARE_IRQ_STATE;
			CORE_ENTER_CRITICAL();
			scheduler_Events &= ~LETIMER0_WAIT_EXPIRED_EVENT; //clears event
			CORE_EXIT_CRITICAL();
			event = LETIMER0_WAIT_EXPIRED_EVENT;
		}
		else if(scheduler_Events & LETIMER0_UF_EVENT)
		{
			CORE_DECLARE_IRQ_STATE;
			CORE_ENTER_CRITICAL();
			scheduler_Events &= ~LETIMER0_UF_EVENT;
			CORE_EXIT_CRITICAL();
			event = LETIMER0_UF_EVENT;
		}
		else if(scheduler_Events & I2C_TRANSFER_DONE_EVENT)
		{
			CORE_DECLARE_IRQ_STATE;
			CORE_ENTER_CRITICAL();
			scheduler_Events &= ~I2C_TRANSFER_DONE_EVENT;
			CORE_EXIT_CRITICAL();
			event = I2C_TRANSFER_DONE_EVENT;
		}

	}

	return event;
}*/
