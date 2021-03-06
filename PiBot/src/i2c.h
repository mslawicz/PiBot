/*
 * i2c.h
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include "serial.h"
#include <vector>


enum I2cDeviceAddress
{
    GYROSCOPE_ADDR = 0x6B,
    ACCELEROMETER_ADDR = GYROSCOPE_ADDR,
    MAGNETOMETER_ADDR = 0x1E,
    PCA9685_ADDR = 0x6F,
    MOTOR_ADDR = PCA9685_ADDR,
    SERVO_ADDR = PCA9685_ADDR,
    PCA9685_ALL_ADDR = 0x70
};



class I2cDevice : public SerialDevice
{
public:
	I2cDevice(SerialBusId i2cBusId, SerialPriority devicePriority, I2cDeviceAddress deviceAddres);
	~I2cDevice();
private:
    int writeData(unsigned handle, unsigned registerAddress, std::vector<uint8_t> data) override;
    int readData(unsigned handle, unsigned registerAddress, uint8_t* dataBuffer, unsigned length) override;
	I2cDeviceAddress address;
};

#endif /* SRC_I2C_H_ */
