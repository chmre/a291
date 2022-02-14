#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include "controler.h"
#include "configurationList.h"
#include "mqttBroker.h"

#include <mosquitto.h>

#define CFG_MQTT_CLIENT_USR_NAME "MQTT Client"
#define CFG_MQTT_CLIENT_OBJ_NAME "MQTTCLIENT"
#define CFG_MQTT_MAX_BROKER_CONNECTIONS 2

#define MQTT_CLIENT_ID_LEN 128
#define MQTT_TOPIC_LEN 256
#define MQTT_MESSAGE_LEN 256

namespace a291unit {
	
	static void MqttConnectCallback(struct mosquitto *mosq, void *obj, int result);
	static void MqttMessageCallback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
	
	class MqttClient : public Controler {
		
	
	private:
		void connectBroker(MqttBroker* server);
		void disconnectBroker(MqttBroker* server);
		void reportConfigurables(MqttBroker* server);
		void publishJson(struct mosquitto* mosq, std::string topic, Configurabel* configurable);
		void publishJson2(struct mosquitto* mosq, std::string topic, Configurabel* configurable);
		void publishValue(struct mosquitto* mosq, std::string topic, Configurabel* configurable);
				
	public:
		MqttClient();
		
        // virtual functions to be implemented from controler
        virtual bool addConfigurabel(Configurabel* configurable);
        virtual ActionResult onInitialize();
        virtual ActionResult onReport();
		virtual ActionResult onTerminate();
		
		void updateControl(struct mosquitto* mosq, const struct mosquitto_message* message);
		void connectionResponse(struct mosquitto* mosq, int connectionStatus);
	};
	
}

#endif