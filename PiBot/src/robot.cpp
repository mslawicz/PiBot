/*
 * robot.cpp
 *
 *  Created on: 23 mar 2019
 *      Author: Marcin
 */

#include "robot.h"

Robot::Robot()
{
    pDrive = new Drive;

}

Robot::~Robot()
{
    delete pDrive;
}

/*
 * starts the control of robot
 */
void Robot::start(void)
{
    pDrive->start();
}

/*
 * stops the control of robot
 */
void Robot::stop(void)
{
    pDrive->stop();
}
