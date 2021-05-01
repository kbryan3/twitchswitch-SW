/*
 * timer.h
 *
 *  This header file contains the includes and prototypes for the timer functions.
 *
 *  @author Kyle Bryan
 *  @date September 2020
 */


#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

#include "gecko_configuration.h"
#include "native_gecko.h"
#include "em_letimer.h"
#include "em_cmu.h"
#include "app.h"

//define calculations were take from Lecture Material

#if (LOWEST_ENERGY_MODE == 3 && ENABLE_SLEEPING == 1)
 #define PRESCALER_VALUE		8
 #define ACTUAL_CLK_FREQ     1000/PRESCALER_VALUE
#else
 #define PRESCALER_VALUE		4
 #define ACTUAL_CLK_FREQ		38400/PRESCALER_VALUE
#endif

#define VALUE_TO_LOAD (LETIMER_PERIOD_MS*ACTUAL_CLK_FREQ)/1000
#define COMPARE1_FLAG_VALUE ((LETIMER_PERIOD_MS - LETIMER_ON_TIME_MS)*ACTUAL_CLK_FREQ)/1000

extern uint32_t underflow_counter;

//*****************************************************************************
/// Function initializes the LETIMER0
///  @param void
///  @return void
///
//*****************************************************************************
void initLETIMER0();

//*****************************************************************************
/// Function blocks for the inputed amount of microseconds(based on LETIMER0)
///  @param uint32_t delayInMicroseconds
///  @return void
///
//*****************************************************************************
void timerWaitUs(uint32_t delayInMicroseconds);

//*****************************************************************************
/// Function returns the value of the LETIMER0
///  @param void
///  @return uint16_t containing the time remaining in the timer
///
//*****************************************************************************
uint16_t getTimerVal();


#endif /* SRC_TIMER_H_ */
