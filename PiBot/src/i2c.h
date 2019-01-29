/*
 * i2c.h
 *
 *  Created on: 6 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#define I2C0	0
#define I2C1	1

#include <pigpio.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>

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


class I2cBus
{
public:
	I2cBus(unsigned id);
	~I2cBus();
    void handler(void);
    void notify(void);
    void startHandler(void);
    void stopHandler(void);
    static std::map<unsigned, I2cBus*> buses;
private:
	unsigned busId;
	char* pData;
	const unsigned DataBufSize = 100;
    std::mutex notificationMutex;
    std::condition_variable queueEvent;
    bool exitHandler;
    std::thread* pI2cHandlerThread;
    bool queueNotEmpty;
};

class I2cDevice
{
public:
	I2cDevice(unsigned i2cBusId, unsigned deviceAddres, uint8_t devicePriority);
	~I2cDevice();
private:
	unsigned busId;
	unsigned address;
	uint8_t priority;
	int handle;
};

#endif /* SRC_I2C_H_ */
