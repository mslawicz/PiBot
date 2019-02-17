/*
 * spi.h
 *
 *  Created on: 13 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_

#include "serial.h"
#include <mutex>
#include <queue>
#include "gpio.h"

class SpiDevice : public SerialDevice
{
public:
      SpiDevice(SerialBusId spiBusId, SerialPriority devicePriority, unsigned bitRate, GpioPin pinCD, unsigned flags = 0);
      // this makes this class abstract
      ~SpiDevice();
private:
      int writeData(unsigned handle, unsigned registerAddress, std::vector<uint8_t> data) override;
      int readData(unsigned handle, unsigned registerAddress, uint8_t* dataBuffer, unsigned length) override;
      int exchangeData(unsigned handle, unsigned registerAddress, std::vector<uint8_t> data, uint8_t* dataBuffer) override;
      SerialBus* pSerialBus;
      std::queue<SerialDataContainer> dataToSend;
      std::queue<SerialDataContainer> receivedData;
      std::mutex sendQueueMutex;
      std::mutex receiveQueueMutex;
      GPIO* pPinCD;
};

#endif /* SRC_SPI_H_ */
