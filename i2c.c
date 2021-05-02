/*
 * i2c.c
 *
 *  This file contains the function to access the
 *      Si7021 temp/humidity sensor via i2c
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources emlib functions were leveraged to set register values
 */
#include "i2c.h"

int32_t c0;
int32_t c00;
int32_t c1;
int32_t c01;
int32_t c10;
int32_t c11;
int32_t c20;
int32_t c21;
int32_t c30;
int32_t raw_temp;
float scaled_temp;
float temperature;
int32_t raw_pressure;
float scaled_pressure;
int32_t pressure;


static int32_t twos_complement(int32_t coeff, int8_t num_bits)
{
	if (coeff & (1<<(num_bits -1)))
	{
		coeff -= 1 << num_bits;
	}

	return coeff;
}


void initI2C0()
{
	I2CSPM_Init_TypeDef *i2c0;
	i2c0 = getI2C0Structure();
	I2CSPM_Init_TypeDef i2C_init = I2C0SPM_INIT_DEFAULT;
	*i2c0 = i2C_init;

	//LOG_INFO("I2C0 Clock Frequency: %u",CMU_ClockFreqGet(cmuClock_I2C0));

	//enable I2C Pins
	I2CSPM_Init(i2c0);
	//LOG_INFO("I2C0 Bus Frequency: %u",I2C_BusFreqGet(I2C0));

}

void initI2C1()
{
	I2CSPM_Init_TypeDef *i2c1;
	i2c1 = getI2C1Structure();
	I2CSPM_Init_TypeDef i2C_init = I2C1SPM_INIT_DEFAULT;
	*i2c1 = i2C_init;

	//LOG_INFO("I2C0 Clock Frequency: %u",CMU_ClockFreqGet(cmuClock_I2C0));

	//enable I2C Pins
	I2CSPM_Init(i2c1);
	//LOG_INFO("I2C0 Bus Frequency: %u",I2C_BusFreqGet(I2C0));

}

void getPressure(struct gecko_cmd_packet* evt)
{

	enum dps310_state state;
	static enum dps310_state next_state = DPS_POWER_ON;
	uint8_t* write_buffer;

	state = next_state;
	/*if(BGLIB_MSG_ID(evt->header) != gecko_evt_system_boot_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_evt_gatt_server_characteristic_status_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_evt_le_connection_closed_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_cmd_le_connection_set_parameters_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_cmd_le_connection_disable_slave_latency_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_cmd_le_connection_close_id)*/
	if(BGLIB_MSG_ID(evt->header) == gecko_evt_system_external_signal_id) //&& connection_Open_Flag == 1)
	{
		switch(state)
		{
			case DPS_POWER_ON: //enable sensor
				//LOG_INFO("External Signal: %d", evt->data.evt_system_external_signal.extsignals);
				if(evt->data.evt_system_external_signal.extsignals & LETIMER0_UF_EVENT)
				{
					//sensorEnable();
					timerWaitUs(80000);
					next_state = DPS_WAIT_FOR_POWER_UP;
				}
				break;
			case DPS_WAIT_FOR_POWER_UP:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWrite(0x10);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_WRITE_COEFF;
				}
				break;
			case DPS_WAIT_FOR_WRITE_COEFF:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_WAIT_FOR_COEFF;

				}
				break;
			case DPS_WAIT_FOR_COEFF:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateRead(); //Start Read
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_SENSOR_WAIT_FOR_READ_COMPLETE;
				}
				break;
			case DPS_SENSOR_WAIT_FOR_READ_COMPLETE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					//sensorDisable(); //PowerOFF Sensor
					write_buffer = getPtrToI2CWriteBuffer();
					c0 = (((uint32_t)write_buffer[0]) << 4 | ((((uint32_t)write_buffer[1]) >> 4) & 0x0F));
					c0 = twos_complement(c0, 12);
					c1 = twos_complement((((write_buffer[1]&0x0F)<<8) | write_buffer[2]),12);
			        c00 = (write_buffer[3] << 12) | (write_buffer[4] << 4) | ((write_buffer[5] >> 4) & 0x0F);
			        c00 = twos_complement(c00, 20);

			        c10 = ((write_buffer[5] & 0x0F) << 16) | (write_buffer[6] << 8) | write_buffer[7];
			        c10 = twos_complement(c10, 20);

			        c01 = twos_complement((write_buffer[8] << 8) | write_buffer[9], 16);
			        c11 = twos_complement((write_buffer[10] << 8) | write_buffer[11], 16);
			        c20 = twos_complement((write_buffer[12] << 8) | write_buffer[13], 16);
			        c21 = twos_complement((write_buffer[14] << 8) | write_buffer[15], 16);
			        c30 = twos_complement((write_buffer[16] << 8) | write_buffer[17], 16);
					LOG_INFO("Coefficients Complete");
					DPS310_InitiateWriteReg(0x06, 0x52);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_PRS_WRITE;
				}
				break;
			case DPS_WAIT_FOR_PRS_WRITE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_TMP_WRITE;

				}
				break;
			case DPS_TMP_WRITE:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWriteReg(0x07,0x50);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_TMP_WRITE;

				}
				break;
			case DPS_WAIT_FOR_TMP_WRITE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_MODE_WRITE;

				}
				break;
			case DPS_MODE_WRITE:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWriteReg(0x08,0x07);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_MODE_WRITE;

				}
				break;
			case DPS_WAIT_FOR_MODE_WRITE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_CFG_WRITE;

				}
				break;
			case DPS_CFG_WRITE:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWriteReg(0x09,0x00);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_CFG_WRITE;

				}
				break;
			case DPS_WAIT_FOR_CFG_WRITE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_WRITE_TO_ID_REG;

				}
				break;
			case DPS_WRITE_TO_ID_REG:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWrite(0x08);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_ID_REG;
				}
				break;
			case DPS_WAIT_FOR_ID_REG:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_READ_ID;
				}
				break;
			case DPS_READ_ID:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateIDRead(); //Start Read
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_ID;
				}
				break;
			case DPS_WAIT_FOR_ID:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					write_buffer = getPtrToI2CWriteBuffer();

					LOG_INFO("ID Complete");
					SLEEP_SleepBlockBegin(sleepEM2);
					timerWaitUs(1000);
					next_state = DPS_WRITE_TO_TEMP_CTRL;
				}
				break;
