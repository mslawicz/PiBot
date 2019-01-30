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

std::map<unsigned, I2cBus*> I2cBus::buses;

//I2C::I2C(unsigned busId, unsigned deviceAddress)
//	: bus_id(busId)
//	, device_address(deviceAddress)
//{
//	p_data = new char[DataBufSize];
//	if(p_data == nullptr)
//	{
//		Program::getInstance().terminate(MEMORY_ALLOCATION_ERROR);
//	}
//	handle = i2cOpen(busId, deviceAddress, 0);
//	if(handle >= 0)
//	{
//		Logger::getInstance().logEvent(INFO, "I2C device opened: bus=", busId, ", address=0x", std::hex, deviceAddress);
//	}
//	else
//	{
//		delete[] p_data;
//		Program::getInstance().terminate(I2C_NOT_OPENED);
//	}
//}
//
//I2C::~I2C()
//{
//	if(handle >= 0)
//	{
//		i2cClose(handle);
//	}
//	if(p_data != nullptr)
//	{
//		delete[] p_data;
//	}
//}
//
//std::vector<char> I2C::read(unsigned address, unsigned length)
//{
//	if(length > DataBufSize)
//	{
//		length = DataBufSize;
//		//TODO this improper scenario should be handled here
//	}
//	int no_of_bytes = i2cReadI2CBlockData(handle, address, p_data, length);
//	std::vector<char> data(p_data, p_data+no_of_bytes);
//	return data;
//}
//
//void I2C::write(unsigned address, std::vector<char> data)
//{
//	int result = i2cWriteI2CBlockData(handle, address, &data[0], data.size());
//	if(result)
//	{
//		//TODO write error handling
//	}
//}

/*
 * constructor of the i2c bus object
 */
I2cBus::I2cBus(unsigned id)
	: busId(id)
{
	pData = new char[DataBufSize];
	if(pData == nullptr)
	{
		Program::getInstance().terminate(MEMORY_ALLOCATION_ERROR);
	}
    exitHandler = true;
    pI2cHandlerThread = nullptr;
    queueEmpty.test_and_set();
    // register this i2c bus object in the bus map
    I2cBus::buses.emplace(busId, this);
    Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " initialized");
}

I2cBus::~I2cBus()
{
	if(pData != nullptr)
	{
		delete[] pData;
		pData = nullptr;
	}
}

/*
 * i2c bus handler
 * transmit i2c frames for all devices in a bus
 * to be run in a separate thread
 */
void I2cBus::handler(void)
{
	Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " handler started");
    do
    {
        std::cout << ">>> handler loop start\n";
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(sendQueueMutex);
        queueEvent.wait(lock, [this]() {return (!queueEmpty.test_and_set() || exitHandler); });
        lock.unlock();
        std::cout << ">>> handler loop continue\n";
        {
        	// body of loop for each registered i2c device
        	{
        		// body of while loop for every message in a queue
        		{
        			// pop message from a queue here
        			std::lock_guard<std::mutex> lock(sendQueueMutex);
        		}
        		// i2c transmission
        		std::this_thread::sleep_for(std::chrono::milliseconds(50));
        		{
        			std::lock_guard<std::mutex> lock(sendQueueMutex);
        			// if data is received then it should be placed in a queue
        		}
        	}
        }
        std::cout << ">>> handler loop end\n";
    } while (!exitHandler);
    std::cout << ">>> exiting handler\n";
}

/*
 * notify handler about queue activity
 */
void I2cBus::requestToSend(void)
{
    std::cout << "sending notification to handler\n";
    queueEmpty.clear();
    queueEvent.notify_one();
}

/*
 * creates a new thread for i2c bus handler
 */
void I2cBus::startHandler(void)
{
	Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " handler start request");
    exitHandler = false;
    pI2cHandlerThread = new std::thread(&I2cBus::handler, this);
}

/*
 * initiate stop of i2c bus handler
 */
void I2cBus::stopHandler(void)
{
    Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " handler stop request");
    exitHandler = true;
    queueEvent.notify_one();
    pI2cHandlerThread->join();
    delete pI2cHandlerThread;
}

void I2cBus::registerDevice(I2cDeviceParameters deviceParameters)
{

}

I2cDevice::I2cDevice(unsigned i2cBusId, unsigned deviceAddres, uint8_t devicePriority)
	: busId(i2cBusId)
	, address(deviceAddres)
	, priority(devicePriority)
{
	if(I2cBus::buses.find(busId) == I2cBus::buses.end())
	{
		Program::getInstance().terminate(WRONG_I2C_BUS);
	}

	pI2cBus = I2cBus::buses.find(busId)->second;
	handle = i2cOpen(busId, address, 0);

	if(handle >= 0)
	{
		Logger::getInstance().logEvent(INFO, "I2C device opened: bus=", busId, ", address=0x", std::hex, address);
	}
	else
	{
		Program::getInstance().terminate(I2C_NOT_OPENED);
	}

	// register this i2c device in the bus object map of devices
	pI2cBus->registerDevice(I2cDeviceParameters{address, priority, this});
}

I2cDevice::~I2cDevice()
{
	if(handle >= 0)
	{
		i2cClose(handle);
	}
}
