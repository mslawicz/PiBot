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
    integral = 0.0f;
}

float PID::calculate(float measuredValue, float referenceValue)
{
    float error = measuredValue - referenceValue;
    integral += error;
    float output = kP * error + kI * integral + kD * (error - previousError);
    previousError = error;
    return output;
}
