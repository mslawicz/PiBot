/*
 * Drive.cpp
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#include "LSM9DS1.h"
#include "drive.h"

Drive::Drive()
{
    pGyroscope = new Gyroscope(I2cBusId::I2C1, I2cDeviceAddress::GYROSCOPE_ADDR, I2cPriority::GYROSCOPE_PR);
    // set gyroscope interrupt pin as input
    GPIO gyroscopeInterruptPin(GYRO_INT_PIN, PI_INPUT, PI_PUD_DOWN);
}

Drive::~Drive()
{
	delete pGyroscope;

}

/*
 * starts the control of motors
 */
void Drive::start(void)
{
	// enable gyroscope interrupts
	gpioSetISRFuncEx(GYRO_INT_PIN, RISING_EDGE, 0, Drive::gyroInterruptCallback, this);
	// trig the first data readout
	pGyroscope->readDataRequest(ImuRegisters::OUT_X_L_G, 6);
}

/*
 * stops the control of motors
 */
void Drive::stop(void)
{
	// disable gyroscope interrupts
	gpioSetISRFuncEx(GYRO_INT_PIN, RISING_EDGE, 0, nullptr, this);
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
    GPIO interMark(16, PI_OUTPUT);
    interMark.write(1);
    if(!pGyroscope->receiveQueueEmpty())
    {
        // there's something in a reception queue
        pGyroscope->getData();
    }

    // send read data request; this data is to be used in the next function call
    pGyroscope->readDataRequest(ImuRegisters::OUT_X_L_G, 6);
    interMark.write(0);
}


