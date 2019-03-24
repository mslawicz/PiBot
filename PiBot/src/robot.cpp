/*
 * robot.cpp
 *
 *  Created on: 23 mar 2019
 *      Author: Marcin
 */

#include "robot.h"
#include "program.h"
#include "udp.h"
#include "logger.h"
#include <sstream>

Robot::Robot()
{
    pDrive = new Drive;
    telemetryEnabled = false;
    exitHandler = true;
    pTelemetryHandlerThread = nullptr;
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
    Logger::getInstance().logEvent(INFO, "robot start request");
    exitHandler = false;
    pTelemetryHandlerThread = new std::thread(&Robot::telemetryHandler, this);
    pDrive->start();
}

/*
 * stops the control of robot
 */
void Robot::stop(void)
{
    Logger::getInstance().logEvent(INFO, "robot stop request");
    pDrive->stop();
    exitHandler = true;
    telemetryNotify();
    pTelemetryHandlerThread->join();
    delete pTelemetryHandlerThread;
}

/*
 * telemetry handler to be called in a new thread
 */
void Robot::telemetryHandler(void)
{
    Logger::getInstance().logEvent(INFO, "telemetry handler started");
    do
    {
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(telemetryHandlerMutex);
        telemetryEvent.wait(lock, [this]() {return (telemetryEnabled || exitHandler); });


        std::stringstream textStream;
        textStream << pDrive->getSensorPitchAngularRate();
        textStream << "\n";
        Program::getInstance().getUdpClient()->sendData(textStream.str());
    } while (!exitHandler);
    Logger::getInstance().logEvent(INFO, "telemetry handler terminated");
}
