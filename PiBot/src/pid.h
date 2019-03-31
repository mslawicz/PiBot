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
    void setKp(float value) {kP = value;}
    void setKi(float value) {kI = value;}
    void setKd(float value) {kD = value;}
    float getKp(void) const {return kP;}
    float getKi(void) const {return kI;}
    float getKd(void) const {return kD;}
    float getProportional(void) const {return proportional;}
    float getIntegral(void) const {return integral;}
    float getDerivative(void) const {return derivative;}
private:
    float kP, kI, kD;
    float previousError;
    float integralError;
    float proportional;
    float integral;
    float derivative;
};

#endif /* SRC_PID_H_ */
