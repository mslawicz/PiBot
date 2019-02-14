/*
 * spi.h
 *
 *  Created on: 13 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_

#include <map>
#include <thread>
#include <condition_variable>

enum SpiChannelId
{
    MainSPI,
    AuxSPI
};

enum SpiPriority
{
    DISPLAY
};

class SpiChannel;
class SpiDevice;

// typedef for the map of SPI channels: channel id, pointer to SpiChannel object
typedef std::map<SpiChannelId, SpiChannel*> MapOfSpiChannels;

class SpiChannel
{
public:
      SpiChannel(SpiChannelId spiChannelId);
      ~SpiChannel();
      void startHandler(void);
      void stopHandler(void);
      static MapOfSpiChannels channels;
//    friend class I2cDevice;
private:
//    void handler(void);
//    void requestToSend(void);
//    void registerDevice(I2cDeviceContainer newDevice);
//    void unregisterDevice(I2cDevice* pDevice);
      SpiChannelId channelId;
      uint8_t* pData;
      const unsigned DataBufSize = 30;
//    std::mutex handlerMutex;
      std::condition_variable queueEvent;
      bool exitHandler;
      std::thread* pSpiHandlerThread;
      std::atomic_flag queueEmpty;
//    std::vector<I2cDeviceContainer> devices;
};

class SpiDevice
{
public:
//    I2cDevice(I2cBusId i2cBusId, I2cDeviceAddress deviceAddres, I2cPriority devicePriority);
//    // this makes this class abstract
//    virtual ~I2cDevice() = 0;
//    friend class I2cBus;
//    void writeData(unsigned registerAddress, std::vector<uint8_t> data);
//    void readDataRequest(unsigned registerAddress, unsigned length);
//    void clearReceiveQueue(void);
//    bool receiveQueueEmpty(void) const {return receivedData.empty();}
//    I2cDataContainer getData(void);
//    I2cDataContainer getLastData(void);
private:
//    I2cBusId busId;
//    I2cDeviceAddress address;
//    I2cPriority priority;
//    int handle;
//    I2cBus* pI2cBus;
//    std::queue<I2cDataContainer> dataToSend;
//    std::queue<I2cDataContainer> receivedData;
//    std::mutex sendQueueMutex;
//    std::mutex receiveQueueMutex;
};

#endif /* SRC_SPI_H_ */
