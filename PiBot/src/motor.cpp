/*
 * motor.cpp
 *
 *  Created on: 3 lut 2019
 *      Author: Marcin
 */

#include "motor.h"
#include <vector>

Motor::Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority)
    : I2cDevice(busId, address, priority)
{
    // register auto increment enable
    // change on ACK, output totem pole
    writeData(PCA9685Registers::MODE1, std::vector<uint8_t>{0x20, 0x0C});

}

Motor::~Motor()
{
    // TODO Auto-generated destructor stub
}

void Motor::test()
{
    // PWM0 always on, PWM1 always off
    writeData(PCA9685Registers::LED0_ON_L, std::vector<uint8_t>{0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10});
    // PWM14 ~50%, PWM15 ~25% delayed ~0.5T
    writeData(PCA9685Registers::LED0_ON_L + 14*4, std::vector<uint8_t>{0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x0C});
}
