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

void gpioInit()
{
	//GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED_port, LED_pin, gpioModePushPull, false);

}

void gpioLedSetOn()
{
	GPIO_PinOutSet(LED_port,LED_pin);
}
void gpioLedSetOff()
{
	GPIO_PinOutClear(LED_port,LED_pin);
}

