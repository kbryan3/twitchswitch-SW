/*
 * acell.h
 *
 *  Created on: Apr 30, 2021
 *      Author: bryan
 */

#ifndef ACCEL_H_
#define ACCEL_H_

#include "i2c.h"
#include "timer.h"
#include "scheduler.h"
#include "ble.h"

#define ACCEL_ADDR 0x38 //MMA8452Q Address

enum accel_init_state
{
	ACCEL_POWER_ON,
	ACCEL_WAIT_FOR_POWER_UP,
	ACCEL_WAIT_REG1_WRITE_COMPLETE,
	ACCEL_WRITE_DATA_CFG,
	ACCEL_WAIT_CGF_WRITE_COMPLETE
};

enum accel_read_state
{
	DPS_WRITE_TO_X_REG,
	WAIT_FOR_X_REG,
	READ_ACCEL,
	WAIT_FOR_READ_ACCEL
};

uint8_t accelInit(struct gecko_cmd_packet* evt);
void readAccel(struct gecko_cmd_packet* evt);

#endif /* ACCEL_H_ */
