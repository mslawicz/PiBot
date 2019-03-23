/*
 * robot.cpp
 *
 *  Created on: 23 mar 2019
 *      Author: Marcin
 */

#include "robot.h"
#include "program.h"
#include "udp.h"
#include <sstream>

Robot::Robot()
{
    pDrive = new Drive;
    telemetryEnabled = false;
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

/*
 * telemetry handler to be called in gyroscope interrupts
 */
void Robot::telemetryHandler(void)
{
    if(telemetryEnabled)
    {
        std::stringstream textStream;
        textStream << pDrive->getSensorPitchAngularRate();
        textStream << "\n";
        Program::getInstance().getUdpClient()->sendData(textStream.str());
    }
}
