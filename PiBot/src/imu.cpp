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
	pGyroInterruptPort = new GPIO(13, PI_INPUT, PI_PUD_DOWN);
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
	// Gyroscope data ready on INT 1_A/G pin
	std::vector<char> data = {0x02};
	pInterface->write(IMU_Registers::INT1_CTRL, data);
	// gyroscope output data rate 238 Hz, 245 dps, LPF=29 Hz
	// LPF2 output for interrupts and DataReg
	// low-power disabled, HPF enabled, HPF 1 Hz
	data = {0x81, 0x0F, 0x44};
	pInterface->write(IMU_Registers::CTRL_REG1_G, data);
	// accelerometer output data rate 238 Hz
	data = {0x80, 0, 0};
	pInterface->write(IMU_Registers::CTRL_REG6_XL, data);
}
