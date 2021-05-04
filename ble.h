/*
 * ble.h
 *
 *  This file contains the function associated with ble including the
 *     main ble state machine to handle ble events
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources -used macros from native_gecko.h
 *             -leveraged the Simplicity SOC Thermometer example
 *
 */
#ifndef SRC_BLE_H_
#define SRC_BLE_H_

#include "app.h"
#include "native_gecko.h"
#include "log.h"
#include "scheduler.h"
#include "accel.h"

/* GATT database */
#include "gatt_db.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "infrastructure.h"
#include "gecko_ble_errors.h"


#define TX_POWER_MIN -300 //-30.0dbm
#define TX_POWER_MAX  100 //10.0dbm

extern uint8_t connection_Open_Flag; //flag for powering/taking temperature



//*****************************************************************************
/// Handles events related to the ble connection
///
///  @param a gecko_cmd_packet containing info on latest event
///  @return void
///
//*****************************************************************************
void handle_ble_event(struct gecko_cmd_packet* evt, uint8_t mode);

//*****************************************************************************
/// Places temperature value into the GATT Server Temp Characteristic
///
///  @param float temperature value to be written into the temp characteristic
///  @return void
///
//*****************************************************************************
void update_temperature(float temp);


#endif /* SRC_BLE_H_ */
