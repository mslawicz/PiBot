/*
 * Drive.cpp
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#include "drive.h"

GPIO GreenLED(23, PI_OUTPUT);

Drive::Drive()
{
	pInterruptSignal = new GPIO(13, PI_INPUT, PI_PUD_UP);
	gpioSetISRFunc(13, FALLING_EDGE, 0, Drive::gyroInterruptCallback);
	GreenLED.write(1);
}

Drive::~Drive()
{
	delete pInterruptSignal;
	GreenLED.write(0);
}

void Drive::gyroInterruptCallback(int gpio, int level, uint32_t tick)
{
	GreenLED.toggle();

}


