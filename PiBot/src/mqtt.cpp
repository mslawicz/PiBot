/*
 * mqtt.cpp
 *
 *  Created on: 2 mar 2019
 *      Author: Marcin
 */

#include "mqtt.h"

MQTT::MQTT(std::string id)
    : mosquittopp(id.c_str())
{
    mosqpp::lib_init();

}

MQTT::~MQTT()
{
    mosqpp::lib_cleanup();
}

