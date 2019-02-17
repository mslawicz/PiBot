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

enum SerialBusId
{
    // I2C buses
    I2C0 = 0x0000,
    I2C1,
    // SPI buses
    SPI_MAIN = 0x0100,
    SPI_AUX
};

enum SerialPriority
{
    PCA9685_PR,
    MOTOR_PR,
    GYROSCOPE_PR,
    ACCELEROMETER_PR,
    MAGNETOMETER_PR,
    DISPLAY_PR
};

class SerialBus;
class SerialDevice;

// typedef for sent/received serial data container:
// data sent container: register address / command, no of bytes requested to read (0=write only), vector of data to send (may be empty when read request)
// data received container: register address / command, no of bytes read (==length of vector), vector of data received
typedef std::tuple<unsigned, unsigned, std::vector<uint8_t>>   SerialDataContainer;

// typedef for serial device definition: priority, pointer to SerialDevice object
typedef std::tuple<SerialPriority, SerialDevice*> SerialDeviceContainer;

// typedef for the map of serial buses: bus id, pointer to SerialBus object
typedef std::map<SerialBusId, SerialBus*> MapOfSerialBuses;


class SerialBus
{
public:
    SerialBus(SerialBusId serialBusId);
    ~SerialBus();
    void startHandler(void);
    void stopHandler(void);
    void registerDevice(SerialDeviceContainer newDevice);
    void unregisterDevice(SerialDevice* pDevice);
    static MapOfSerialBuses buses;
    friend class SerialDevice;
private:
    const std::map<SerialBusId, std::string> names
    {
        {I2C0, "I2C0"},
        {I2C1, "I2C1"},
        {SPI_MAIN, "SPI-main"},
        {SPI_AUX, "SPI-aux"}
    };
    void handler(void);
    void requestToSend(void);
    SerialBusId busId;
    uint8_t* pData;
    const unsigned DataBufSize = 32;
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
    SerialDevice(SerialBusId serialBusId, SerialPriority devicePriority, uint8_t deviceAddres = 0);
    // this makes this class abstract
    virtual ~SerialDevice();
    friend class SerialBus;
    void writeDataRequest(unsigned registerAddress, std::vector<uint8_t> data);
    void readDataRequest(unsigned registerAddress, unsigned length);
    void exchangeDataRequest(unsigned registerAddress, std::vector<uint8_t> data);
    void clearReceiveQueue(void);
    bool receiveQueueEmpty(void) const {return receivedData.empty();}
    SerialDataContainer getData(void);
    SerialDataContainer getLastData(void);
protected:
    virtual int writeData(unsigned handle, unsigned registerAddress, std::vector<uint8_t> data) = 0;
    virtual int readData(unsigned handle, unsigned registerAddress, uint8_t* dataBuffer, unsigned length) = 0;
    virtual int exchangeData(unsigned handle, unsigned registerAddress, std::vector<uint8_t> data, uint8_t* dataBuffer) = 0;
    int handle;
    SerialBusId busId;
    SerialPriority priority;
private:
    uint8_t address;
    SerialBus* pSerialBus;
    std::queue<SerialDataContainer> dataToSend;
    std::queue<SerialDataContainer> receivedData;
    std::mutex sendQueueMutex;
    std::mutex receiveQueueMutex;
};

#endif /* SRC_SERIAL_H_ */
