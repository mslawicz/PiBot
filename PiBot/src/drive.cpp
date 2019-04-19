/*
 * Drive.cpp
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#include "LSM9DS1.h"
#include "drive.h"
#include "program.h"

Drive::Drive()
{
    pGyroscope = new Gyroscope(SerialBusId::I2C1, SerialPriority::GYROSCOPE_PR, I2cDeviceAddress::GYROSCOPE_ADDR);
    pAccelerometer = new Accelerometer(SerialBusId::I2C1, SerialPriority::ACCELEROMETER_PR, I2cDeviceAddress::ACCELEROMETER_ADDR);
    sensorAngularRateX = sensorAngularRateY = sensorAngularRateZ = 0.0;
    sensorAccelerationX = sensorAccelerationY = sensorAccelerationZ = 0.0;
    targetPitchAngularRate = 0.0;
    pPitchPID = new PID(0.5, 0.05, 0.05);
    // left motor
    pMotors.push_back(new Motor(SerialBusId::I2C1, SerialPriority::MOTOR_PR, I2cDeviceAddress::MOTOR_ADDR, 0));
    // right motor
    pMotors.push_back(new Motor(SerialBusId::I2C1, SerialPriority::MOTOR_PR, I2cDeviceAddress::MOTOR_ADDR, 2));
    pitchControlSpeed = 0.0;
    yawSpeed = 0.0;
}

Drive::~Drive()
{
    for(auto pMotor : pMotors)
    {
        delete pMotor;
        pMotor = nullptr;
    }
    delete pAccelerometer;
	delete pGyroscope;
	delete pPitchPID;
}

/*
 * starts the control of motors
 */
void Drive::start(void)
{
	// enable gyroscope interrupts
    // interrupt function is called either on interrupt signal or after stated timeout in ms
	gpioSetISRFuncEx(GpioPin::GYRO_INT, RISING_EDGE, 10, Drive::gyroInterruptCallback, this);
}

/*
 * stops the control of motors
 */
void Drive::stop(void)
{
	// disable gyroscope interrupts
	gpioSetISRFuncEx(GpioPin::GYRO_INT, RISING_EDGE, 0, nullptr, this);
    pMotors[0]->setSpeed(0, true);
    pMotors[1]->setSpeed(0, true);
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
void Drive::gyroInterruptCallback(int gpio, int level, uint32_t tick,
		void* pDriveObject)
{
	static_cast<Drive*>(pDriveObject)->pitchControl(level, tick);
}

/*
 * pitch control method
 */
void Drive::pitchControl(int level, uint32_t tick)
{
    const unsigned dataLength = 6;

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
        if((std::get<0>(data) == ImuRegisters::OUT_X_L_G) && (std::get<1>(data) == dataLength))
        {
            //valid data received
            sensorAngularRateX = *reinterpret_cast<int16_t*>(&std::get<2>(data)[0]) * pGyroscope->range / 0x7FFF;
            sensorAngularRateY = *reinterpret_cast<int16_t*>(&std::get<2>(data)[2]) * pGyroscope->range / 0x7FFF;
            sensorAngularRateZ = *reinterpret_cast<int16_t*>(&std::get<2>(data)[4]) * pGyroscope->range / 0x7FFF;

            //pitchControlSpeed = -1.0 * pPitchPID->calculate(targetPitchAngularRate, sensorPitchAngularRate);
            // set the speed of both motors
            // TODO: limit the speed to allowed range
            motorSpeed[0] = pitchControlSpeed - yawSpeed;
            motorSpeed[1] = pitchControlSpeed + yawSpeed;
            // send speed value to the motors
            pMotors[0]->setSpeed(motorSpeed[0]);
            pMotors[1]->setSpeed(motorSpeed[1]);
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
                Program::getInstance().getRobot()->telemetryParameters["pitchControlSpeed"] = pitchControlSpeed;
            }
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
void Drive::setYawSpeed(float speed)
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
void Drive::setTargetPitchAngularRate(float rate)
{
    if(rate > 1.0f)
    {
        rate = 1.0f;
    }
    else if (rate < -1.0f)
    {
        rate = -1.0f;
    }
    targetPitchAngularRate = rate;
}
