/*
 * pid.cpp
 *
 *  Created on: 11 lut 2019
 *      Author: Marcin
 */

#include "pid.h"

PID::PID(float coefficientP, float coefficientI, float coefficientD, float integralLimit)
    : kP(coefficientP)
    , kI(coefficientI)
    , kD(coefficientD)
    , limit(integralLimit)
{
    previousError = 0.0f;
    proportional = 0.0f;
    integral = 0.0f;
    derivative = 0.0f;
}

float PID::calculate(float setPointValue, float measuredProcessValue, float dt)
{
    float error = measuredProcessValue - setPointValue;
    proportional = kP * error;
    integral += kI * error * dt;
    if(integral > limit)
    {
        integral = limit;
    }
    else if (integral < -limit)
    {
        integral = -limit;
    }
    derivative = kD * (error - previousError) / dt;
    float output = proportional + integral + derivative;
    previousError = error;
    return output;
}
