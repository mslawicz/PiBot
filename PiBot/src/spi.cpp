/*
 * spi.cpp
 *
 *  Created on: 13 lut 2019
 *      Author: Marcin
 */

#include "spi.h"
#include "logger.h"
#include "program.h"
#include <algorithm>

MapOfSpiChannels SpiChannel::channels;

SpiChannel::SpiChannel(SpiChannelId spiChannelId)
    : channelId(spiChannelId)
{
    pData = new uint8_t[DataBufSize];
    if(pData == nullptr)
    {
        Program::getInstance().terminate(MEMORY_ALLOCATION_ERROR);
    }
    exitHandler = true;
    pSpiHandlerThread = nullptr;
    queueEmpty.test_and_set();
    // register this SPI channel object in the channel map
    SpiChannel::channels.emplace(channelId, this);
    Logger::getInstance().logEvent(INFO, "SPI channel#", channelId, " initialized");
}

SpiChannel::~SpiChannel()
{
    if(pData != nullptr)
    {
        delete[] pData;
        pData = nullptr;
    }
}

/*
 * creates a new thread for SPI channel handler
 */
void SpiChannel::startHandler(void)
{
    Logger::getInstance().logEvent(INFO, "SPI channel#", channelId, " handler start request");
    exitHandler = false;
    pSpiHandlerThread = new std::thread(&SpiChannel::handler, this);
}

/*
 * initiate stop of SPI channel handler
 */
void SpiChannel::stopHandler(void)
{
    Logger::getInstance().logEvent(INFO, "SPI channel#", channelId, " handler stop request");
    exitHandler = true;
    queueEvent.notify_one();
    pSpiHandlerThread->join();
    delete pSpiHandlerThread;
    Logger::getInstance().logEvent(INFO, "SPI channel#", channelId, " handler terminated");
}

/*
 * SPI bus handler
 * transmit SPI frames for all devices in a channel
 * to be run in a separate thread
 */
void SpiChannel::handler(void)
{
    Logger::getInstance().logEvent(INFO, "SPI channel#", channelId, " handler started");
    do
    {
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(handlerMutex);
        queueEvent.wait(lock, [this]() {return (!queueEmpty.test_and_set() || exitHandler); });
        lock.unlock();

        auto iDevice = devices.begin();
        // iterate through all registered SPI devices until their send queues are empty
        while(iDevice != devices.end())
        {
            //if the queue is not empty - pop message
            if(!std::get<1>(*iDevice)->dataToSend.empty())
            {
                // the sending queue of this SPI device is not empty
                SpiDataContainer dataContainer;
                {
                    // queue modification protected by mutex
                    std::lock_guard<std::mutex> lock(std::get<1>(*iDevice)->sendQueueMutex);
                    dataContainer = std::get<1>(*iDevice)->dataToSend.front();
                    std::get<1>(*iDevice)->dataToSend.pop();
                }
                if(std::get<1>(dataContainer) == 0)
                {
                    // SPI write operation
                    int result = 1;//SSSSPIWriteSPIBlockData(std::get<1>(*iDevice)->handle, std::get<0>(dataContainer), (char*)&std::get<2>(dataContainer)[0], std::get<2>(dataContainer).size());
                    if(result)
                    {
                        Logger::getInstance().logEvent(ERROR, "SPI write error: channel=", channelId, ", error=", result);
                    }
                }
                else
                {
                    // SPI read operation
                    if(std::get<1>(dataContainer) > DataBufSize)
                    {
                        Program::getInstance().terminate(SPI_BUFFER_SIZE);
                    }
                    int no_of_bytes = 1;//SSS SPIReadSPIBlockData(std::get<1>(*iDevice)->handle, std::get<0>(dataContainer), (char*)pData, std::get<1>(dataContainer));
                    std::vector<uint8_t> data(pData, pData+no_of_bytes);
                    {
                        //push received data to receive queue
                        std::lock_guard<std::mutex> lock(std::get<1>(*iDevice)->receiveQueueMutex);
                        std::get<1>(*iDevice)->receivedData.push(SpiDataContainer{std::get<0>(dataContainer), no_of_bytes, data});
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
 * registers SPI device in the vector (sorted by priority)
 */
void SpiChannel::registerDevice(SpiDeviceContainer newDevice)
{
    devices.push_back(newDevice);
    std::sort(devices.begin(), devices.end());
}

/*
 * unregisters SPI device from the vector
 */
void SpiChannel::unregisterDevice(SpiDevice* pDevice)
{
    for (auto iDevice = devices.begin(); iDevice < devices.end(); iDevice++)
    {
        if(std::get<1>(*iDevice) == pDevice)
        {
            devices.erase(iDevice);
            Logger::getInstance().logEvent(INFO, "unregistering SPI device: channel=", channelId, ", priority=0x", pDevice->priority );
            break;
        }
    }
}

/*
 * constructor of a new SPI device
 */
SpiDevice::SpiDevice(SpiChannelId spiChannelId, SpiPriority devicePriority, unsigned bitRate)
    : channelId(spiChannelId)
    , priority(devicePriority)
{
    if(SpiChannel::channels.find(channelId) == SpiChannel::channels.end())
    {
        Program::getInstance().terminate(WRONG_SPI_CHANNEL);
    }

    pSpiChannel = SpiChannel::channels.find(channelId)->second;
    handle = spiOpen(channelId, bitRate, 0);

    if(handle >= 0)
    {
        Logger::getInstance().logEvent(INFO, "SPI device opened: channel=", channelId, ", priority=0x", priority);
    }
    else
    {
        Program::getInstance().terminate(SPI_NOT_OPENED);
    }

    // register this SPI device in the channel object map of devices
    pSpiChannel->registerDevice(SpiDeviceContainer{priority, this});
}

/*
 * pure virtual destructor
 */
SpiDevice::~SpiDevice()
{
    if(handle >= 0)
    {
        spiClose(handle);
    }
    pSpiChannel->unregisterDevice(this);
}
