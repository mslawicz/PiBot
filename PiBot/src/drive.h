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
#include "pid.h"
#include "motor.h"


class Drive
{
public:
	Drive();
	~Drive();
	// starts the control of motors
	void start(void);
	// stops the control of motors
	void stop(void);
	float getTestValue(void) const {return calculatedSpeed;}    //XXX test
private:
	// callback function for gyroscope generated interrupts
	static void gyroInterruptCallback(int gpio, int level, uint32_t tick, void* pDriveObject);
	// pitch control method
	void pitchControl(int level, uint32_t tick);
	// pointer to gyroscope device
	Gyroscope* pGyroscope;
	float pitchAngularRate;
	// pointer to pitch control loop PID
	PID* pPitchPID;
	float calculatedSpeed; //XXX test
	std::vector<Motor*> pMotors;
};

#endif /* SRC_DRIVE_H_ */
