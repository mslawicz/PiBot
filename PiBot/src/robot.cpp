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
    Logger::getInstance().logEvent(INFO, "start robot control");
    pGyroscope = new Gyroscope(SerialBusId::I2C1, SerialPriority::GYROSCOPE_PR, I2cDeviceAddress::GYROSCOPE_ADDR);
    pAccelerometer = new Accelerometer(SerialBusId::I2C1, SerialPriority::ACCELEROMETER_PR, I2cDeviceAddress::ACCELEROMETER_ADDR);
    sensorAngularRateX = sensorAngularRateY = sensorAngularRateZ = 0.0;
    sensorAccelerationX = sensorAccelerationY = sensorAccelerationZ = 0.0;
    pDrive = new Drive;
    pPitchPID = new PID(0.5, 0.05, 0.05);
    pSpeedPID = new PID(0.1, 0.1, 0);
    telemetryEnabled = false;
    telemetryTriggered = false;
    yawSpeed = 0.0;
    lastTick = 0;
    pitch = roll = yaw = 0.0;
    alpha = 0.005;
    targetPitch = 0.0;
    pitchControlSpeed = targetSpeed = 0.0;
    exitHandler = false;
    pMotorSpeedFilter = new EMA(0.001);
    // enable gyroscope interrupts
    // interrupt function is called either on interrupt signal or after stated timeout in ms
    gpioSetISRFuncEx(GpioPin::GYRO_INT, RISING_EDGE, 12, Robot::gyroInterruptCallback, this);
    pTelemetryHandlerThread = new std::thread(&Robot::telemetryHandler, this);
    speed = 0.0f;
}

Robot::~Robot()
{
    Logger::getInstance().logEvent(INFO, "stop robot control");
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
    delete pDrive;
    delete pAccelerometer;
    delete pGyroscope;
    delete pPitchPID;
    delete pSpeedPID;
    delete pMotorSpeedFilter;
}

/*
 * starts the control of robot
 */
void Robot::start(void)
{
    targetSpeed = 0.0f;
    //targetPitch = 0.0f;
    speed = 0.0f;
    pPitchPID->reset();
    pSpeedPID->reset();
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
        // sensor acceleration - X axis [g]
        textStream << telemetryParameters["sensorAccelerationX"] << ",";
        // robot pitch [rad]
        textStream << telemetryParameters["pitch"] << ",";
        // Pitch PID Kp
        textStream << telemetryParameters["PitchKp"] << ",";
        // Pitch PID Ki
        textStream << telemetryParameters["PitchKi"] << ",";
        // Pitch PID Kd
        textStream << telemetryParameters["PitchKd"] << ",";
        // Pitch PID P value
        textStream << telemetryParameters["PitchP"] << ",";
        // Pitch PID I value
        textStream << telemetryParameters["PitchI"] << ",";
        // Pitch PID D value
        textStream << telemetryParameters["PitchD"] << ",";
        // both motors pitch control speed [1.0 == full forward speed]
        textStream << telemetryParameters["pitchControlSpeed"] << ",";
        // complementary filter coefficient alpha
        textStream << telemetryParameters["alpha"] << ",";
        textStream << telemetryParameters["dt"] << ",";
        // Speed PID Kp
        textStream << telemetryParameters["SpeedKp"] << ",";
        // Speed PID Ki
        textStream << telemetryParameters["SpeedKi"] << ",";
        // Speed PID Kd
        textStream << telemetryParameters["SpeedKd"] << ",";
        // Speed PID P value
        textStream << telemetryParameters["SpeedP"] << ",";
        // Speed PID I value
        textStream << telemetryParameters["SpeedI"] << ",";
        // Speed PID D value
        textStream << telemetryParameters["SpeedD"] << ",";
        // calculated target pitch
        textStream << telemetryParameters["targetPitch"] << ",";
        textStream << telemetryParameters["speed"] << ",";
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

            const float beta = 0.01;
            // complementary filter for robot speed
            speed = (1.0 - beta) * (speed + 0.102 * sensorAccelerationX * dt) + beta * 0.89535f * pitchControlSpeed;
            //targetPitch = -pSpeedPID->calculate(targetSpeed, speed, dt);

            pitchControlSpeed = -pPitchPID->calculate(targetPitch + targetSpeed, pitch, -sensorAngularRateX, dt);
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
                Program::getInstance().getRobot()->telemetryParameters["sensorAccelerationX"] = sensorAccelerationX;
                Program::getInstance().getRobot()->telemetryParameters["PitchKp"] =  pPitchPID->getKp();
                Program::getInstance().getRobot()->telemetryParameters["PitchKi"] =  pPitchPID->getKi();
                Program::getInstance().getRobot()->telemetryParameters["PitchKd"] =  pPitchPID->getKd();
                Program::getInstance().getRobot()->telemetryParameters["PitchP"] =  pPitchPID->getProportional();
                Program::getInstance().getRobot()->telemetryParameters["PitchI"] =  pPitchPID->getIntegral();
                Program::getInstance().getRobot()->telemetryParameters["PitchD"] =  pPitchPID->getDerivative();
                Program::getInstance().getRobot()->telemetryParameters["pitch"] = pitch;
                Program::getInstance().getRobot()->telemetryParameters["pitchControlSpeed"] = pitchControlSpeed;
                Program::getInstance().getRobot()->telemetryParameters["alpha"] = alpha;
                Program::getInstance().getRobot()->telemetryParameters["dt"] = dt * 1e3;
                Program::getInstance().getRobot()->telemetryParameters["SpeedKp"] =  pSpeedPID->getKp();
                Program::getInstance().getRobot()->telemetryParameters["SpeedKi"] =  pSpeedPID->getKi();
                Program::getInstance().getRobot()->telemetryParameters["SpeedKd"] =  pSpeedPID->getKd();
                Program::getInstance().getRobot()->telemetryParameters["SpeedP"] =  pSpeedPID->getProportional();
                Program::getInstance().getRobot()->telemetryParameters["SpeedI"] =  pSpeedPID->getIntegral();
                Program::getInstance().getRobot()->telemetryParameters["SpeedD"] =  pSpeedPID->getDerivative();
                Program::getInstance().getRobot()->telemetryParameters["targetPitch"] = targetPitch;
                Program::getInstance().getRobot()->telemetryParameters["speed"] = speed;
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
 * set robot speed indirectly by modifying target pitch
 */
void Robot::setSpeed(float speed)
{
    if(speed > 0.1f)
    {
        speed = 0.1f;
    }
    else if (speed < -0.1f)
    {
        speed = -0.1f;
    }
    targetSpeed = speed;
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
