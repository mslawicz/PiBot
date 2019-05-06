/*
 * Drive.h
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_DRIVE_H_
#define SRC_DRIVE_H_

#include "motor.h"
#include <vector>

enum MotorName
{
    LeftMotor,
    RightMotor
};

class Drive
{
public:
	Drive();
	~Drive();
	// starts the control of motors
	void start(void);
	// stops the control of motors
	void stop(void);
	void setMotorSpeed(MotorName motorName, float speed);
private:
	std::vector<Motor*> pMotors;    // vector of pointers to motor objects
	std::vector<float> motorSpeed{0.0, 0.0};  // vector of motor speed values
};

#endif /* SRC_DRIVE_H_ */
