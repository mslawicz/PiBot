/*
 * spi.cpp
 *
 *  Created on: 13 lut 2019
 *      Author: Marcin
 */

#include "spi.h"
#include "logger.h"
#include "program.h"



/*
 * constructor of a new SPI device
 */
SpiDevice::SpiDevice(SerialBusId spiBusId, SerialPriority devicePriority, unsigned bitRate, unsigned flags)
    : SerialDevice(spiBusId, devicePriority, 0)
    , busId(spiBusId)
    , priority(devicePriority)
{
    if(SerialBus::buses.find(busId) == SerialBus::buses.end())
    {
        Program::getInstance().terminate(WRONG_SPI_BUS);
    }

    pSerialBus = SerialBus::buses.find(busId)->second;
    handle = spiOpen(busId & 0xFF, bitRate, flags);

    if(handle >= 0)
    {
        Logger::getInstance().logEvent(INFO, "SPI device opened: bus=", busId, ", priority=0x", priority);
    }
    else
    {
        Program::getInstance().terminate(SPI_NOT_OPENED);
    }

    // register this SPI device in the channel object map of devices
    pSerialBus->registerDevice(SerialDeviceContainer{priority, this});
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
    pSerialBus->unregisterDevice(this);
}
