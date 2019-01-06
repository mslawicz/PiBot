/*
 * i2c.cpp
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

#include <stdlib.h>
#include "exit_codes.h"
#include "i2c.h"


I2C::I2C(unsigned BusId, unsigned DeviceAddress)
	: m_BusId(BusId)
	, m_DeviceAddress(DeviceAddress)
{
	m_pDataBuf = new char[m_DataBufSize];
	if(m_pDataBuf == NULL)
	{
		exit(ExitCode::MEMORY_ALLOCATION_ERROR);
	}
	m_handle = i2cOpen(BusId, DeviceAddress, 0);
	// TODO: how to react to unsuccessfull opening?
}

I2C::~I2C()
{
	if(m_handle >= 0)
	{
		i2cClose(m_handle);
	}
	if(m_pDataBuf != NULL)
	{
		delete[] m_pDataBuf;
	}
}

std::vector<char> I2C::Read(unsigned address, unsigned length)
{

	int NoOfBytes = i2cReadI2CBlockData(m_handle, address, m_pDataBuf, length);
	std::vector<char> Data(m_pDataBuf, m_pDataBuf+NoOfBytes);
	return Data;
}

