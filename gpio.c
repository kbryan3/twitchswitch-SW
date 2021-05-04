/*
 * gpio.c
 *
 *  Created on: Apr 29, 2021
 *      Author: bryan
 */

#include "gpio.h"
#include "em_gpio.h"
#include <string.h>



#define	LED_port 3
#define LED_pin  15
#define DIP0_port 3
#define DIP0_pin 10
#define DIP1_port 3
#define DIP1_pin 11

void gpioInit()
{
	//GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(DIP0_port, DIP0_pin, gpioModePushPull, false);
	GPIO_PinModeSet(DIP1_port, DIP1_pin, gpioModeInput, false);
}

void gpioLedSetOn()
{
	GPIO_PinOutSet(LED_port,LED_pin);
}
void gpioLedSetOff()
{
	GPIO_PinOutClear(LED_port,LED_pin);
}

uint8_t getDIPValue()
{
	uint8_t value;
	uint8_t tmp1, tmp2;
	tmp1 = GPIO_PinInGet(DIP0_port, DIP0_pin);
	tmp2 = GPIO_PinInGet(DIP1_port, DIP1_pin);
	value = ((tmp1 << 1) & 0x02) | (tmp2 & 0x01);

	return value & 0x03;
}

