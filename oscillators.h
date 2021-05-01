/*
 * oscillators.h
 *
 *  This file contains a method to setup the oscillators
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources emlib functions were leveraged to set register values
 */

#ifndef SRC_OSCILLATORS_H_
#define SRC_OSCILLATORS_H_

#include "em_cmu.h"


//prototypes

//*****************************************************************************
/// function initializes the ultra low frequency oscillator
///  @param void
///  @return void
///
//*****************************************************************************
void init_ULFRCO();

//*****************************************************************************
/// function initializes the low frequency oscillator
///  @param void
///  @return void
///
//*****************************************************************************
void init_LFXO();
void init_LFRCO();

//*****************************************************************************
/// function initializes the high frequency oscillator
///  @param void
///  @return void
///
//*****************************************************************************
void init_HFXO();

#endif /* SRC_OSCILLATORS_H_ */
