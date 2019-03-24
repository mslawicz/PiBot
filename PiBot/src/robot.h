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
    void telemetryEnable(bool state) { telemetryEnabled = state; }
    void telemetryNotify(void) { telemetryEvent.notify_one(); }
private:
    Drive* pDrive;
    bool telemetryEnabled;
    std::thread* pTelemetryHandlerThread;
    bool exitHandler;
    std::condition_variable telemetryEvent;
    std::mutex telemetryHandlerMutex;
};

#endif /* SRC_ROBOT_H_ */
