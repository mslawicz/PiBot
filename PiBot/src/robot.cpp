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
    sensorAngularRateX = sensorAngularRateY = sensorAngularRateZ = 0.0;
    sensorAccelerationX = sensorAccelerationY = sensorAccelerationZ = 0.0;
    pDrive = new Drive;
    pPitchPID = new PID(0.5, 0.05, 0.05);
    telemetryEnabled = false;
    exitHandler = true;
    pTelemetryHandlerThread = nullptr;
    telemetryTriggered = false;
    yawSpeed = 0.0;
    lastTick = 0;
    pitch = roll = yaw = 0.0;
    alpha = 0.02;
    targetPitch = 0.0;
    pitchControlSpeed = 0.0;
}

Robot::~Robot()
{
    delete pDrive;
    delete pAccelerometer;
    delete pGyroscope;
    delete pPitchPID;
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
    gpioSetISRFuncEx(GpioPin::GYRO_INT, RISING_EDGE, 10, Robot::gyroInterruptCallback, this);
    pTelemetryHandlerThread = new std::thread(&Robot::telemetryHandler, this);

    //TODO place it in proper place: pPitchPID->reset();
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
        textStream << telemetryParameters["dt"] << ",";
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
            sensorAccelerationX = *reinterpret_cast<int16_t*>(&std::get<2>(data)[0]) * pAccelerometer->range / 0x7FFF;
            sensorAccelerationY = *reinterpret_cast<int16_t*>(&std::get<2>(data)[2]) * pAccelerometer->range / 0x7FFF;
            sensorAccelerationZ = *reinterpret_cast<int16_t*>(&std::get<2>(data)[4]) * pAccelerometer->range / 0x7FFF;
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
            sensorAngularRateX = *reinterpret_cast<int16_t*>(&std::get<2>(data)[0]) * pGyroscope->range / 0x7FFF;
            sensorAngularRateY = *reinterpret_cast<int16_t*>(&std::get<2>(data)[2]) * pGyroscope->range / 0x7FFF;
            sensorAngularRateZ = *reinterpret_cast<int16_t*>(&std::get<2>(data)[4]) * pGyroscope->range / 0x7FFF;

            //calculate time elapsed from the last calculations
            float dt = (tick - lastTick) * TickPeriod;

            // calculate tilt of the robot
            pitch = (1.0 - alpha) * (pitch + sensorAngularRateX * dt) + alpha * static_cast<float>(atan2(sensorAccelerationX, sensorAccelerationZ));
            roll = (1.0 - alpha) * (roll + sensorAngularRateY * dt) + alpha * static_cast<float>(atan2(sensorAccelerationY, sensorAccelerationZ));
            yaw = 0.999 * (yaw + sensorAngularRateZ * dt);


            pitchControlSpeed = pPitchPID->calculate(targetPitch, pitch, sensorAngularRateX, dt);
            // set the speed of both motors
            // TODO: limit the speed to allowed range
            if(pDrive->isActive())
            {
                pDrive->setMotorSpeed(MotorName::LeftMotor, pitchControlSpeed + yawSpeed);
                pDrive->setMotorSpeed(MotorName::RightMotor, pitchControlSpeed - yawSpeed);
            }
            {
                std::lock_guard<std::mutex> lock(Program::getInstance().getRobot()->telemetryHandlerMutex);
                Program::getInstance().getRobot()->telemetryParameters["sensorAngularRateX"] = sensorAngularRateX;
                Program::getInstance().getRobot()->telemetryParameters["sensorAngularRateY"] = sensorAngularRateY;
                Program::getInstance().getRobot()->telemetryParameters["sensorAngularRateZ"] = sensorAngularRateZ;
                Program::getInstance().getRobot()->telemetryParameters["sensorAccelerationX"] = sensorAccelerationX;
                Program::getInstance().getRobot()->telemetryParameters["sensorAccelerationY"] = sensorAccelerationY;
                Program::getInstance().getRobot()->telemetryParameters["sensorAccelerationZ"] = sensorAccelerationZ;
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
                Program::getInstance().getRobot()->telemetryParameters["alpha"] = alpha;
                Program::getInstance().getRobot()->telemetryParameters["dt"] = dt * 1e3;
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
