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
	if(SerialBus::buses.find(busId) == SerialBus::buses.end())
	{
		Program::getInstance().terminate(ExitCode::WRONG_I2C_BUS);
	}

	pSerialBus = SerialBus::buses.find(busId)->second;
	handle = i2cOpen(busId, address, 0);

	if(handle >= 0)
	{
		Logger::getInstance().logEvent(INFO, "I2C device opened: bus=", busId, ", address=0x", std::hex, address, " priority=0x", priority);
	}
	else
	{
		Program::getInstance().terminate(I2C_NOT_OPENED);
	}

	// register this i2c device in the bus object map of devices
	pSerialBus->registerDevice(SerialDeviceContainer{priority, this});
}


I2cDevice::~I2cDevice()
{
	if(handle >= 0)
	{
		i2cClose(handle);
	}
	pSerialBus->unregisterDevice(this);
}

int I2cDevice::writeData(unsigned handle, unsigned command, std::vector<uint8_t> data)
{

}

int I2cDevice::readData(unsigned handle, unsigned command, uint8_t* dataBuffer, unsigned length)
{

}
