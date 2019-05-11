/*
 * filter.h
 *
 *  Created on: 11 maj 2019
 *      Author: Marcin
 */

#ifndef SRC_FILTER_H_
#define SRC_FILTER_H_

class EMA
{
public:
    EMA(float filterStrength);
    float process(float input);
    void setStrength(float value) { strength = value; }
private:
    float strength;
    float filteredValue;
};

#endif /* SRC_FILTER_H_ */
