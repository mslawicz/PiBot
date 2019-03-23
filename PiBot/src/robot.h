/*
 * robot.h
 *
 *  Created on: 23 mar 2019
 *      Author: Marcin
 *
 *  This is the main object of the robot kinematics
 */

#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_

#include "drive.h"

class Robot
{
public:
    Robot();
    ~Robot();
    // starts the control of robot
    void start(void);
    // stops the control of robot
    void stop(void);
private:
    Drive* pDrive;
};

#endif /* SRC_ROBOT_H_ */
