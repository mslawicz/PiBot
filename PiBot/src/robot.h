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
#include "imu.h"
#include "pid.h"
#include "filter.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <unordered_map>

class Robot
{
public:
    Robot();
    ~Robot();
    // starts the robot functions
    void start(void);
    // stops the robot functions
    void stop(void);
    // this handler is to be run in a new thread
    void telemetryHandler(void);
    void setTelemetry(bool state) { telemetryEnabled = state; }
    void telemetryNotify(void) { telemetryTriggered = true; telemetryEvent.notify_one(); }
    bool isTelemetryEnabled(void) const { return telemetryEnabled; }
    Drive* getDrive(void) const {return pDrive;}
    void setYawSpeed(float speed);
    void setAlpha(float value);
    float getAlpha(void) const { return alpha; }
    void setTargetPitch(float pitch);
    PID* getPitchPID(void) const {return pPitchPID;}
    PID* getSpeedPID(void) const {return pSpeedPID;}
    void setBeta(float value) { pMotorSpeedFilter->setStrength(value); }
    std::unordered_map<std::string, float> telemetryParameters;
    std::mutex telemetryHandlerMutex;
private:
    // callback function for gyroscope generated interrupts
    static void gyroInterruptCallback(int gpio, int level, uint32_t tick, void* pRobotObject);
    // pitch control method
    void pitchControl(int level, uint32_t tick);
    Drive* pDrive;
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
    float yawSpeed; // motor differential speed - causes  robot yaw
    uint32_t lastTick;
    float alpha;
    float pitchControlSpeed;    // calculated motor speed for pitch control (for both motors)
    float targetSpeed;          // target speed of the robot
    // pointer to pitch control loop PID
    PID* pPitchPID;
    // pointer to speed control loop PID
    PID* pSpeedPID;
    EMA* pMotorSpeedFilter;
};

#endif /* SRC_ROBOT_H_ */
