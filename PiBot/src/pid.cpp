/*
 * pid.cpp
 *
 *  Created on: 11 lut 2019
 *      Author: Marcin
 */

#include "pid.h"

PID::PID(float coefficientP, float coefficientI, float coefficientD)
    : kP(coefficientP)
    , kI(coefficientI)
    , kD(coefficientD)
{
    previousError = 0.0f;
    integralError = 0.0f;
    proportional = 0.0f;
    integral = 0.0f;
    derivative = 0.0f;
}

float PID::calculate(float setPointValue, float measuredProcessValue, float dt)
{
    float error = setPointValue - measuredProcessValue;
    proportional = kP * error;
    integralError += error * dt;
    integral = kI * integralError;
    derivative = kD * (error - previousError) / dt;
    float output = proportional + integral + derivative;
    previousError = error;
    return output;
}
