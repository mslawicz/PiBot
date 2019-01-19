/*
 * Drive.cpp
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#include "drive.h"

Drive::Drive()
{
	pIMU = new IMU;

	pGreenLED = new GPIO(23, PI_OUTPUT);	//XXX test
	pGreenLED->write(1);	//XXX test
}

Drive::~Drive()
{
	delete pIMU;

	pGreenLED->write(0);	//XXX test
	delete pGreenLED;	//XXX test
}

/*
 * starts the control of motors
 */
void Drive::start(void)
{
	// enable gyroscope interrupts
	gpioSetISRFuncEx(pIMU->getGyroInterruptPort(), RISING_EDGE, 0,
			Drive::gyroInterruptCallback, this);
}

/*
 * stops the control of motors
 */
void Drive::stop(void)
{
	// disable gyroscope interrupts
	gpioSetISRFuncEx(pIMU->getGyroInterruptPort(), RISING_EDGE, 0, nullptr,
			this);
}

/*
 * callback function for gyroscope generated interrupts
 * it must be a static method, but the pointer to drive object is passed as an argument
 */
void Drive::gyroInterruptCallback(int gpio, int level, uint32_t tick,
		void* pDriveObject)
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


