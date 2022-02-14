#include "mqttClient.h"
#include "slog.h"
#include "configurabel.h"
#include "mqttOutput.h"
#include "unitManager.h"

#include <algorithm>
#include <iostream>
#include <fstream>


void a291unit::MqttConnectCallback(struct mosquitto* mosq, void* obj, int result) {
	a291unit::MqttClient* client = (a291unit::MqttClient*)obj;

/* int result values
 * 0 - success
 * 1 - connection refused (unacceptable protocol version)
 * 2 - connection refused (identifier rejected)
 * 3 - connection refused (broker unavailable)
 * 4-255 - reserved for future use
*/
	std::string txt;
	switch(result) {
		case 0:
		txt = ") success";
		break;
		
		case 1:
		txt = ") connection refused (unacceptable protocol version)";
		break;
		
		case 2:
		txt = ") connection refused (identifier rejected)";
		break;
		
		case 3:
		txt = ") connection refused (broker unavailable)";
		break;
		
		default:
		txt = ") reserved for future use";
		break;
	}

	SLOG_INFO(client->getAliasName(), " MQTT connect callback result (", result, txt);

	client->connectionResponse(mosq, result);

}

/**
 * @brief	MQTT message receive callback
 * @param mosq
 * @param obj
 * @param message
 */
void a291unit::MqttMessageCallback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message) {
	a291unit::MqttClient* client = (a291unit::MqttClient*)obj;

	#ifdef DEBUG
	SLOG_DEBUG(client->getAliasName(), " MQTT message received, topic: ", message->topic, " paylod len: ", (int)message->payloadlen, " value: ", (char*) message->payload);
	#endif

	client->updateControl(mosq, message);
}

a291unit::MqttClient::MqttClient() :
Controler(CFG_MQTT_CLIENT_OBJ_NAME, CFG_MQTT_MAX_BROKER_CONNECTIONS) {

		char clientId[MQTT_CLIENT_ID_LEN];
		char hostname[50];
		gethostname(hostname, sizeof(hostname));
		snprintf(clientId, MQTT_CLIENT_ID_LEN-1, "%s_%d", hostname, getpid());
		mAliasName = clientId;
		SLOG_INFO("MQTT Client ID: ", mAliasName);
}

bool a291unit::MqttClient::addConfigurabel(Configurabel* configurable) {
	return (mContolerSensorList.append(configurable) == NULL) ? false : true;
}

a291unit::ActionResult a291unit::MqttClient::onInitialize() {
	
	MqttBroker* server = (MqttBroker*) getFirstConfigurabel();
	while(server != NULL) {
		struct mosquitto*  mosq = mosquitto_new(mAliasName.c_str(), true, this);
		if(NULL == mosq)
		{
			SLOG_ERROR("Error: MQTT Client can not be created");
			server->setCurrentStatus(cs_disabled);
		}
		else
		{
			mosquitto_connect_callback_set(mosq, MqttConnectCallback);
			mosquitto_message_callback_set(mosq, MqttMessageCallback);
			server->setMosquittoControle(mosq);
			server->setCurrentStatus(cs_idle);
			mosquitto_loop_start(mosq);
			connectBroker(server);
		}
		
		server = (MqttBroker*) getNextConfigurabel(server);
	}
	
	return ar_success;
}

void a291unit::MqttClient::connectBroker(MqttBroker* server) {
	struct mosquitto* mosq = server->getMosquittoControle();
	mosquitto_username_pw_set(mosq, "a291", "a291");
	server->setCurrentStatus(cs_idle);

    int rtc = mosquitto_connect(mosq, server->getHostAddress().c_str(), server->getPort(), server->getKeepAlive());
    if(rtc == MOSQ_ERR_SUCCESS)
    {
		SLOG_INFO(getAliasName(), " request connected to MQTT broker ", server->getHostAddress());
    }
    else
    {
        int e = errno;
        if(e == 113)
        {
            SLOG_WARNING("Broker connect error: ", (int)e, " (no response from server)");
			server->setCurrentStatus(cs_ready);
        }
        else
        {
            SLOG_ERROR("Broker connect error: ", (int)e);
			server->setCurrentStatus(cs_disabled);
        }
    }    
}

void a291unit::MqttClient::disconnectBroker(MqttBroker* server) {
	struct mosquitto* mosq = server->getMosquittoControle();
	mosquitto_loop_stop(mosq, true);
	mosquitto_disconnect(mosq);
   	if(mosq != NULL) { mosquitto_destroy(mosq); }

}

