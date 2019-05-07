/*
 * pid.h
 *
 *  Created on: 11 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_PID_H_
#define SRC_PID_H_

#include <stdint.h>

class PID
{
public:
    PID(float coefficientP, float coefficientI, float coefficientD, float integralLlimit = 1.0);
    float calculate(float setPointValue, float measuredProcessValue, float derivativeInput, float dt);
    void setKp(float value) {kP = value;}
    void setKi(float value) {kI = value;}
    void setKd(float value) {kD = value;}
    float getKp(void) const {return kP;}
    float getKi(void) const {return kI;}
    float getKd(void) const {return kD;}
    float getProportional(void) const {return proportional;}
    float getIntegral(void) const {return integral;}
    float getDerivative(void) const {return derivative;}
    void reset(void) { proportional = integral = derivative = 0.0; }
private:
    float kP, kI, kD;
    float proportional;
    float integral;
    float derivative;
    float limit;
};

#endif /* SRC_PID_H_ */
