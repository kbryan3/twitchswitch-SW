/*
 * scheduler.h
 *
 *  This header file contains the includes and prototypes for the scheduler functions.
 *
 *  @author Kyle Bryan
 *  @date September 2020
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include <stdbool.h>
#include "em_core.h"
#include "native_gecko.h"

//enum for different scheduler events
enum schedule_events{
	LETIMER0_UF_EVENT = 1,
	LETIMER0_WAIT_EXPIRED_EVENT = 2,
	I2C_TRANSFER_DONE_EVENT = 4
};
//prototypes

//*****************************************************************************
/// Function sets the scheduler event for a given event(schedule_events enum)
///   This must be called from a critical section
///  @param enum schedule_events event event that has occurred and should be set
///  @return void
///
//*****************************************************************************
void schedulerSetEvent(enum schedule_events event);

//*****************************************************************************
/// Function returns an event that has been flagged
///  @param void
///  @return an integer value correlated to event that has occurred
///
//*****************************************************************************
//uint32_t get_event();

//*****************************************************************************
/// Function checks if any events have occured
///  @param void
///  @return boolean(True: there is an event flag, False: no events flagged
///
//*****************************************************************************
//bool events_present();


#endif /* SRC_SCHEDULER_H_ */
