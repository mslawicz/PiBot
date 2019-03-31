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
#include <thread>
#include <condition_variable>
#include <mutex>
#include <unordered_map>

class Robot
{
public:
    Robot();
    ~Robot();
    // starts the control of robot
    void start(void);
    // stops the control of robot
    void stop(void);
    // this handler is to be run in a new thread
    void telemetryHandler(void);
    void setTelemetry(bool state) { telemetryEnabled = state; }
    void telemetryNotify(void) { telemetryTriggered = true; telemetryEvent.notify_one(); }
    bool isTelemetryEnabled(void) const { return telemetryEnabled; }
    Drive* getDrive(void) const {return pDrive;}
    std::unordered_map<std::string, float> telemetryParameters;
    std::mutex telemetryHandlerMutex;
private:
    Drive* pDrive;
    bool telemetryEnabled;
    std::thread* pTelemetryHandlerThread;
    bool exitHandler;
    std::condition_variable telemetryEvent;
    bool telemetryTriggered;
};

#endif /* SRC_ROBOT_H_ */
