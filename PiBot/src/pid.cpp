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
    proportional = 0.0f;
    integral = 0.0f;
    derivative = 0.0f;
    previousError = 0.0f;
    cumulativeError = 0.0f;
}

float PID::calculate(float setPointValue, float measuredProcessValue, float derivativeInput, float dt)
{
    float error = setPointValue - measuredProcessValue;
    proportional = kP * error;
    cumulativeError += error * dt;
    integral = kI * cumulativeError;
    if(integral > limit)
    {
        cumulativeError = kI * limit;
    }
    else if (integral < -limit)
    {
        cumulativeError = -kI * limit;
    }
    derivative = kD * derivativeInput;
    float output = proportional + integral + derivative;
    return output;
}

float PID::calculate(float setPointValue, float measuredProcessValue, float dt)
{
    float error = setPointValue - measuredProcessValue;
    proportional = kP * error;
    cumulativeError += error * dt;
    integral = kI * cumulativeError;
    if(integral > limit)
    {
        cumulativeError = kI * limit;
    }
    else if (integral < -limit)
    {
        cumulativeError = -kI * limit;
    }
    derivative = kD * (error - previousError) / dt;
    previousError = error;
    float output = proportional + integral + derivative;
    return output;
}
