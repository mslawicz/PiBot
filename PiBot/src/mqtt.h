/*
 * mqtt.h
 *
 *  Created on: 2 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_MQTT_H_
#define SRC_MQTT_H_

#include <mosquittopp.h>
#include <iostream>

class MQTT : public mosqpp::mosquittopp
{
public:
    MQTT(std::string id);
    ~MQTT();
};

#endif /* SRC_MQTT_H_ */
