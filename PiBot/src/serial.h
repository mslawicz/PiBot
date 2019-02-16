/*
 * serial.h
 *
 *  Created on: 16 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_SERIAL_H_
#define SRC_SERIAL_H_

#include <vector>
#include <tuple>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

class SerialBus;
class SerialDevice;

// typedef for sent/received serial data container:
// data sent container: register address, no of bytes requested to read (0=write only), vector of data to send (may be empty when read request)
// data received container: register address, no of bytes read (==length of vector), vector of data received
typedef std::tuple<unsigned, unsigned, std::vector<uint8_t>>   SerialDataContainer;

// typedef for serial device definition: priority, pointer to SerialDevice object
typedef std::tuple<unsigned, SerialDevice*> SerialDeviceContainer;

// typedef for the map of serial buses: bus id, pointer to SerialBus object
typedef std::map<unsigned, SerialBus*> MapOfSerialBuses;


class SerialBus
{
public:
    SerialBus(unsigned serialBusId);
    ~SerialBus();
    void startHandler(void);
    void stopHandler(void);
    static MapOfSerialBuses buses;
    friend class SerialDevice;
private:
    void handler(void);
    void requestToSend(void);
    void registerDevice(SerialDeviceContainer newDevice);
    void unregisterDevice(SerialDevice* pDevice);
    unsigned busId;
    uint8_t* pData;
    const unsigned DataBufSize = 30;
    std::mutex handlerMutex;
    std::condition_variable queueEvent;
    bool exitHandler;
    std::thread* pSerialHandlerThread;
    std::atomic_flag queueEmpty;
    std::vector<SerialDeviceContainer> devices;
};

class SerialDevice
{
public:
    SerialDevice(unsigned serialBusId, unsigned devicePriority, uint8_t deviceAddres = 0);
    // this makes this class abstract
    virtual ~SerialDevice() = 0;
    friend class SerialBus;
    void writeDataRequest(unsigned registerAddress, std::vector<uint8_t> data);
    void readDataRequest(unsigned registerAddress, unsigned length);
    void exchangeDataRequest(std::vector<uint8_t> data);
    void clearReceiveQueue(void);
    bool receiveQueueEmpty(void) const {return receivedData.empty();}
    SerialDataContainer getData(void);
    SerialDataContainer getLastData(void);
private:
    unsigned busId;
    uint8_t address;
    unsigned priority;
    int handle;
    SerialBus* pSerialBus;
    std::queue<SerialDataContainer> dataToSend;
    std::queue<SerialDataContainer> receivedData;
    std::mutex sendQueueMutex;
    std::mutex receiveQueueMutex;
};

#endif /* SRC_SERIAL_H_ */
