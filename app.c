/***************************************************************************//**
 * @file app.c
 * @brief Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Bluetooth stack headers */
#include <sleep.h>
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

#include "app.h"


uint8_t initAccelFlag;
/* Main application */
void appMain(gecko_configuration_t *pconfig)
{
#if DISABLE_SLEEP > 0
  pconfig->sleep.flags = 0;
#endif
  /* Initialize stack */
  gecko_init(pconfig);
  /* Initialize debug prints. Note: debug prints are off by default. See DEBUG_LEVEL in app.h */
  initLog();
  initAccelFlag = 0;
  init_LFRCO();
  gpioInit();
  gpioLedSetOn();
  initLETIMER0();
  /*Initialize logging */
  logInit();
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
#if ENABLE_SLEEPING == 1
  SLEEP_Init_t sleepConfig = {0};
  SLEEP_InitEx(&sleepConfig);
#if LOWEST_ENERGY_MODE == 0
  SLEEP_SleepBlockBegin(sleepEM1);
#elif LOWEST_ENERGY_MODE == 1
  SLEEP_SleepBlockBegin(sleepEM2);
#elif LOWEST_ENERGY_MODE == 2
  SLEEP_SleepBlockBegin(sleepEM3);
#else
  //SLEEP_SleepBlockBegin(sleepEM4);
#endif
#endif
  while (1) {

	  struct gecko_cmd_packet* evt;
      if(!gecko_event_pending())
      {
    	  logFlush();
      }

      evt = gecko_wait_event();
      handle_ble_event(evt);
      if(initAccelFlag == 0)
      {
    	  initAccelFlag = accelInit(evt);
      }
//	  getTemperature(evt);
//	  getPressure(evt);
  }
}