a291unit::ActionResult a291unit::MqttClient::onTerminate() {
	MqttBroker* server = (MqttBroker*) getFirstConfigurabel();
	while(server != NULL) {
		disconnectBroker(server);
		mosquitto_destroy(server->getMosquittoControle());
		SLOG_INFO("Disconnected from MQTT broker ", server->getHostAddress());
		server = (MqttBroker*) getNextConfigurabel(server);
	}
	
	return ar_success;
	
}

a291unit::ActionResult a291unit::MqttClient::onReport() {
	MqttBroker* server = (MqttBroker*) getFirstConfigurabel();
	while(server != NULL) {
		CofigurabelStatus cs = server->getCurrentStatus();
		switch(cs) {
			
			case cs_ready:
			connectBroker(server);
			if(server->getCurrentStatus() == cs_idle) {
				reportConfigurables(server);
			}
			else {
				SLOG_INFO("MQTT broker ", server->getHostAddress(), " status is ", Configurabel::StatusToString(cs));
			}
			break;
			
			case cs_idle:
			reportConfigurables(server);
			break;

			default:
			SLOG_INFO("MQTT broker ", server->getHostAddress(), " is ", Configurabel::StatusToString(cs));
			break;
			
		}


		server = (MqttBroker*) getNextConfigurabel(server);
	}
	
	return ar_success;
}

void a291unit::MqttClient::reportConfigurables(MqttBroker* server) {
	struct mosquitto* mosq = server->getMosquittoControle();
	
	Configurabel* c = a291unit::UnitManager::getFirstConfigurable();
	while(c != NULL) {
		if(rs_reportPending == c->getMqttReportStatus()) {
			if(c->getMqttReportType() == rt_json) {
				publishJson2(mosq, server->getTopic(), c);
			}
			else {
				publishValue(mosq, server->getTopic(), c);
			}
			c->setMqttReportDone();
		}
		
		c = a291unit::UnitManager::getNextConfigurable(c);
	}
	
}

void a291unit::MqttClient::updateControl(struct mosquitto* mosq, const struct mosquitto_message* message) {
	std::string value = (char*)(message->payload);

	// find the broker from where the message was received
	MqttBroker* b = (MqttBroker*)getFirstConfigurabel();
	while(b != NULL) {
		if(b->getMosquittoControle() == mosq) {
				break;
		}
		
		b = (MqttBroker*)getNextConfigurabel(b);
	}
	
	// process the message 
	if(b == NULL) {
		SLOG_ERROR("Failed to find broker");
	}
	else {
		// found broker
		std::string msgTopic = message->topic;
		b->removeServerTopic(msgTopic);
		
		#ifdef DEBUG
		SLOG_DEBUG("Processing message from broker: ", b->getHostAddress());
		#endif

		Configurabel* c = a291unit::UnitManager::getFirstConfigurable();
		while(c != NULL) {
			if(c->matchControl(msgTopic)) {
				try {
					float f = std::stof(value);
					#ifdef DEBUG
					SLOG_DEBUG("set MQTT value ", message->topic, " ", f);
					#endif
					c->setMqttSubscriptionValue(msgTopic, f);
				}
				catch(...) {
					SLOG_ERROR("MqttClient UpdateControl() invalid value ", value);
				}
				break;
			}

			c = a291unit::UnitManager::getNextConfigurable(c);
		}
	}
}




