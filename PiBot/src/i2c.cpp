/*
 * i2c.cpp
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

#include "i2c.h"
#include "logger.h"
#include "program.h"
#include "gpio.h"


/*
 * constructor of a new i2c device
 */
I2cDevice::I2cDevice(SerialBusId i2cBusId, SerialPriority devicePriority, I2cDeviceAddress deviceAddres)
	: SerialDevice(i2cBusId, devicePriority, deviceAddres)
	, address(deviceAddres)
{
	handle = i2cOpen(busId, address, 0);

	if(handle >= 0)
	{
		Logger::getInstance().logEvent(INFO, "I2C device opened: bus=", busId, ", address=0x", std::hex, address, " priority=0x", priority);
	}
	else
	{
		Program::getInstance().terminate(I2C_NOT_OPENED);
	}
}


I2cDevice::~I2cDevice()
{

}

int I2cDevice::writeData(unsigned handle, unsigned registerAddress, std::vector<uint8_t> data)
{
    return i2cWriteI2CBlockData(handle, registerAddress, (char*)&data[0], data.size());
}

int I2cDevice::readData(unsigned handle, unsigned registerAddress, uint8_t* dataBuffer, unsigned length)
{
    return i2cReadI2CBlockData(handle, registerAddress, (char*)dataBuffer, length);
}
