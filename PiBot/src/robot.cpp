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
    telemetryTriggered = false;
    pKickstand = new ServoGPIO(GpioPin::KICKSTAND_SERVO);
    pKickstand->setValue(KickstandDown);
}

Robot::~Robot()
{
    delete pDrive;
    delete pKickstand;
}

/*
 * starts the control of robot
 */
void Robot::start(void)
{
    Logger::getInstance().logEvent(INFO, "robot start request");
    exitHandler = false;
    pTelemetryHandlerThread = new std::thread(&Robot::telemetryHandler, this);
    pKickstand->setValue(KickstandUp);
    pDrive->start();
}

/*
 * stops the control of robot
 */
void Robot::stop(void)
{
    Logger::getInstance().logEvent(INFO, "robot stop request");
    pKickstand->setValue(KickstandDown);
    pDrive->stop();
    if(pTelemetryHandlerThread != nullptr)
    {
        exitHandler = true;
        telemetryNotify();
        pTelemetryHandlerThread->join();
        delete pTelemetryHandlerThread;
        pTelemetryHandlerThread = nullptr;
    }
}

/*
 * telemetry handler to be called in a new thread
 */
void Robot::telemetryHandler(void)
{
    std::stringstream textStream;
    Logger::getInstance().logEvent(INFO, "telemetry handler started");
    do
    {
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(telemetryHandlerMutex);
        telemetryEvent.wait(lock, [this]() {return (telemetryTriggered || exitHandler); });
        telemetryTriggered = false;
        // create parameter csv string
        textStream.str(std::string());

        // gyroscope angular rate - X axis [rad/s]
        textStream << telemetryParameters["sensorAngularRateX"] << ",";
        // gyroscope angular rate - Y axis [rad/s]
        textStream << telemetryParameters["sensorAngularRateY"] << ",";
        // gyroscope angular rate - Z axis [rad/s]
        textStream << telemetryParameters["sensorAngularRateZ"] << ",";
        // sensor acceleration - X axis [g]
        textStream << telemetryParameters["sensorAccelerationX"] << ",";
        // sensor acceleration - Y axis [g]
        textStream << telemetryParameters["sensorAccelerationY"] << ",";
        // sensor acceleration - Z axis [g]
        textStream << telemetryParameters["sensorAccelerationZ"] << ",";
        // robot tilt [rad]
        textStream << telemetryParameters["pitch"] << ",";
        textStream << telemetryParameters["roll"] << ",";
        textStream << telemetryParameters["yaw"] << ",";
        // PID Kp
        textStream << telemetryParameters["PidKp"] << ",";
        // PID Ki
        textStream << telemetryParameters["PidKi"] << ",";
        // PID Kd
        textStream << telemetryParameters["PidKd"] << ",";
        // PID P value
        textStream << telemetryParameters["PidProportional"] << ",";
        // PID I value
        textStream << telemetryParameters["PidIntegral"] << ",";
        // PID D value
        textStream << telemetryParameters["PidDerivative"] << ",";
        // both motors pitch control speed [1.0 == full forward speed]
        textStream << telemetryParameters["pitchControlSpeed"] << ",";
        // complementary filter coefficient alpha
        textStream << telemetryParameters["alpha"] << ",";
        textStream << "\n";
        Program::getInstance().getUdpClient()->sendData(textStream.str());
        lock.unlock();
    } while (!exitHandler);
    Logger::getInstance().logEvent(INFO, "telemetry handler terminated");
}
