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
    void setP(float value) {kP = value;}
    void setI(float value) {kI = value;}
    void setD(float value) {kD = value;}
    float getP(void) const {return kP;}
    float getI(void) const {return kI;}
    float getD(void) const {return kD;}
private:
    float kP, kI, kD;
    float previousError;
    float integral;
};

#endif /* SRC_PID_H_ */
