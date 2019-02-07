/*
 * PCA9685.h
 *
 *  Created on: 3 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_PCA9685_H_
#define SRC_PCA9685_H_

#include "i2c.h"

enum PCA9685Registers
{
    MODE1 = 0x00,
    LED0_ON_L = 0x06,
    PRE_SCALE = 0xFE
};


/*
 * class of a PCA9685 control device
 * its purpose is an one-time configuration of the device
 */
class PCA9685 : public I2cDevice
{
public:
    PCA9685(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority);
};

#endif /* SRC_PCA9685_H_ */