/*			case DPS_CMD_TEMP_MEASUREMENT:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWriteReg(0x08,0x02);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_TEMP;
				}
				break;
			case DPS_WAIT_FOR_TEMP:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_WRITE_TO_TEMP_CTRL;

				}
				break;*/
			case DPS_WRITE_TO_TEMP_CTRL:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWrite(0x03);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_TEMP_CTRL;
				}
				break;
			case DPS_WAIT_FOR_TEMP_CTRL:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_READ_TEMP;
				}
				break;
			case DPS_READ_TEMP:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateTemperatureRead(); //Start Read
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_SENSOR_WAIT_FOR_READ_TEMP;
				}
				break;
			case DPS_SENSOR_WAIT_FOR_READ_TEMP:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					write_buffer = getPtrToI2CWriteBuffer();
					raw_temp = write_buffer[0] << 16 | write_buffer[1] << 8 | write_buffer[2];
					raw_temp = twos_complement(raw_temp, 24);
					scaled_temp = (float)raw_temp/(float)524288;

					temperature = (float)c0*0.5 +(float)c1*scaled_temp;

					LOG_INFO("Temperature Complete");
					SLEEP_SleepBlockBegin(sleepEM2);
					timerWaitUs(1000);
					next_state = DPS_WRITE_TO_MEAS_CTRL;
				}
				break;
/*			case DPS_CMD_PRESSURE_MEASUREMENT:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWriteReg(0x08,0x01);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_PRESSURE;
				}
				break;
			case DPS_WAIT_FOR_PRESSURE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(150000);
					next_state = DPS_WRITE_TO_MEAS_CTRL;

				}
				break;*/
			case DPS_WRITE_TO_MEAS_CTRL:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiateWrite(0x00);
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_WAIT_FOR_WRITE_CTRL;
				}
				break;
			case DPS_WAIT_FOR_WRITE_CTRL:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = DPS_READ_PRESSURE;
				}
				break;
			case DPS_READ_PRESSURE:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					DPS310_InitiatePressureRead(); //Start Read
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = DPS_SENSOR_WAIT_FOR_READ_PRESSURE;
				}
				break;
			case DPS_SENSOR_WAIT_FOR_READ_PRESSURE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					//sensorDisable(); //PowerOFF Sensor
					write_buffer = getPtrToI2CWriteBuffer();
					raw_pressure = write_buffer[0] << 16 | write_buffer[1] << 8 | write_buffer[2];
					raw_pressure = twos_complement(raw_pressure, 24);
					scaled_pressure = (float)raw_pressure/(float)1040384;

					pressure = c00 +
							   scaled_pressure *(c10+scaled_pressure*(c20+scaled_pressure*c30)) +
							   scaled_temp*c01 +
							   scaled_temp *scaled_pressure*(c11+scaled_pressure * c21);
					pressure = pressure/100;
					LOG_INFO("Pressure Complete");
					timerWaitUs(1000);
					next_state = DPS_WRITE_TO_TEMP_CTRL;
				}
				break;
		}
	}
/*	else if(connection_Open_Flag == 0)
	{
		//sensorDisable();
		next_state = TEMP_SENSOR_POWER_ON;
	}
*/
}

