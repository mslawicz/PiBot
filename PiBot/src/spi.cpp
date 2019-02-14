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
