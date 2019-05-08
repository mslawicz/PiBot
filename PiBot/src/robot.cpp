/*
 * robot.cpp
 *
 *  Created on: 23 mar 2019
 *      Author: Marcin
 */

#include "LSM9DS1.h"
#include "robot.h"
#include "program.h"
#include "udp.h"
#include "logger.h"
#include <sstream>
#include <math.h>

Robot::Robot()
{
    pGyroscope = new Gyroscope(SerialBusId::I2C1, SerialPriority::GYROSCOPE_PR, I2cDeviceAddress::GYROSCOPE_ADDR);
    pAccelerometer = new Accelerometer(SerialBusId::I2C1, SerialPriority::ACCELEROMETER_PR, I2cDeviceAddress::ACCELEROMETER_ADDR);
    gyroscopeX = gyroscopeY = gyroscopeZ = 0.0;
    accelerometerX = accelerometerY = accelerometerZ = 0.0;
    pDrive = new Drive;
    pPitchPID = new PID(0.5, 0.05, 0.05);
    telemetryEnabled = false;
    exitHandler = true;
    pTelemetryHandlerThread = nullptr;
    telemetryTriggered = false;
    yawSpeed = 0.0;
    lastTick = 0;
    pitch = roll = yaw = 0.0;
    alpha = 0.005;
    targetPitch = 0.0;
    pitchControlSpeed = 0.0;
    pAHRS = new AHRS;
}

Robot::~Robot()
{
    delete pDrive;
    delete pAccelerometer;
    delete pGyroscope;
    delete pPitchPID;
    delete pAHRS;
}

/*
 * starts the control of robot
 */
void Robot::start(void)
{
    Logger::getInstance().logEvent(INFO, "robot start request");
    exitHandler = false;
    // enable gyroscope interrupts
    // interrupt function is called either on interrupt signal or after stated timeout in ms
    gpioSetISRFuncEx(GpioPin::GYRO_INT, RISING_EDGE, 12, Robot::gyroInterruptCallback, this);
    pTelemetryHandlerThread = new std::thread(&Robot::telemetryHandler, this);
}

/*
 * stops the control of robot
 */
void Robot::stop(void)
{
    Logger::getInstance().logEvent(INFO, "robot stop request");
    // disable gyroscope interrupts
    gpioSetISRFuncEx(GpioPin::GYRO_INT, RISING_EDGE, 0, nullptr, this);
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
        textStream << telemetryParameters["gyroscopeX"] << ",";
        // gyroscope angular rate - Y axis [rad/s]
        textStream << telemetryParameters["gyroscopeY"] << ",";
        // gyroscope angular rate - Z axis [rad/s]
        textStream << telemetryParameters["gyroscopeZ"] << ",";
        // sensor acceleration - X axis [g]
        textStream << telemetryParameters["accelerometerX"] << ",";
        // sensor acceleration - Y axis [g]
        textStream << telemetryParameters["accelerometerY"] << ",";
        // sensor acceleration - Z axis [g]
        textStream << telemetryParameters["accelerometerZ"] << ",";
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
        textStream << telemetryParameters["dt"] << ",";
        // quaternion values
        textStream << telemetryParameters["q0"] << ",";
        textStream << telemetryParameters["q1"] << ",";
        textStream << telemetryParameters["q2"] << ",";
        textStream << telemetryParameters["q3"] << ",";
        textStream << "\n";
        Program::getInstance().getUdpClient()->sendData(textStream.str());
        lock.unlock();
    } while (!exitHandler);
    Logger::getInstance().logEvent(INFO, "telemetry handler terminated");
}

/*
 * callback function for gyroscope generated interrupts
 * it must be a static method, but the pointer to drive object is passed as an argument
 * Parameter   Value    Meaning
 *
 * gpio        0-53     The GPIO which has changed state
 *
 * level       0-2      0 = change to low (a falling edge)
 *                      1 = change to high (a rising edge)
 *                      2 = no level change (interrupt timeout)
 *
 * tick        32 bit   The number of microseconds since boot
 *                      WARNING: this wraps around from
 *                      4294967295 to 0 roughly every 72 minutes
 *
 * pDriveObject pointer Pointer to an arbitrary object
 *
 */
void Robot::gyroInterruptCallback(int gpio, int level, uint32_t tick,
        void* pRobotObject)
{
    static_cast<Robot*>(pRobotObject)->pitchControl(level, tick);
}

/*
 * pitch control method
 */
