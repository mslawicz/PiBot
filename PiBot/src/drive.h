/*
 * Drive.h
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_DRIVE_H_
#define SRC_DRIVE_H_

#include "gpio.h"
#include "imu.h"


class Drive
{
public:
	Drive();
	~Drive();
	// starts the control of motors
	void start(void);
	// stops the control of motors
	void stop(void);
private:
	// callback function for gyroscope generated interrupts
	static void gyroInterruptCallback(int gpio, int level, uint32_t tick, void* pDriveObject);
	// pitch control method
	void pitchControl(int level, uint32_t tick);
	// pointer to gyroscope device
	Gyroscope* pGyroscope;
};

#endif /* SRC_DRIVE_H_ */
