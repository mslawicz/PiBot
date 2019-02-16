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

class SpiDevice : public SerialDevice
{
public:
      SpiDevice(SerialBusId spiBusId, SerialPriority devicePriority, unsigned bitRate, unsigned flags = 0);
      // this makes this class abstract
      ~SpiDevice();
private:
      SerialBusId busId;
      SerialPriority priority;
      SerialBus* pSerialBus;
      std::queue<SerialDataContainer> dataToSend;
      std::queue<SerialDataContainer> receivedData;
      std::mutex sendQueueMutex;
      std::mutex receiveQueueMutex;
};

#endif /* SRC_SPI_H_ */
