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
#include <vector>


class Drive
{
public:
	Drive();
	~Drive();
	// starts the control of motors
	void start(void);
	// stops the control of motors
	void stop(void);
	float getSensorPitchAngularRate(void) const {return sensorPitchAngularRate;}
private:
	// callback function for gyroscope generated interrupts
	static void gyroInterruptCallback(int gpio, int level, uint32_t tick, void* pDriveObject);
	// pitch control method
	void pitchControl(int level, uint32_t tick);
	// pointer to gyroscope device
	Gyroscope* pGyroscope;
	float sensorPitchAngularRate;
	float targetPitchAngularRate;
	// pointer to pitch control loop PID
	PID* pPitchPID;
	float pitchControlSpeed;    // calculated motor speed for pitch control (for both motors)
	std::vector<Motor*> pMotors;    // vector of pointers to motor objects
	std::vector<float> motorSpeed{0.0, 0.0};  // vector of motor speed values
};

#endif /* SRC_DRIVE_H_ */
