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

#include <vector>
#include <pigpio.h>

class I2C
{
public:
	I2C(unsigned BusId, unsigned DeviceAddress);
	~I2C();
	std::vector<char> read(unsigned address, unsigned length);
	void write(unsigned address, std::vector<char> data);

private:
	unsigned bus_id;
	unsigned device_address;
	int handle;
	char* p_data;
	const unsigned DataBufSize = 100;
};

#endif /* SRC_I2C_H_ */
