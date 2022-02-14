#ifndef MQTTSERVER_H
#define MQTTSERVER_H

#include "configurabel.h"

#include <mosquitto.h>

#define CFG_MQTT_BROKER_USR_NAME "MQTT Broker"
#define CFG_MQTT_BROKER_OBJ_NAME "MQTTBROKER"

#define MQTT_DEFAULT_PORT 1883
#define MQTT_DEFAULT_KEEP_ALIVE 60

#define MQTT_CLIENT_ID_LEN 128

#define MQTT_BROKER_PARAM "BROKER"
#define MQTT_TOPIC_PARAM "TOPIC"
#define MQTT_PORT_PARAM "PORT"
#define MQTT_KEEP_ALIVE_PARAM "KEEPALIVE"

#define MQTT_REPORT_INTERVAL_30_SECOND 30

namespace a291unit {

	class MqttBroker : public Configurabel {
	
	private:
		// attributes
		std::string mHostAddress;
		std::string mTopic;
		int mPort;
		int mKeepAlive;
		struct mosquitto* mMosquittoControle;
		
	public:
		MqttBroker(int instanceNumber);
		
		// getter to access structure mosquitto
		void setMosquittoControle(mosquitto* newMosquittoControle) { this->mMosquittoControle = newMosquittoControle; }
		mosquitto* getMosquittoControle() { return mMosquittoControle; }

		// getter for MQTT broker parameters
		const std::string& getHostAddress() const { return mHostAddress; }
		int getKeepAlive() const { return mKeepAlive; }
		int getPort() const { return mPort; }
		const std::string& getTopic() const { return mTopic; }
		void removeServerTopic(std::string &messageTopic);

		// overwrite status report
		virtual void setCurrentStatus(const CofigurabelStatus& newStatus);

		// implement pure virtual functions from configurable class
		virtual ActionResult setConfiguration(std::string name, std::string value);
		virtual ActionResult listConfiguration(std::ofstream &cfg_out);
		virtual ActionResult onCheckConfiguration();
		virtual ActionResult onInitialize();

	};
}

#endif
