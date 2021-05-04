/*
 * accel.c
 *
 *  Created on: Apr 30, 2021
 *      Author: team twitchswitch
 */
#include "accel.h"
int16_t x,y,z;
static float cx,cy,cz = 10;

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
					timerWaitUs(1000);
					next_state = ACCEL_POWER_ON;
					init = 1;
				}
				break;
		}
	}
	return init;
}

void __attribute__((optimize("O0"))) readAccel()
{

	enum accel_read_state state;
	uint8_t* accel_buffer;
	uint16_t acceleration;
	static enum accel_read_state next_state = DPS_WRITE_TO_X_REG;
	state = next_state;
	/*if(BGLIB_MSG_ID(evt->header) == gecko_evt_system_external_signal_id) //&& connection_Open_Flag == 1)
	{
		switch(state)
		{
			case DPS_WRITE_TO_X_REG:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					I2C1InitiateWrite(ACCEL_ADDR,0x01);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = WAIT_FOR_X_REG;
				}
				break;
			case WAIT_FOR_X_REG:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C1_IRQn);
					timerWaitUs(10000);
					next_state = READ_ACCEL;
				}
				break;
			case READ_ACCEL:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					I2C1InitiateSeqRead(ACCEL_ADDR,6); //Start Read
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = WAIT_FOR_READ_ACCEL;
				}
				break;
			case WAIT_FOR_READ_ACCEL:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C1_IRQn);
					accel_buffer = getPtrToI2CWriteBuffer();
					x = ((short)(accel_buffer[0]<<8 | accel_buffer[1])) >> 4;
					y = ((short)(accel_buffer[2]<<8 | accel_buffer[3])) >> 4;
					z = ((short)(accel_buffer[4]<<8 | accel_buffer[5])) >> 4;
					cx = ((float) x / (float)(1<<11) * (float)(2))*9.807;
					cy = ((float) y / (float)(1<<11) * (float)(2))*9.807;
					cz = ((float) z / (float)(1<<11) * (float)(2))*9.807;
					SLEEP_SleepBlockBegin(sleepEM2);
					timerWaitUs(10000);
					next_state = DPS_WRITE_TO_X_REG;
				}
				break;
		}
	}*/
	int i =0;
	while(i < 1000)
	{
		i++;
	}
	I2C1InitiateWrite(ACCEL_ADDR,0x01);
	while(I2C_TRANSFER_DONE == 0);
	I2C_TRANSFER_DONE = 0;
	I2C1InitiateSeqRead(ACCEL_ADDR,6);
	while(I2C_TRANSFER_DONE == 0);
	I2C_TRANSFER_DONE = 0;
	accel_buffer = getPtrToI2CWriteBuffer();
	x = ((short)(accel_buffer[0]<<8 | accel_buffer[1])) >> 4;
	y = ((short)(accel_buffer[2]<<8 | accel_buffer[3])) >> 4;
	z = ((short)(accel_buffer[4]<<8 | accel_buffer[5])) >> 4;
	cx = ((float) x / (float)(1<<11) * (float)(2))*9.807;
	cy = ((float) y / (float)(1<<11) * (float)(2))*9.807;
	cz = ((float) z / (float)(1<<11) * (float)(2))*9.807;
}

float getAccel()
{
	return cz;
}


