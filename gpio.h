/*
 * gpio.h
 *
 *  Created on: Apr 29, 2021
 *      Author: bryan
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include "app.h"

void gpioInit();
void gpioLedSetOn();
void gpioLedSetOff();
uint8_t getDIPValue();

#endif /* GPIO_H_ */