void Robot::pitchControl(int level, uint32_t tick)
{
    const unsigned dataLength = 6;
    const float TickPeriod = 1e-6;  // 1 Tick = 1 microsecond

    if(!pAccelerometer->receiveQueueEmpty())
    {
        // there's data in the accelerometer reception queue
        auto data = pAccelerometer->getLastData();
        if((std::get<0>(data) == ImuRegisters::OUT_X_L_XL) && (std::get<1>(data) == dataLength))
        {
            //valid data received
            accelerometerX = *reinterpret_cast<int16_t*>(&std::get<2>(data)[0]) * pAccelerometer->range / 0x7FFF;
            accelerometerY = *reinterpret_cast<int16_t*>(&std::get<2>(data)[2]) * pAccelerometer->range / 0x7FFF;
            accelerometerZ = *reinterpret_cast<int16_t*>(&std::get<2>(data)[4]) * pAccelerometer->range / 0x7FFF;
        }
    }

    if(!pGyroscope->receiveQueueEmpty())
    {
        // there's data in the gyroscope reception queue
        auto data = pGyroscope->getLastData();
        if((std::get<0>(data) == ImuRegisters::OUT_X_L_G) &&
                (std::get<1>(data) == dataLength) &&
                (tick != lastTick))
        {
            //valid gyroscope data received
            gyroscopeX = *reinterpret_cast<int16_t*>(&std::get<2>(data)[0]) * pGyroscope->range / 0x7FFF;
            gyroscopeY = *reinterpret_cast<int16_t*>(&std::get<2>(data)[2]) * pGyroscope->range / 0x7FFF;
            gyroscopeZ = *reinterpret_cast<int16_t*>(&std::get<2>(data)[4]) * pGyroscope->range / 0x7FFF;

            //calculate time elapsed from the last calculations
            float dt = (tick - lastTick) * TickPeriod;

            pAHRS->process(gyroscopeX, gyroscopeY, gyroscopeZ, accelerometerX, accelerometerY, accelerometerZ, 0, 0, 0, dt);

            // calculate pitch of the robot
            pitch = asinf(2.0f * (pAHRS->getQ(0) * pAHRS->getQ(2) - pAHRS->getQ(1) * pAHRS->getQ(3)));


            pitchControlSpeed = pPitchPID->calculate(targetPitch, pitch, gyroscopeX, dt);
            // set the speed of both motors
            // TODO: limit the speed to allowed range
            if(pDrive->isActive())
            {
                pDrive->setMotorSpeed(MotorName::LeftMotor, pitchControlSpeed + yawSpeed);
                pDrive->setMotorSpeed(MotorName::RightMotor, pitchControlSpeed - yawSpeed);
            }
            {
                std::lock_guard<std::mutex> lock(Program::getInstance().getRobot()->telemetryHandlerMutex);
                Program::getInstance().getRobot()->telemetryParameters["gyroscopeX"] = gyroscopeX;
                Program::getInstance().getRobot()->telemetryParameters["gyroscopeY"] = gyroscopeY;
                Program::getInstance().getRobot()->telemetryParameters["gyroscopeZ"] = gyroscopeZ;
                Program::getInstance().getRobot()->telemetryParameters["accelerometerX"] = accelerometerX;
                Program::getInstance().getRobot()->telemetryParameters["accelerometerY"] = accelerometerY;
                Program::getInstance().getRobot()->telemetryParameters["accelerometerZ"] = accelerometerZ;
                Program::getInstance().getRobot()->telemetryParameters["PidKp"] =  pPitchPID->getKp();
                Program::getInstance().getRobot()->telemetryParameters["PidKi"] =  pPitchPID->getKi();
                Program::getInstance().getRobot()->telemetryParameters["PidKd"] =  pPitchPID->getKd();
                Program::getInstance().getRobot()->telemetryParameters["PidProportional"] =  pPitchPID->getProportional();
                Program::getInstance().getRobot()->telemetryParameters["PidIntegral"] =  pPitchPID->getIntegral();
                Program::getInstance().getRobot()->telemetryParameters["PidDerivative"] =  pPitchPID->getDerivative();
                Program::getInstance().getRobot()->telemetryParameters["pitch"] = pitch;
                Program::getInstance().getRobot()->telemetryParameters["roll"] = roll;
                Program::getInstance().getRobot()->telemetryParameters["yaw"] = yaw;
                Program::getInstance().getRobot()->telemetryParameters["pitchControlSpeed"] = pitchControlSpeed;
                Program::getInstance().getRobot()->telemetryParameters["dt"] = dt * 1e3;
                Program::getInstance().getRobot()->telemetryParameters["q0"] = pAHRS->getQ(0);
                Program::getInstance().getRobot()->telemetryParameters["q1"] = pAHRS->getQ(1);
                Program::getInstance().getRobot()->telemetryParameters["q2"] = pAHRS->getQ(2);
                Program::getInstance().getRobot()->telemetryParameters["q3"] = pAHRS->getQ(3);
            }

            lastTick = tick;
        }

        if(Program::getInstance().getRobot()->isTelemetryEnabled())
        {
            // notify telemetry handler thread
            Program::getInstance().getRobot()->telemetryNotify();
        }
    }


    // send read data request; this data is to be used in the next function call
    pGyroscope->readDataRequest(ImuRegisters::OUT_X_L_G, dataLength);
    pAccelerometer->readDataRequest(ImuRegisters::OUT_X_L_XL, dataLength);
}

/*
 * set robot yaw speed
 */
void Robot::setYawSpeed(float speed)
{
    if(speed > 1.0f)
    {
        speed = 1.0f;
    }
    else if (speed < -1.0f)
    {
        speed = -1.0f;
    }
    yawSpeed = speed;
}

/*
 * set target pitch angular rate
 */
void Robot::setTargetPitch(float pitch)
{
    if(pitch > 1.0f)
    {
        pitch = 1.0f;
    }
    else if (pitch < -1.0f)
    {
        pitch = -1.0f;
    }
    targetPitch = pitch;
}

/*
 * set complementary filter coefficient alpha
 */
void Robot::setAlpha(float value)
{
    if(value > 0.2f)
    {
        value = 0.2f;
    }
    else if (value < 0)
    {
        value = 0.0f;
    }
    alpha = value;
}
