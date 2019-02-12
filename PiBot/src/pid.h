/*
 * pid.h
 *
 *  Created on: 11 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_PID_H_
#define SRC_PID_H_

class PID
{
public:
    PID(float coefficientP, float coefficientI, float coefficientD);
    float calculate(float setPointValue, float measuredProcessValue);
private:
    float kP, kI, kD;
    float previousError;
    float integral;
};

#endif /* SRC_PID_H_ */
