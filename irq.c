/*
 * irq.c
 *
 *  This file contains the handler for the LETIMER0
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources emlib functions were leveraged to set register values
 *  		 Code from lecture slides was leveraged for I2C0_IRQHandler
 */

#include "irq.h"
//static uint32_t led_state = 0;
void LETIMER0_IRQHandler(void)
{
	uint32_t flags = 0;
	//check for cause of interrupt
	flags = LETIMER_IntGet(LETIMER0);
	uint32_t flags_en = LETIMER_IntGetEnabled(LETIMER0);
	LETIMER_IntClear(LETIMER0, flags);


	//clear interrupt flag and take action
	if(flags_en&2)
	{
		LETIMER_IntDisable(LETIMER0, LETIMER_IF_COMP1);
		CORE_DECLARE_IRQ_STATE;
		CORE_ENTER_CRITICAL();
		schedulerSetEvent(LETIMER0_WAIT_EXPIRED_EVENT);
		CORE_EXIT_CRITICAL();
	}
	if(flags_en&4)
	{
		CORE_DECLARE_IRQ_STATE;
		CORE_ENTER_CRITICAL();
		underflow_counter++;
		schedulerSetEvent(LETIMER0_UF_EVENT);
		CORE_EXIT_CRITICAL();
	}


}

void I2C0_IRQHandler(void) {
	// get the IRQ flags and'ed with what's enabled,
	// see why we got here
	 //uint32_t i2c_flags = I2C0->IF & I2C0->IEN;
	 //uint32_t i2c_state = I2C0->STATE;
	 //uint32_t i2c_status = I2C0->STATUS;
	 // Shepherds the IC2 transfer along
	 uint16_t transferStatus = I2C_Transfer(I2C0);
	 if( transferStatus == i2cTransferDone )
	 {
		 //enters critical section
		 CORE_DECLARE_IRQ_STATE;
		 CORE_ENTER_CRITICAL();
		 //set event for get temperature state machine
		 schedulerSetEvent(I2C_TRANSFER_DONE_EVENT);
		 // exit critical section
		 CORE_EXIT_CRITICAL();
	 }
}
