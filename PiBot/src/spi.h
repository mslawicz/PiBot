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
#include <tuple>
#include <vector>
#include <queue>

enum SpiChannelId
{
    SPI_MAIN,
    SPI_AUX
};

enum SpiPriority
{
    DISPLAY
};

class SpiChannel;
class SpiDevice;

// typedef for sent/received SPI data container:
// data sent container: register address, no of bytes requested to read (0=write only), vector of data to send (may be empty when read request)
// data received container: register address, no of bytes read (==length of vector), vector of data received
typedef std::tuple<unsigned, unsigned, std::vector<uint8_t>> SpiDataContainer;

// typedef for SPI device definition: priority, pointer to SPI object
typedef std::tuple<SpiPriority, SpiDevice*> SpiDeviceContainer;

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
      friend class SpiDevice;
private:
      void handler(void);
//    void requestToSend(void);
      void registerDevice(SpiDeviceContainer newDevice);
      void unregisterDevice(SpiDevice* pDevice);
      SpiChannelId channelId;
      uint8_t* pData;
      const unsigned DataBufSize = 30;
      std::mutex handlerMutex;
      std::condition_variable queueEvent;
      bool exitHandler;
      std::thread* pSpiHandlerThread;
      std::atomic_flag queueEmpty;
      std::vector<SpiDeviceContainer> devices;
};

class SpiDevice
{
public:
      SpiDevice(SpiChannelId spiChannelId, SpiPriority devicePriority, unsigned bitRate, unsigned flags = 0);
      // this makes this class abstract
      virtual ~SpiDevice() = 0;
      friend class SpiChannel;
//    void writeData(unsigned registerAddress, std::vector<uint8_t> data);
//    void readDataRequest(unsigned registerAddress, unsigned length);
//    void clearReceiveQueue(void);
//    bool receiveQueueEmpty(void) const {return receivedData.empty();}
//    I2cDataContainer getData(void);
//    I2cDataContainer getLastData(void);
private:
      SpiChannelId channelId;
      SpiPriority priority;
      int handle;
      SpiChannel* pSpiChannel;
      std::queue<SpiDataContainer> dataToSend;
      std::queue<SpiDataContainer> receivedData;
      std::mutex sendQueueMutex;
      std::mutex receiveQueueMutex;
};

#endif /* SRC_SPI_H_ */
