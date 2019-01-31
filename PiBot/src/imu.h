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
	// configuration of IMU device
	void config(void);
	// pointer to port for gyroscope interrupts
	GPIO* pGyroInterruptPort;
	// pointer to interface to IMU device
	//I2C* pInterface;
};

class Gyroscope : public I2cDevice
{
public:
    Gyroscope(I2cBusId busId, uint8_t address, I2cPriority priority);
    int getData(void) const     // example of getting data from a i2c device
    {
        return 111;
    }
};

#endif /* SRC_IMU_H_ */
