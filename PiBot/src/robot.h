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
#include "servo.h"
#include "imu.h"
#include "gpio.h"
#include "pid.h"
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
    void setYawSpeed(float speed);
    void setAlpha(float value);
    void setTargetPitch(float pitch);
    std::unordered_map<std::string, float> telemetryParameters;
    std::mutex telemetryHandlerMutex;
    PID* getPitchPID(void) const {return pPitchPID;}
private:
    Drive* pDrive;
    // callback function for gyroscope generated interrupts
    static void gyroInterruptCallback(int gpio, int level, uint32_t tick, void* pRobotObject);
    // pitch control method
    void pitchControl(int level, uint32_t tick);
    // pointer to gyroscope device
    Gyroscope* pGyroscope;
    // pointer to accelerometer device
    Accelerometer* pAccelerometer;
    // sensor angular rate values [rad/s]
    float sensorAngularRateX;
    float sensorAngularRateY;
    float sensorAngularRateZ;
    // sensor acceleration values [g]
    float sensorAccelerationX;
    float sensorAccelerationY;
    float sensorAccelerationZ;
    // robot tilts [rad]
    float pitch;
    float roll;
    float yaw;
    float targetPitch;
    bool telemetryEnabled;
    std::thread* pTelemetryHandlerThread;
    bool exitHandler;
    std::condition_variable telemetryEvent;
    bool telemetryTriggered;
    ServoGPIO* pKickstand;
    const float KickstandUp = 0.0f;
    const float KickstandDown = 0.4f;
    float yawSpeed; // motor differential speed - causes  robot yaw
    uint32_t lastTick;
    float alpha;
    float pitchControlSpeed;    // calculated motor speed for pitch control (for both motors)
    // pointer to pitch control loop PID
    PID* pPitchPID;
};

#endif /* SRC_ROBOT_H_ */
