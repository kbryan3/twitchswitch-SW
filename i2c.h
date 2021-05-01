/*
 * i2c.h
 *
 *  This file contains the function prototypes to access the
 *      Si7021 temp/humidity sensor via i2c
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include <sleep.h>
#include "em_i2c.h"
#include "i2cspm.h"
#include "oscillators.h"
#include "app.h"
#include "gpio.h"
#include "timer.h"
#include "scheduler.h"
#include "ble.h"
#include "log.h"

I2C_TransferSeq_TypeDef si7013_seq;

I2C_TransferSeq_TypeDef accel_seq;

I2CSPM_Init_TypeDef i2c0;

I2CSPM_Init_TypeDef i2c1;

uint8_t i2c_Write_Buffer[19];

uint8_t connection_Open_Flag;

enum temp_sensor_state
{
	TEMP_SENSOR_POWER_ON,
	TEMP_SENSOR_WAIT_FOR_POWER_UP,
	//TEMP_SENSOR_START_WRITE_TRANSFER,
	TEMP_SENSOR_WAIT_FOR_WRITE_COMPLETE,
	TEMP_SENSOR_WAIT_FOR_TEMP_MEASUREMENT,
	//TEMP_SENSOR_START_READ_TRANSFER,
	TEMP_SENSOR_WAIT_FOR_READ_COMPLETE
	//TEMP_SENSOR_POWER_OFF,
	//TEMP_SENSOR_NUM_STATES
};

enum dps310_state
{
	DPS_POWER_ON,
	DPS_WAIT_FOR_POWER_UP,
	DPS_WAIT_FOR_WRITE_COEFF,
	DPS_WAIT_FOR_COEFF,
	DPS_WAIT_FOR_READ_COEFF,
	DPS_SENSOR_WAIT_FOR_READ_COMPLETE,
	DPS_WAIT_FOR_PRS_WRITE,
	DPS_TMP_WRITE,
	DPS_WAIT_FOR_TMP_WRITE,
	DPS_MODE_WRITE,
	DPS_WAIT_FOR_MODE_WRITE,
	DPS_CFG_WRITE,
	DPS_WAIT_FOR_CFG_WRITE,
	DPS_WRITE_TO_ID_REG,
	DPS_WAIT_FOR_ID_REG,
	DPS_READ_ID,
	DPS_WAIT_FOR_ID,
	DPS_CMD_TEMP_MEASUREMENT,
	DPS_WAIT_FOR_TEMP,
	DPS_WRITE_TO_TEMP_CTRL,
	DPS_WAIT_FOR_TEMP_CTRL,
	DPS_READ_TEMP,
	DPS_SENSOR_WAIT_FOR_READ_TEMP,
	DPS_CMD_PRESSURE_MEASUREMENT,
	DPS_WAIT_FOR_PRESSURE,
	DPS_WRITE_TO_MEAS_CTRL,
	DPS_WAIT_FOR_WRITE_CTRL,
	DPS_READ_PRESSURE,
	DPS_SENSOR_WAIT_FOR_READ_PRESSURE


};

//*****************************************************************************
/// Initializes the I2C controller
///
///  @param void
///  @return void
///
//*****************************************************************************
void initI2C0();
void initI2C1();

//*****************************************************************************
/// Write to a register for a device on I2C1
///
///  @param addr - slave address, reg - register to write to, data - reg value
///  @return pass/fail of write
///
//*****************************************************************************
I2C_TransferReturn_TypeDef I2C1InitiateWriteReg(uint8_t addr,uint8_t reg, uint8_t data);

void getPressure(struct gecko_cmd_packet* evt);
I2C_TransferReturn_TypeDef DPS310_InitiateWrite(uint8_t reg);
I2C_TransferReturn_TypeDef DPS310_InitiateWriteReg(uint8_t reg, uint8_t data);
I2C_TransferReturn_TypeDef DPS310_InitiateRead();
I2C_TransferReturn_TypeDef DPS310_InitiateIDRead();
I2C_TransferReturn_TypeDef DPS310_InitiateTemperatureRead();
I2C_TransferReturn_TypeDef DPS310_InitiatePressureRead();

//*****************************************************************************
/// Goes through the process of commanding routines to return a temperature
///
///  @param gecko_cmd_packet containing info on most recent event
///  @return void
///
//*****************************************************************************
void getTemperature(struct gecko_cmd_packet* evt);

//*****************************************************************************
/// Initiates a write transaction to begin a temperature measurement on Si7013
///
///  @param void
///  @return the status of the transfer
///
//*****************************************************************************
I2C_TransferReturn_TypeDef Si7013_InitiateWrite();

//*****************************************************************************
/// Initiates a read transaction to read latest temp measurement of Si7013
///
///  @param void
///  @return the status of the transaction
///
//*****************************************************************************
I2C_TransferReturn_TypeDef Si7013_InitiateRead();

//*****************************************************************************
/// Prints out temperature Value(can only be called after a MeasureTemp
///
///  @param void
///  @return void
///
//*****************************************************************************
void Si7013_getTemp();

//*****************************************************************************
/// Tells the si7013 to do a temperature measurement
///
///  @param void
///  @return void
///
//*****************************************************************************
void Si7013_MeasureTemp();

//*****************************************************************************
/// returns a pointer to the seq data structure
///
///  @param void
///  @return pointer to the I2C sequence(defined in em_i2c.h libary)
///
//*****************************************************************************
I2C_TransferSeq_TypeDef *getPtrToSI7013Sequence();
I2C_TransferSeq_TypeDef *getPtrToAccelSequence();

//*****************************************************************************
/// returns a pointer to the i2c_Write_Buffer
///
///  @param void
///  @return pointer to the i2c_Write_Buffer
///
//*****************************************************************************
uint8_t *getPtrToI2CWriteBuffer();

//*****************************************************************************
/// returns a pointer to the i2c I2CSPM_Init_TypeDef Structure
///
///  @param void
///  @return pointer to the i2c structure
///
//*****************************************************************************
I2CSPM_Init_TypeDef *getI2C0Structure();
I2CSPM_Init_TypeDef *getI2C1Structure();

//*****************************************************************************
/// Enables or Disables the I2C0's SDA, SCL and VCC connections
///  Use to minimize power when not utilizing the I2C peripherals
///  @param void
///  @return void
///
//*****************************************************************************
void sensorEnable();
void sensorDisable();
#endif /* SRC_I2C_H_ */
