/*
 * accel.c
 *
 *  Created on: Apr 30, 2021
 *      Author: team twitchswitch
 */
#include "accel.h"

uint8_t accelInit(struct gecko_cmd_packet* evt)
{
	uint8_t init = 0;
	enum accel_init_state state;
	static enum accel_init_state next_state = ACCEL_POWER_ON;
	state = next_state;
	if(BGLIB_MSG_ID(evt->header) == gecko_evt_system_external_signal_id) //&& connection_Open_Flag == 1)
	{
		switch(state)
		{
			case ACCEL_POWER_ON:
				//LOG_INFO("External Signal: %d", evt->data.evt_system_external_signal.extsignals);
				if(evt->data.evt_system_external_signal.extsignals & LETIMER0_UF_EVENT)
				{
					initI2C1();
					timerWaitUs(550);
					next_state = TEMP_SENSOR_WAIT_FOR_POWER_UP;
				}
				break;
			case ACCEL_WAIT_FOR_POWER_UP:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					I2C1InitiateWriteReg((uint8_t)ACCEL_ADDR, 0x2A, 0x11);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = ACCEL_WAIT_REG1_WRITE_COMPLETE;
				}
				break;
			case ACCEL_WAIT_REG1_WRITE_COMPLETE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C1_IRQn);
					timerWaitUs(10000);
					next_state = ACCEL_WRITE_DATA_CFG;

				}
				break;
			case ACCEL_WRITE_DATA_CFG:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					I2C1InitiateWriteReg((uint8_t)ACCEL_ADDR, 0x0E, 0x00);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = ACCEL_WAIT_CGF_WRITE_COMPLETE;
				}
				break;
			case ACCEL_WAIT_CGF_WRITE_COMPLETE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C1_IRQn);
					next_state = ACCEL_POWER_ON;
					init = 1;
				}
				break;
		}
	}
	return init;
}


