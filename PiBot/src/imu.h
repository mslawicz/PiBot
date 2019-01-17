/*
 * imu.h
 *
 *  Created on: 17 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_IMU_H_
#define SRC_IMU_H_

#include "gpio.h"
#include "i2c.h"


class IMU
{
public:
	IMU();
	~IMU();
	unsigned int getGyroInterruptPort(void) const { return pGyroInterruptPort->getNumber(); }
private:
	// pointer to port for gyroscope interrupts
	GPIO* pGyroInterruptPort;
	// pointer to interface to IMU device
	I2C* pInterface;
};

#endif /* SRC_IMU_H_ */
