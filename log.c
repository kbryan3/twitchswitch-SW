/*
 * log.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Dan Walkes
 *
 *      loggerGetTimestamp() was updated by Kyle Bryan
 */

#include "retargetserial.h"
#include "log.h"
#include <stdbool.h>

#include "irq.h"

#if INCLUDE_LOGGING
/**
 * @return a timestamp value for the logger, typically based on a free running timer.
 * This will be printed at the beginning of each log message.
 */
uint32_t loggerGetTimestamp(void)
{
	//return timerGetRunTimeMilliseconds();
	//return 0;
	uint32_t timerVal;
	uint32_t milliseconds;
    #ifdef MY_USE_SYSTICKS
	   return getSysTicks();
    #else
	   timerVal = (uint32_t)getTimerVal();
	   if(timerVal == 0)
	   {
		   return (3000 * underflow_counter);
	   }
	   //total milliseconds passed(number of ticks + overflow counter)
	   milliseconds = ((VALUE_TO_LOAD - timerVal)*1000)/(ACTUAL_CLK_FREQ)
			   +(3000 * underflow_counter);
	   return milliseconds;
    #endif
}

/**
 * Initialize logging for Blue Gecko.
 * See https://www.silabs.com/community/wireless/bluetooth/forum.topic.html/how_to_do_uart_loggi-ByI
 */
void logInit(void)
{
	RETARGET_SerialInit();
	/**
	 * See https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__RetargetIo.html#ga9e36c68713259dd181ef349430ba0096
	 * RETARGET_SerialCrLf() ensures each linefeed also includes carriage return.  Without it, the first character is shifted in TeraTerm
	 */
	RETARGET_SerialCrLf(true);
	LOG_INFO("Initialized Logging");
}

/**
 * Block for chars to be flushed out of the serial port.  Important to do this before entering SLEEP() or you may see garbage chars output.
 */
void logFlush(void)
{
	RETARGET_SerialFlush();
}
#endif
