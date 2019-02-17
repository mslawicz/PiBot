/*
 * i2c.cpp
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

//#include "i2c.h"
//#include "logger.h"
//#include "program.h"
//#include <algorithm>
//#include <stdlib.h>



/*
 * constructor of a new i2c device
 */
I2cDevice::I2cDevice(SerialBusId spiBusId, SerialPriority devicePriority, I2cDeviceAddress deviceAddres)
	: busId(i2cBusId)
	, address(deviceAddres)
	, priority(devicePriority)
{
	if(I2cBus::buses.find(busId) == I2cBus::buses.end())
	{
		Program::getInstance().terminate(WRONG_SERIAL_BUS);
	}

	pI2cBus = I2cBus::buses.find(busId)->second;
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
	pI2cBus->registerDevice(I2cDeviceContainer{priority, this});
}

/*
 * pure virtual destructor
 */
I2cDevice::~I2cDevice()
{
	if(handle >= 0)
	{
		i2cClose(handle);
	}
	pI2cBus->unregisterDevice(this);
}

/*
 * puts i2c data to send into send queue and notifies i2c bus handler
 */
void I2cDevice::writeData(unsigned registerAddress, std::vector<uint8_t> data)
{
    {
        std::lock_guard<std::mutex> lock(sendQueueMutex);
        dataToSend.push(I2cDataContainer{registerAddress, 0, data});
    }
    pI2cBus->requestToSend();
}

/*
 * puts i2c data to initiate reception into send queue and notifies i2c bus handler
 */
void I2cDevice::readDataRequest(unsigned registerAddress, unsigned length)
{
    {
        std::lock_guard<std::mutex> lock(sendQueueMutex);
        dataToSend.push(I2cDataContainer{registerAddress, length, std::vector<uint8_t>()});
    }
    pI2cBus->requestToSend();
}

/*
 * clears device reception queue without reading it
 */
void I2cDevice::clearReceiveQueue(void)
{
    std::lock_guard<std::mutex> lock(receiveQueueMutex);
    receivedData = std::queue<I2cDataContainer>();
}

/*
 * gets data container from the receive queue
 */
I2cDataContainer I2cDevice::getData(void)
{
    I2cDataContainer data;
    if(!receivedData.empty())
    {
        std::lock_guard<std::mutex> lock(receiveQueueMutex);
        data = receivedData.front();
        receivedData.pop();
    }
    return data;
}

/*
 * gets lastdata container from the receive queue
 * clears the queue
 */
I2cDataContainer I2cDevice::getLastData(void)
{
    I2cDataContainer data;
    if(!receivedData.empty())
    {
        std::lock_guard<std::mutex> lock(receiveQueueMutex);
        data = receivedData.back();
        receivedData = std::queue<I2cDataContainer>();
    }
    return data;
}
