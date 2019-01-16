/*
 * Drive.cpp
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#include "drive.h"

Drive::Drive()
{
	pGyroInterruptPort = new GPIO(13, PI_INPUT, PI_PUD_UP);
	pGreenLED = new GPIO(23, PI_OUTPUT);	//XXX test
	gpioSetISRFuncEx(pGyroInterruptPort->getNumber(), FALLING_EDGE, 0, Drive::gyroInterruptCallback, this);
	pGreenLED->write(1);	//XXX test
}

Drive::~Drive()
{
	delete pGyroInterruptPort;
	pGreenLED->write(0);	//XXX test
	delete pGreenLED;	//XXX test
}

/*
 * internal drive control function
 * callback function for gyroscope generated interrupts
 * it must be a static method, but the pointer to drive object is passed as an argument
 */
void Drive::gyroInterruptCallback(int gpio, int level, uint32_t tick, void* pObject)
{
	Drive* pDrive = static_cast<Drive*>(pObject);
	pDrive->pGreenLED->toggle();	//XXX test

}


