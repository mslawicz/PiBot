/*
 * i2c.cpp
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

#include "i2c.h"
#include "logger.h"
#include "program.h"
#include <stdlib.h>


I2C::I2C(unsigned busId, unsigned deviceAddress)
	: bus_id(busId)
	, device_address(deviceAddress)
{
	p_data = new char[DataBufSize];
	if(p_data == nullptr)
	{
		Program::getInstance().terminate(MEMORY_ALLOCATION_ERROR);
	}
	handle = i2cOpen(busId, deviceAddress, 0);
	if(handle >= 0)
	{
		Logger::getInstance().logEvent(INFO, "I2C device opened: bus=", busId, ", address=0x", std::hex, deviceAddress);
	}
	else
	{
		// TODO: how to react to unsuccessful opening?
	}
}

I2C::~I2C()
{
	if(handle >= 0)
	{
		i2cClose(handle);
	}
	if(p_data != nullptr)
	{
		delete[] p_data;
	}
}

std::vector<char> I2C::read(unsigned address, unsigned length)
{
	if(length > DataBufSize)
	{
		length = DataBufSize;
		//TODO this improper scenario should be handled here
	}
	int no_of_bytes = i2cReadI2CBlockData(handle, address, p_data, length);
	std::vector<char> data(p_data, p_data+no_of_bytes);
	return data;
}

void I2C::write(unsigned address, std::vector<char> data)
{
	int result = i2cWriteI2CBlockData(handle, address, &data[0], data.size());
	if(result)
	{
		//TODO write error handling
	}
}
