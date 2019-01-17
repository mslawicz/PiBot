/*
 * Drive.cpp
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#include "drive.h"

Drive::Drive()
{
	pIMUInterruptPort = new GPIO(13, PI_INPUT, PI_PUD_UP);
	// interface to LSM9DS1 - Accelerometer and gyroscope
	pIMUInterface = new I2C(I2C1, 0x6B);
	gpioSetISRFuncEx(pIMUInterruptPort->getNumber(), FALLING_EDGE, 0, Drive::giroInterruptCallback, this);

	pGreenLED = new GPIO(23, PI_OUTPUT);	//XXX test
	pGreenLED->write(1);	//XXX test
}

Drive::~Drive()
{
	delete pIMUInterface;
	delete pIMUInterruptPort;

	pGreenLED->write(0);	//XXX test
	delete pGreenLED;	//XXX test
}

/*
 * callback function for gyroscope generated interrupts
 * it must be a static method, but the pointer to drive object is passed as an argument
 */
void Drive::giroInterruptCallback(int gpio, int level, uint32_t tick, void* pDriveObject)
{
	static_cast<Drive*>(pDriveObject)->pitchControl(level, tick);

}

/*
 * pitch control method
 */
void Drive::pitchControl(int level, uint32_t tick)
{
	pGreenLED->toggle();	//XXX test
}

