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
#include <mutex>
#include <queue>

enum I2cDeviceAddress
{
    GYROSCOPE_ADDR = 0x6B,
    ACCELEROMETER_ADDR = GYROSCOPE_ADDR,
    MAGNETOMETER_ADDR = 0x1E,
    MOTOR_ADDR = 0x6F,
    PCA9685_ADDR = MOTOR_ADDR,
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
    std::queue<SerialDataContainer> dataToSend;
    std::queue<SerialDataContainer> receivedData;
    std::mutex sendQueueMutex;
    std::mutex receiveQueueMutex;
};

#endif /* SRC_I2C_H_ */
