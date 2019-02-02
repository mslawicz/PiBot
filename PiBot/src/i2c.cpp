/*
 * i2c.cpp
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

#include "i2c.h"
#include "logger.h"
#include "program.h"
#include <algorithm>
#include <stdlib.h>

MapOfI2cBuses I2cBus::buses;


/*
 * constructor of the i2c bus object
 */
I2cBus::I2cBus(I2cBusId i2cBusId)
	: busId(i2cBusId)
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
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(handlerMutex);
        queueEvent.wait(lock, [this]() {return (!queueEmpty.test_and_set() || exitHandler); });
        lock.unlock();

        auto iDevice = devices.begin();
        // iterate through all registered i2c devices until their send queues are empty
        while(iDevice != devices.end())
        {
            //if the queue is not empty - pop message
            if(!std::get<1>(*iDevice)->dataToSend.empty())
            {
                // the sending queue of this i2c device is not empty
                I2cDataContainer dataContainer;
                {
                    // queue modification protected by mutex
                    std::lock_guard<std::mutex> lock(std::get<1>(*iDevice)->sendQueueMutex);
                    dataContainer = std::get<1>(*iDevice)->dataToSend.front();
                    std::get<1>(*iDevice)->dataToSend.pop();
                }
                if(std::get<1>(dataContainer) == 0)
                {
                    // i2c write operation
                    int result = i2cWriteI2CBlockData(std::get<1>(*iDevice)->handle, std::get<0>(dataContainer), &std::get<2>(dataContainer)[0], std::get<2>(dataContainer).size());
                    if(result)
                    {
                        Logger::getInstance().logEvent(ERROR, "I2C write error: bus=", busId, " device=", std::get<1>(*iDevice)->address);
                    }
                }
                else
                {
                    // i2c read operation
                    if(std::get<1>(dataContainer) > DataBufSize)
                    {
                        Program::getInstance().terminate(I2C_BUFFER_SIZE);
                    }
                    int no_of_bytes = i2cReadI2CBlockData(std::get<1>(*iDevice)->handle, std::get<0>(dataContainer), pData, std::get<1>(dataContainer));
                    std::vector<char> data(pData, pData+no_of_bytes);
                    {
                        //push received data to receive queue
                        std::lock_guard<std::mutex> lock(std::get<1>(*iDevice)->receiveQueueMutex);
                        std::get<1>(*iDevice)->receivedData.push(I2cDataContainer{});
                    }
                }

                // after transmission start from the beginning
                iDevice = devices.begin();
            }
            else
            {
                // if nothing to transmit - move to next device
                iDevice++;
            }
        }

    } while (!exitHandler);
}

/*
 * notify handler about queue activity
 */
void I2cBus::requestToSend(void)
{
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

/*
 * registers i2c device in the vector (sorted by priority)
 */
void I2cBus::registerDevice(I2cDeviceContainer newDevice)
{
    devices.push_back(newDevice);
    std::sort(devices.begin(), devices.end());
}

/*
 * constructor of a new i2c device
 */
I2cDevice::I2cDevice(I2cBusId i2cBusId, I2cDeviceAddress deviceAddres, I2cPriority devicePriority)
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
}

/*
 * puts i2c data to send into send queue and notifies i2c bus handler
 */
void I2cDevice::writeData(unsigned registerAddress, std::vector<char> data)
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
        dataToSend.push(I2cDataContainer{registerAddress, length, std::vector<char>()});
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
 * reads data container from the receive queue
 */
I2cDataContainer I2cDevice::readData(void)
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
