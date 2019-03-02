/*
 * mqtt.cpp
 *
 *  Created on: 2 mar 2019
 *      Author: Marcin
 */

#include "mqtt.h"
#include "logger.h"

MQTT::MQTT(std::string id, std::string hostAddress)
    : mosquittopp(id.c_str())
    , address(hostAddress)
{
    mosqpp::lib_init();
    int libVersion = mosqpp::lib_version(nullptr, nullptr, nullptr);
    Logger::getInstance().logEvent(INFO, "Mosquitto library version ", libVersion / 1000000, ".", (libVersion / 1000) % 1000, ".", libVersion % 1000, " initialized");
    // non-blocking connection to broker
    connect_async(address.c_str());
    // start thread managing connection
    loop_start();
}

MQTT::~MQTT()
{
    disconnect();
    // end the connection thread
    loop_stop();
    // cleanup mosquitto
    mosqpp::lib_cleanup();
}

/*
 * callback executed on connection
 */
void MQTT::on_connect(int rc)
{
    if(rc)
    {
        Logger::getInstance().logEvent(ERROR, "MQTT: failed to connect to ", address.c_str(), ":1883 (code=", rc, ")");
    }
    else
    {
        Logger::getInstance().logEvent(INFO, "MQTT: connected to ", address.c_str(), ":1883");
    }
    //TODO: made subscriptions here
}

/*
 * callback executed on disconnection
 */
void MQTT::on_disconnect(int rc)
{
    if(rc)
     {
         Logger::getInstance().logEvent(ERROR, "MQTT: unexpected disconnection");
     }
     else
     {
         Logger::getInstance().logEvent(INFO, "MQTT: disconnected upon request");
     }
}

/*
 * callback executed on subscription
 */
void MQTT::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    Logger::getInstance().logEvent(INFO, "MQTT: subscribed");
}
