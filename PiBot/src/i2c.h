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
	std::vector<char> Read(unsigned address, unsigned length);

private:
	unsigned m_BusId;
	unsigned m_DeviceAddress;
	int m_handle;
	char* m_pDataBuf;
	const unsigned m_DataBufSize = 100;
};

#endif /* SRC_I2C_H_ */
