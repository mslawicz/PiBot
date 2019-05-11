/*
 * filter.cpp
 *
 *  Created on: 11 maj 2019
 *      Author: Marcin
 */

#include "filter.h"

EMA::EMA(float filterStrength)
    : strength(filterStrength)
{
    filteredValue = 0.0f;
}

float EMA::process(float input)
{
    filteredValue += strength * (input - filteredValue);
    return filteredValue;
}
