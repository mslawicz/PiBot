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
    int error = disconnect();
    switch(error)
    {
    case MOSQ_ERR_SUCCESS:
        Logger::getInstance().logEvent(INFO, "MQTT: disconnection request");
        break;
    case MOSQ_ERR_NO_CONN:
        Logger::getInstance().logEvent(WARNING, "MQTT: disconnection request without connection to a broker");
        break;
    default:
        Logger::getInstance().logEvent(ERROR, "MQTT: disconnection request error");
        break;
    }

    // end the connection thread
    Logger::getInstance().logEvent(INFO, "MQTT: loop stop request");
    error = loop_stop();
    switch(error)
    {
    case MOSQ_ERR_SUCCESS:
        Logger::getInstance().logEvent(INFO, "MQTT: loop stopped");
        break;
    case MOSQ_ERR_NO_CONN:
        Logger::getInstance().logEvent(WARNING, "MQTT: loop thread was not found");
        break;
    default:
        Logger::getInstance().logEvent(ERROR, "MQTT: loop stop request error");
        break;
    }

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
        subscribe(nullptr, "PiBot");
    }
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
    Logger::getInstance().logEvent(INFO, "MQTT: subscribed with id=", mid);
}

/*
 * callback after publishing a message
 */
void MQTT::on_publish(int mid)
{
    Logger::getInstance().logEvent(INFO, "MQTT: published message id=", mid);
}

/*
 * handles incoming MQTT message
 */
void MQTT::on_message(const struct mosquitto_message *message)
{
    Logger::getInstance().logEvent(INFO, "MQTT: received message id=", message->mid, ": ", reinterpret_cast<char*>(message->payload));
}

/*
 * subscribes to a topic
 */
void MQTT::subscribe(int *mid, std::string topic)
{
    int error = mosqpp::mosquittopp::subscribe(mid, topic.c_str());
    switch(error)
    {
    case MOSQ_ERR_SUCCESS:
        Logger::getInstance().logEvent(INFO, "MQTT: subscribed to topic '", topic.c_str(), "'");
        break;
    case MOSQ_ERR_NO_CONN:
        Logger::getInstance().logEvent(ERROR, "MQTT: subscription request without connection");
        break;
    default:
        Logger::getInstance().logEvent(ERROR, "MQTT: subscription error");
        break;
    }
}

/*
 * publish a message
 */
void MQTT::publish(int *mid, std::string topic, int payloadlen, const void* payload)
{
    mosqpp::mosquittopp::publish(mid, topic.c_str(), payloadlen, payload);
}
