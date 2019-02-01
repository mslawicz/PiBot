/*
 * i2c.h
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include <pigpio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <atomic>
#include <tuple>
#include <queue>

enum I2cPriority
{
    GYROSCOPE_PR,
    MOTOR_PR,
    ACCELEROMETER_PR,
    MAGNETOMETER_PR
};



enum I2cBusId
{
    I2C0,
    I2C1
};

class I2cBus;
class I2cDevice;

// typedef for sent/received i2c data container:
// data sent container: register address, no of bytes requested to read (0=write only), vector of data to send (may be empty when read request)
// data received container: register address, no of bytes read (==length of vector), vector of data received
typedef std::tuple<unsigned, unsigned, std::vector<char>>   I2cDataContainer;

// typedef for i2c device definition: priority, pointer to I2cDevice object
typedef std::tuple<I2cPriority, I2cDevice*> I2cDeviceContainer;

// typedef for the map of i2c buses: bus id, pointer to I2cBus object
typedef std::map<I2cBusId, I2cBus*> MapOfI2cBuses;


class I2cBus
{
public:
	I2cBus(I2cBusId i2cBusId);
	~I2cBus();
    void startHandler(void);
    void stopHandler(void);
    static MapOfI2cBuses buses;
    friend class I2cDevice;
private:
    void handler(void);
    void requestToSend(void);
    void registerDevice(I2cDeviceContainer newDevice);
    I2cBusId busId;
	char* pData;
	const unsigned DataBufSize = 30;
    std::mutex handlerMutex;
    std::condition_variable queueEvent;
    bool exitHandler;
    std::thread* pI2cHandlerThread;
    std::atomic_flag queueEmpty;
    std::vector<I2cDeviceContainer> devices;
};

class I2cDevice
{
public:
	I2cDevice(I2cBusId i2cBusId, unsigned deviceAddres, I2cPriority devicePriority);
	// this makes this class abstract
	virtual ~I2cDevice() = 0;
	friend class I2cBus;
	void writeData(unsigned registerAddress, std::vector<char> data);
	void readDataRequest(unsigned registerAddress, unsigned length);
private:
	I2cBusId busId;
	unsigned address;
	I2cPriority priority;
	int handle;
	I2cBus* pI2cBus;
    std::queue<I2cDataContainer> dataToSend;
    std::queue<I2cDataContainer> receivedData;
    std::mutex sendQueueMutex;
    std::mutex receiveQueueMutex;
};

#endif /* SRC_I2C_H_ */
