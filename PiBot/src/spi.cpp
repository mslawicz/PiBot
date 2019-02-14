/*
 * spi.cpp
 *
 *  Created on: 13 lut 2019
 *      Author: Marcin
 */

#include "spi.h"
#include "logger.h"
#include "program.h"

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
    // register this i2c bus object in the bus map
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
    //SSS pI2cHandlerThread = new std::thread(&I2cBus::handler, this);
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