void getTemperature(struct gecko_cmd_packet* evt)
{

	enum temp_sensor_state state;
	static enum temp_sensor_state next_state = TEMP_SENSOR_POWER_ON;
	uint8_t* write_buffer;

	state = next_state;
	/*if(BGLIB_MSG_ID(evt->header) != gecko_evt_system_boot_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_evt_gatt_server_characteristic_status_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_evt_le_connection_closed_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_cmd_le_connection_set_parameters_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_cmd_le_connection_disable_slave_latency_id ||
	   BGLIB_MSG_ID(evt->header) != gecko_cmd_le_connection_close_id)*/
	if(BGLIB_MSG_ID(evt->header) == gecko_evt_system_external_signal_id) //&& connection_Open_Flag == 1)
	{
		switch(state)
		{
			case TEMP_SENSOR_POWER_ON: //enable sensor
				//LOG_INFO("External Signal: %d", evt->data.evt_system_external_signal.extsignals);
				if(evt->data.evt_system_external_signal.extsignals & LETIMER0_UF_EVENT)
				{
					//sensorEnable();
					timerWaitUs(80000);
					next_state = TEMP_SENSOR_WAIT_FOR_POWER_UP;
				}
				break;
			case TEMP_SENSOR_WAIT_FOR_POWER_UP:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					Si7013_InitiateWrite();
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = TEMP_SENSOR_WAIT_FOR_WRITE_COMPLETE;
				}
				break;
			case TEMP_SENSOR_WAIT_FOR_WRITE_COMPLETE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					timerWaitUs(10000);
					next_state = TEMP_SENSOR_WAIT_FOR_TEMP_MEASUREMENT;

				}
				break;
			case TEMP_SENSOR_WAIT_FOR_TEMP_MEASUREMENT:
				if(evt->data.evt_system_external_signal.extsignals
						& LETIMER0_WAIT_EXPIRED_EVENT)
				{
					Si7013_InitiateRead(); //Start Read
					SLEEP_SleepBlockBegin(sleepEM2);
					next_state = TEMP_SENSOR_WAIT_FOR_READ_COMPLETE;
				}
				break;
			case TEMP_SENSOR_WAIT_FOR_READ_COMPLETE:
				if(evt->data.evt_system_external_signal.extsignals
						& I2C_TRANSFER_DONE_EVENT)
				{
					SLEEP_SleepBlockEnd(sleepEM2);
					NVIC_DisableIRQ(I2C0_IRQn);
					//sensorDisable(); //PowerOFF Sensor
					write_buffer = getPtrToI2CWriteBuffer();
					uint16_t raw_temp = (write_buffer[0] << 8) | write_buffer[1];
					float temp = (175.72 *(float)raw_temp)/65536.0 - 46.85;
					update_temperature(temp);
					LOG_INFO("Temperature is: %lu degrees C\n", (uint32_t)temp);
					next_state = TEMP_SENSOR_POWER_ON;
				}
				break;
		}
	}
	else if(connection_Open_Flag == 0)
	{
		//sensorDisable();
		next_state = TEMP_SENSOR_POWER_ON;
	}

}

I2C_TransferSeq_TypeDef *getPtrToSI7013Sequence()
{
	return (&si7013_seq);
}

I2C_TransferSeq_TypeDef *getPtrToAccelSequence()
{
	return (&accel_seq);
}

uint8_t *getPtrToI2CWriteBuffer()
{
	return (&i2c_Write_Buffer[0]);
}

I2CSPM_Init_TypeDef *getI2C0Structure()
{
	return(&i2c0);
}

I2CSPM_Init_TypeDef *getI2C1Structure()
{
	return(&i2c1);
}

I2C_TransferReturn_TypeDef DPS310_InitiateWrite(uint8_t reg)
{


	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	write_buffer[0] = reg;
	ptrToSeq = getPtrToAccelSequence();
	ptrToSeq->addr = 0xEE;
	ptrToSeq->flags = I2C_FLAG_WRITE;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 1;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;

}

I2C_TransferReturn_TypeDef I2C1InitiateWrite(uint8_t addr, uint8_t reg)
{


	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	write_buffer[0] = reg;
	ptrToSeq = getPtrToAccelSequence();
	ptrToSeq->addr = addr;
	ptrToSeq->flags = I2C_FLAG_WRITE;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 1;
	NVIC_EnableIRQ(I2C1_IRQn);
	transferStatus = I2C_TransferInit(I2C1, ptrToSeq);
	return transferStatus;

}

I2C_TransferReturn_TypeDef I2C1InitiateSeqRead(uint8_t addr, uint8_t len)
{
	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = addr;
	ptrToSeq->flags = I2C_FLAG_READ;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = len;
	NVIC_EnableIRQ(I2C1_IRQn);
	transferStatus = I2C_TransferInit(I2C1, ptrToSeq);
	return transferStatus;
}



