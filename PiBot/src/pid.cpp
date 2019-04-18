/*
 * pid.cpp
 *
 *  Created on: 11 lut 2019
 *      Author: Marcin
 */

#include "pid.h"
#include <pigpio.h>

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
    lastTick = 0;
}

float PID::calculate(float setPointValue, float measuredProcessValue)
{
    const float MsInS = 1e-6;
    float error = setPointValue - measuredProcessValue;
    uint32_t currentTick = gpioTick();
    float deltaT = ((currentTick == lastTick) ? 1 : (currentTick - lastTick)) * MsInS;
    proportional = kP * error;
    integralError += error * deltaT;
    integral = kI * integralError;
    derivative = kD * (error - previousError) / deltaT;
    float output = proportional + integral + derivative;
    previousError = error;
    lastTick = currentTick;
    return output;
}
