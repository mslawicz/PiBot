/*
 * Drive.h
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_DRIVE_H_
#define SRC_DRIVE_H_

#include "gpio.h"
#include "i2c.h"

class Drive
{
public:
	Drive();
	~Drive();
private:
	// pitch control callback function for gyroscope generated interrupts
	static void IMUInterruptCallback(int gpio, int level, uint32_t tick, void* pObject);
	// port for gyroscope interrupts
	GPIO* pIMUInterruptPort;
	// I2C interface to gyroscope and accelerometer
	I2C* pIMUInterface;

	GPIO* pGreenLED;	// XXX for test only
};

#endif /* SRC_DRIVE_H_ */
