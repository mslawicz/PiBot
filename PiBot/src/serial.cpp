/*
 * serial.cpp
 *
 *  Created on: 16 lut 2019
 *      Author: Marcin
 */

#include "serial.h"

MapOfSerialBuses SerialBus::buses;


/*
 * constructor of the serial bus object
 */
SerialBus::SerialBus(SerialBusId serialBusId)
    : busId(serialBusId)
{
    pData = new uint8_t[DataBufSize];
    if(pData == nullptr)
    {
        Program::getInstance().terminate(MEMORY_ALLOCATION_ERROR);
    }
    exitHandler = true;
    pSerialHandlerThread = nullptr;
    queueEmpty.test_and_set();
    // register this serial bus object in the bus map
    SerialBus::buses.emplace(busId, this);
    Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " initialized");
}

SerialBus::~SerialBus()
{
    if(pData != nullptr)
    {
        delete[] pData;
        pData = nullptr;
    }
}

/*
 * serial bus handler
 * transmit serial frames for all devices in a bus
 * to be run in a separate thread
 */
void SerialBus::handler(void)
{
    Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " handler started");
    do
    {
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(handlerMutex);
        queueEvent.wait(lock, [this]() {return (!queueEmpty.test_and_set() || exitHandler); });
        lock.unlock();

        auto iDevice = devices.begin();
        // iterate through all registered serial devices until their send queues are empty
        while(iDevice != devices.end())
        {
            //if the queue is not empty - pop message
            if(!std::get<1>(*iDevice)->dataToSend.empty())
            {
                // the sending queue of this serial device is not empty
                SerialDataContainer dataContainer;
                {
                    // queue modification protected by mutex
                    std::lock_guard<std::mutex> lock(std::get<1>(*iDevice)->sendQueueMutex);
                    dataContainer = std::get<1>(*iDevice)->dataToSend.front();
                    std::get<1>(*iDevice)->dataToSend.pop();
                }
                if(std::get<1>(dataContainer) == 0)
                {
                    // serial write operation
                    int result = serialWriteI2CBlockData(std::get<1>(*iDevice)->handle, std::get<0>(dataContainer), (char*)&std::get<2>(dataContainer)[0], std::get<2>(dataContainer).size());
                    if(result)
                    {
                        Logger::getInstance().logEvent(ERROR, "I2C write error: bus=", busId, ", device=", std::hex, std::get<1>(*iDevice)->address, ", error=", result);
                    }
                }
                else
                {
                    // serial read operation
                    if(std::get<1>(dataContainer) > DataBufSize)
                    {
                        Program::getInstance().terminate(I2C_BUFFER_SIZE);
                    }
                    int no_of_bytes = serialReadI2CBlockData(std::get<1>(*iDevice)->handle, std::get<0>(dataContainer), (char*)pData, std::get<1>(dataContainer));
                    std::vector<uint8_t> data(pData, pData+no_of_bytes);
                    {
                        //push received data to receive queue
                        std::lock_guard<std::mutex> lock(std::get<1>(*iDevice)->receiveQueueMutex);
                        std::get<1>(*iDevice)->receivedData.push(SerialDataContainer{std::get<0>(dataContainer), no_of_bytes, data});
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
void SerialBus::requestToSend(void)
{
    queueEmpty.clear();
    queueEvent.notify_one();
}

/*
 * creates a new thread for serial bus handler
 */
void SerialBus::startHandler(void)
{
    Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " handler start request");
    exitHandler = false;
    pSerialHandlerThread = new std::thread(&SerialBus::handler, this);
}

/*
 * initiate stop of serial bus handler
 */
void SerialBus::stopHandler(void)
{
    Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " handler stop request");
    exitHandler = true;
    queueEvent.notify_one();
    pSerialHandlerThread->join();
    delete pSerialHandlerThread;
    Logger::getInstance().logEvent(INFO, "I2C bus #", busId, " handler terminated");
}

/*
 * registers serial device in the vector (sorted by priority)
 */
void SerialBus::registerDevice(SerialDeviceContainer newDevice)
{
    devices.push_back(newDevice);
    std::sort(devices.begin(), devices.end());
}

/*
 * unregisters serial device from the vector
 */
void SerialBus::unregisterDevice(SerialDevice* pDevice)
{
    for (auto iDevice = devices.begin(); iDevice < devices.end(); iDevice++)
    {
        if(std::get<1>(*iDevice) == pDevice)
        {
            devices.erase(iDevice);
            Logger::getInstance().logEvent(INFO, "unregistering I2C device: bus=", busId, ", address=0x", std::hex, pDevice->address, ", priority=0x", pDevice->priority );
            break;
        }
    }
}

/*
 * constructor of a new serial device
 */
SerialDevice::SerialDevice(SerialBusId serialBusId, SerialDeviceAddress deviceAddres, SerialPriority devicePriority)
    : busId(serialBusId)
    , address(deviceAddres)
    , priority(devicePriority)
{
    if(SerialBus::buses.find(busId) == SerialBus::buses.end())
    {
        Program::getInstance().terminate(WRONG_I2C_BUS);
    }

    pSerialBus = SerialBus::buses.find(busId)->second;
    handle = serialOpen(busId, address, 0);

    if(handle >= 0)
    {
        Logger::getInstance().logEvent(INFO, "I2C device opened: bus=", busId, ", address=0x", std::hex, address, " priority=0x", priority);
    }
    else
    {
        Program::getInstance().terminate(I2C_NOT_OPENED);
    }

    // register this serial device in the bus object map of devices
    pSerialBus->registerDevice(SerialDeviceContainer{priority, this});
}

/*
 * pure virtual destructor
 */
SerialDevice::~SerialDevice()
{
    if(handle >= 0)
    {
        serialClose(handle);
    }
    pSerialBus->unregisterDevice(this);
}

/*
 * puts serial data to send into send queue and notifies serial bus handler
 */
void SerialDevice::writeData(unsigned registerAddress, std::vector<uint8_t> data)
{
    {
        std::lock_guard<std::mutex> lock(sendQueueMutex);
        dataToSend.push(SerialDataContainer{registerAddress, 0, data});
    }
    pSerialBus->requestToSend();
}

/*
 * puts serial data to initiate reception into send queue and notifies serial bus handler
 */
void SerialDevice::readDataRequest(unsigned registerAddress, unsigned length)
{
    {
        std::lock_guard<std::mutex> lock(sendQueueMutex);
        dataToSend.push(SerialDataContainer{registerAddress, length, std::vector<uint8_t>()});
    }
    pSerialBus->requestToSend();
}

/*
 * clears device reception queue without reading it
 */
void SerialDevice::clearReceiveQueue(void)
{
    std::lock_guard<std::mutex> lock(receiveQueueMutex);
    receivedData = std::queue<SerialDataContainer>();
}

/*
 * gets data container from the receive queue
 */
SerialDataContainer SerialDevice::getData(void)
{
    SerialDataContainer data;
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
SerialDataContainer SerialDevice::getLastData(void)
{
    SerialDataContainer data;
    if(!receivedData.empty())
    {
        std::lock_guard<std::mutex> lock(receiveQueueMutex);
        data = receivedData.back();
        receivedData = std::queue<SerialDataContainer>();
    }
    return data;
}
