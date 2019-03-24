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
    sensorPitchAngularRate = 0.0;
    targetPitchAngularRate = 0.0;
    pPitchPID = new PID(0.5, 0.05, 0.05);
    // left motor
    pMotors.push_back(new Motor(SerialBusId::I2C1, SerialPriority::MOTOR_PR, I2cDeviceAddress::MOTOR_ADDR, 0));
    // right motor
    pMotors.push_back(new Motor(SerialBusId::I2C1, SerialPriority::MOTOR_PR, I2cDeviceAddress::MOTOR_ADDR, 2));
    pitchControlSpeed = 0.0;
}

Drive::~Drive()
{
    for(auto pMotor : pMotors)
    {
        delete pMotor;
        pMotor = nullptr;
    }
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
    if(!pGyroscope->receiveQueueEmpty())
    {
        // there's data in the reception queue
        auto data = pGyroscope->getLastData();
        if((std::get<0>(data) == ImuRegisters::OUT_X_L_G) && (std::get<1>(data) == dataLength))
        {
            //valid data received
            sensorPitchAngularRate = *reinterpret_cast<int16_t*>(&std::get<2>(data)[0]) * pGyroscope->range / 0xFFFF;
            pitchControlSpeed = pPitchPID->calculate(targetPitchAngularRate, sensorPitchAngularRate);
            motorSpeed[0] = pitchControlSpeed;
            motorSpeed[1] = pitchControlSpeed;
            pMotors[0]->setSpeed(motorSpeed[0]);
            pMotors[1]->setSpeed(motorSpeed[1]);
        }
    }


    // send read data request; this data is to be used in the next function call
    pGyroscope->readDataRequest(ImuRegisters::OUT_X_L_G, dataLength);

    // notify telemetry handler thread
    Program::getInstance().getRobot()->telemetryNotify();
}


