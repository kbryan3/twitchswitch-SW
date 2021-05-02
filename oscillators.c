/*
 * oscillators.c
 *
 *  This file contains a method to setup the oscillators
 *
 *  @author Kyle Bryan
 *  @date September 2020
 *  @resources emlib functions were leveraged to set register values
 */

#include "oscillators.h"

void init_ULFRCO()
{
	//enable oscillator LFXO
	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
}

void init_LFRCO()
{
	//enable oscillator LFXO
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
	CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
}

void init_LFXO()
{
	//enable oscillator LFXO
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
}

void init_HFXO()
{
	//enable oscillator HFXO
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
}
