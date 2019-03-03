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
    port = 1883;
    mosqpp::lib_init();
    int libVersion = mosqpp::lib_version(nullptr, nullptr, nullptr);
    Logger::getInstance().logEvent(INFO, "Mosquitto library version ", libVersion / 1000000, ".", (libVersion / 1000) % 1000, ".", libVersion % 1000, " initialized");
    // non-blocking connection to broker
    int error = connect_async(address.c_str(), port);
    if(error)
    {
        Logger::getInstance().logEvent(ERROR, "MQTT: connection request to ", address.c_str(), ":", port, " returned error code=", error);
    }
    else
    {
        Logger::getInstance().logEvent(INFO, "MQTT: request connection to ", address.c_str(), ":", port);
        // connection request OK, start thread to process network traffic
        error = loop_start();
        if(error)
        {
            Logger::getInstance().logEvent(ERROR, "MQTT: loop not started (error code=", error, ")");
        }
        else
        {
            Logger::getInstance().logEvent(INFO, "MQTT: loop started");
        }
    }
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
        Logger::getInstance().logEvent(ERROR, "MQTT: failed to connect to ", address.c_str(), ":", port, " (code=", rc, ")");
    }
    else
    {
        Logger::getInstance().logEvent(INFO, "MQTT: connected to ", address.c_str(), ":", port);
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
