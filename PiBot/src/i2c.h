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

enum I2cPriority
{
    GYROSCOPE,
    MOTOR,
    ACCELEROMETER,
    MAGNETOMETER
};

enum I2cBusId
{
    I2C0,
    I2C1
};

//class I2C
//{
//public:
//	I2C(unsigned busId, unsigned deviceAddress);
//	~I2C();
//	std::vector<char> read(unsigned address, unsigned length);
//	void write(unsigned address, std::vector<char> data);
//
//private:
//	unsigned bus_id;
//	unsigned device_address;
//	int handle;
//	char* p_data;
//	const unsigned DataBufSize = 100;
//};

class I2cDevice;

class I2cBus
{
public:
	I2cBus(I2cBusId i2cBusId);
	~I2cBus();
    void handler(void);
    void requestToSend(void);
    void startHandler(void);
    void stopHandler(void);
    void registerDevice(std::tuple<uint8_t, unsigned, I2cDevice*> newDevice);
    static std::map<unsigned, I2cBus*> buses;
private:
    I2cBusId busId;
	char* pData;
	const unsigned DataBufSize = 100;
    std::mutex sendQueueMutex;
    std::condition_variable queueEvent;
    bool exitHandler;
    std::thread* pI2cHandlerThread;
    std::atomic_flag queueEmpty;
    std::vector<std::tuple<uint8_t, unsigned, I2cDevice*>> devices;
};

class I2cDevice
{
public:
	I2cDevice(I2cBusId i2cBusId, unsigned deviceAddres, I2cPriority devicePriority);
	~I2cDevice();
private:
	I2cBusId busId;
	unsigned address;
	I2cPriority priority;
	int handle;
	I2cBus* pI2cBus;
};

#endif /* SRC_I2C_H_ */