/* int result values
 * 0 - success
 * 1 - connection refused (unacceptable protocol version)
 * 2 - connection refused (identifier rejected)
 * 3 - connection refused (broker unavailable)
 * 4-255 - reserved for future use
*/
void a291unit::MqttClient::connectionResponse(struct mosquitto* mosq, int connectionStatus) {
	// find the broker from where the message was received
	MqttBroker* b = (MqttBroker*)getFirstConfigurabel();
	while(b != NULL) {
		if(b->getMosquittoControle() == mosq) {
				break;
		}
		
		b = (MqttBroker*)getNextConfigurabel(b);
	}

	
	if(b != NULL) {
		int okCount = 0;
		int failCount = 0;
		int totalCount = 0;
		
		// search for items that need subscription
		Configurabel* c = a291unit::UnitManager::getFirstConfigurable();
		while(c != NULL) {
			std::string topic = b->getTopic();
			int subscriptionCount = c->getMqttSubscriptionCount();
		    totalCount += subscriptionCount;
			
			if(subscriptionCount > 0) {
				#ifdef DEBUG
				SLOG_INFO(getAliasName(), ": Subscribing ", c->getAliasName(), " subscriptions: ", subscriptionCount);
				#endif
			
				for(int i = 0; i < subscriptionCount; i++) {
					
					std::string subscr = c->getSubscription(topic, i);

					if(MOSQ_ERR_SUCCESS == mosquitto_subscribe(mosq, NULL, subscr.c_str(), 0)) {
						SLOG_INFO(getAliasName(), " success: ", c->getAliasName(), " subscribing to: ", subscr);
						okCount++;
					}
					else {
						SLOG_ERROR(getAliasName(), " faile: ", c->getAliasName(), " subscribing to: ", subscr);
						failCount++;
					}
				}
			}
			

			c = a291unit::UnitManager::getNextConfigurable(c);
		}
		
		SLOG_INFO(getAliasName(), ": Total subsciptions to ", b->getHostAddress(), ": ", totalCount);
		
	}
	else {
		SLOG_ERROR("Connect callback falled to find broker");
	}
}

void a291unit::MqttClient::publishJson(struct mosquitto* mosq, std::string topic, Configurabel* configurable) {
	std::string sensorTopic = topic + "/" + configurable->getAliasName();
	
	const int valueCount = configurable->getValueCount();
	std::string msg = "";
	
	ActionResult ar = configurable->getLastReadResult();
	msg = "{ \"status\" : \"";
	msg += Configurabel::ActionResultToString(ar);
	msg += "\"";
	
	for(int i=0; i<configurable->getValueCount(); i++) {
		
		// add value
		msg += ", \"";
		msg +=configurable->getValueName(i);
		msg += "\" : ";

		const float value = configurable->getValue(i);
		char vChar[10];
		sprintf(vChar, "%.1f", value);
		msg +=  vChar;

		// add unit
		msg += ", \"";
		msg +=configurable->getValueName(i) + "_unit";
		msg += "\" : \"";
		msg += configurable->getValueUnit(i);
		msg += "\"";
	}
	msg += " }";

    #ifdef DEBUG
	SLOG_DEBUG("MQTT msg: ", sensorTopic, " = ", msg);
    #endif

    mosquitto_publish(mosq, NULL, sensorTopic.c_str(), msg.length(), msg.c_str(), 0, false);

}

void a291unit::MqttClient::publishJson2(struct mosquitto* mosq, std::string topic, Configurabel* configurable) {
	std::string sensorTopic = topic + "/" + configurable->getAliasName();
	
	const int valueCount = configurable->getValueCount();
	std::string msg = "{";
	
	msg += "\"sensor\": \"";
	msg += configurable->getConfigurabelType();
	msg += "\"";
	
	msg += ", \"status\": \"";
	ActionResult ar = configurable->getLastReadResult();
	msg += Configurabel::ActionResultToString(ar);
	msg += "\"";
	
	for(int i=0; i<configurable->getValueCount(); i++) {

		msg += ", \"";
		msg +=configurable->getValueName(i);
		msg += "\": {";
		
		// add value
		msg += "\"value\": ";

		const float value = configurable->getValue(i);
		char vChar[10];
		sprintf(vChar, " %.1f", value);
		msg +=  vChar;

		// add unit
		msg += ", \"unit\": \"";
		msg += configurable->getValueUnit(i);
		msg += "\"";
		msg += " }";
	}

	msg += " }";

    #ifdef DEBUG
	SLOG_DEBUG("MQTT msg: ", sensorTopic, " = ", msg);
    #endif

    mosquitto_publish(mosq, NULL, sensorTopic.c_str(), msg.length(), msg.c_str(), 0, false);

}

void a291unit::MqttClient::publishValue(struct mosquitto* mosq, std::string topic, Configurabel* configurable) {
	
	for(int i=0; i<configurable->getValueCount(); i++) {
		std::string sensorTopic = topic + "/" + configurable->getAliasName();

		std::string msg = "";
		
		const float value = configurable->getValue(i);
		char vChar[10];
		sprintf(vChar, "%.1f", value);
		msg +=  vChar;
		
		#ifdef DEBUG
		SLOG_DEBUG("MQTT msg: ", sensorTopic, " = ", msg);
		#endif

		mosquitto_publish(mosq, NULL, sensorTopic.c_str(), msg.length(), msg.c_str(), 0, false);
	}	
	
}
