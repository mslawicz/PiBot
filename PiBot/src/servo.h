/*
 * servo.h
 *
 *  Created on: 29 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_SERVO_H_
#define SRC_SERVO_H_

#include "i2c.h"
#include <vector>

class Servo : public I2cDevice
{
public:
    Servo(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address, uint8_t servoNo);
    void setValue(float value);
private:
    const uint8_t MaxServos = 4;
    uint8_t servoNo;
    const std::vector<uint8_t> TB6612Servo {0, 1, 14, 15};
};

#endif /* SRC_SERVO_H_ */