I2C_TransferReturn_TypeDef DPS310_InitiateWriteReg(uint8_t reg, uint8_t data)
{


	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	write_buffer[0] = reg;
	write_buffer[1] = data;
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = 0xEE;
	ptrToSeq->flags = I2C_FLAG_WRITE;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 2;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;

}

I2C_TransferReturn_TypeDef I2C1InitiateWriteReg(uint8_t addr,uint8_t reg, uint8_t data)
{


	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	write_buffer[0] = reg;
	write_buffer[1] = data;
	ptrToSeq = getPtrToAccelSequence();
	ptrToSeq->addr = addr;
	ptrToSeq->flags = I2C_FLAG_WRITE;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 2;
	NVIC_EnableIRQ(I2C1_IRQn);
	transferStatus = I2C_TransferInit(I2C1, ptrToSeq);
	return transferStatus;

}



I2C_TransferReturn_TypeDef DPS310_InitiateRead()
{


	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = 0xEE;
	ptrToSeq->flags = I2C_FLAG_READ;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 19;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;

}

I2C_TransferReturn_TypeDef DPS310_InitiateIDRead()
{
	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = 0xEE;
	ptrToSeq->flags = I2C_FLAG_READ;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 1;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;
}

I2C_TransferReturn_TypeDef DPS310_InitiateTemperatureRead()
{
	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = 0xEE;
	ptrToSeq->flags = I2C_FLAG_READ;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 3;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;
}

I2C_TransferReturn_TypeDef DPS310_InitiatePressureRead()
{
	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = 0xEE;
	ptrToSeq->flags = I2C_FLAG_READ;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 3;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;
}

I2C_TransferReturn_TypeDef Si7013_InitiateWrite()
{


	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	write_buffer[0] = 0xF3;
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = 0x80;
	ptrToSeq->flags = I2C_FLAG_WRITE;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 1;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;

}

I2C_TransferReturn_TypeDef Si7013_InitiateRead()
{


	I2C_TransferSeq_TypeDef *ptrToSeq;
	uint8_t *write_buffer;
	I2C_TransferReturn_TypeDef transferStatus;
	write_buffer = getPtrToI2CWriteBuffer();
	ptrToSeq = getPtrToSI7013Sequence();
	ptrToSeq->addr = 0x80;
	ptrToSeq->flags = I2C_FLAG_READ;
	ptrToSeq->buf[0].data = write_buffer;
	ptrToSeq->buf[0].len = 2;
	NVIC_EnableIRQ(I2C0_IRQn);
	transferStatus = I2C_TransferInit(I2C0, ptrToSeq);
	return transferStatus;

}

void Si7013_MeasureTemp()
{

	I2C_TransferSeq_TypeDef measureCMD;
	int32_t result;
	uint8_t cmd[1];
	//uint8_t read_data[2];


	cmd[0] = 0xF3; //take temp measurement
	measureCMD.addr = 0x80;
	measureCMD.flags = I2C_FLAG_WRITE;
	measureCMD.buf[0].data = cmd;
	measureCMD.buf[0].len = 1;
	//measureCMD.buf[1].data = read_data;
	//measureCMD.buf[1].len = 1;

	result = I2CSPM_Transfer(I2C0, &measureCMD);
	if(result != i2cTransferDone)
	{
		LOG_INFO("Transfer Unsuccessful");
	}
	else
	{
		//LOG_INFO("Transfer Successful");
	}
}

void Si7013_getTemp()
{

	uint16_t raw_temp;
	float temp;
	I2C_TransferSeq_TypeDef measureCMD;
	int32_t result;
	//uint8_t reg_read[1]; //take temp measurement
	uint8_t read_data[2];


	//reg_read[0] = 0xE0;
	measureCMD.addr = 0x80;
	measureCMD.flags = I2C_FLAG_READ;
	measureCMD.buf[0].data = read_data;
	measureCMD.buf[0].len = 2;
	//measureCMD.buf[1].data = read_data;
	//measureCMD.buf[1].len = 2;

	result = I2CSPM_Transfer(I2C0, &measureCMD);
	if(result != i2cTransferDone)
	{
		LOG_INFO("Transfer Unsuccessful");
	}
	else
	{
		//LOG_INFO("Transfer Successful");
	}
	raw_temp = (read_data[0] << 8) | read_data[1];
	temp = (175.72 *(float)raw_temp)/65536.0 - 46.85;
	LOG_INFO("Temperature is: %lu degrees C\n", (uint32_t)temp);
}



void sensorEnable()
{
	gpioSensorEnableSetOn();
}

void sensorDisable()
{
	gpioSensorEnableSetOff();
}
