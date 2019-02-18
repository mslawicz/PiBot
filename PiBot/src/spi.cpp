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
SpiDevice::SpiDevice(SerialBusId spiBusId, SerialPriority devicePriority, unsigned bitRate, GpioPin pinCD, unsigned flags)
    : SerialDevice(spiBusId, devicePriority, 0)
{
    handle = spiOpen(busId & 0xFF, bitRate, flags);

    if(handle >= 0)
    {
        Logger::getInstance().logEvent(INFO, "SPI device opened: bus=0x", std::hex, busId, ", priority=0x", priority);
    }
    else
    {
        Program::getInstance().terminate(SPI_NOT_OPENED);
    }

    pPinCD = new GPIO(pinCD, PI_OUTPUT);
    pPinCD->write(1);
}


SpiDevice::~SpiDevice()
{
    delete pPinCD;
}

/*
 * writes data to SPI device
 */
int SpiDevice::writeData(unsigned handle, unsigned command, std::vector<uint8_t> data)
{
    if(command > 0)
    {
        // command must be the first byte
        pPinCD->write(0);
        data.emplace(data.begin(), (char)command);
    }
    else
    {
        // write data only
        pPinCD->write(1);
    }

    return spiWrite(handle, (char*)&data[0], data.size()) != static_cast<int>(data.size());
}

/*
 * reads data from SPI device
 */
int SpiDevice::readData(unsigned handle, unsigned command, uint8_t* dataBuffer, unsigned length)
{
    int noOfBytesReceived;

    if(command > 0)
    {
        // command must be sent before read
        pPinCD->write(0);
        uint8_t dataToWrite[length+1] = {0};
        dataToWrite[0] = command & 0xFF;
        noOfBytesReceived = spiXfer(handle, (char*)dataToWrite, (char*)dataBuffer, length+1);
    }
    else
    {
        // read data only
        pPinCD->write(1);
        noOfBytesReceived = spiRead(handle, (char*)dataBuffer, length);
    }

    return noOfBytesReceived;
}

