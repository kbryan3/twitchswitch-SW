/*
 * irq.h
 *
 *  This file contains the handler for the LETIMER0
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources emlib functions were leveraged to set register values
 */

#ifndef SRC_IRQ_H_
#define SRC_IRQ_H_

#include "app.h"
#include "em_letimer.h"
#include "em_core.h"
#include "em_i2c.h"
#include "gecko_configuration.h"
#include "gpio.h"
#include "timer.h"
#include "native_gecko.h"
#include "scheduler.h"

//global variable
uint32_t underflow_counter;
#endif /* SRC_IRQ_H_ */
