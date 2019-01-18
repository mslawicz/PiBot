/*
 * imu.cpp
 *
 *  Created on: 17 sty 2019
 *      Author: Marcin
 */

#include "imu.h"
#include "LSM9DS1.h"
#include "program.h"
#include "logger.h"

IMU::IMU()
{
	// port of gyroscope interrupt signal
	pGyroInterruptPort = new GPIO(13, PI_INPUT, PI_PUD_UP);
	// interface to IMU device
	pInterface = new I2C(I2C1, 0x6B);
	// verify the IMU device
	auto data = pInterface->read(IMU_Registers::WHO_AM_I, 1);
	if (data[0] != LSM9DS1_AG)
	{
		// this is not expected LSM9DS1 A/G device
		Program::getInstance().terminate(WRONG_I2C_DEVICE);
	}
}

IMU::~IMU()
{
	delete pInterface;
	delete pGyroInterruptPort;
}

/*
 * configuration of IMU device
 */
void IMU::config(void)
{

}
