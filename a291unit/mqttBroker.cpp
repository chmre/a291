#include "mqttBroker.h"
#include "mqttClient.h"
#include "slog.h"

#include <algorithm>
#include <fstream>
#include <mosquitto.h>

a291unit::MqttBroker::MqttBroker(int instanceNumber) :
Configurabel(instanceNumber, CFG_MQTT_CLIENT_OBJ_NAME, CFG_MQTT_BROKER_OBJ_NAME, 0, 0, 0) {
	mHostAddress = "";
    mPort = MQTT_DEFAULT_PORT;
    mKeepAlive = MQTT_DEFAULT_KEEP_ALIVE;
    

	mMosquittoControle = NULL;
	
	mCurrentStatus = cs_configured;
}


a291unit::ActionResult a291unit::MqttBroker::setConfiguration(std::string name, std::string value) {
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	ActionResult ar = ar_configurationError;
    
    if(0 == name.compare(MQTT_BROKER_PARAM)) {
        mHostAddress = value;
        ar = ar_success;
    }
    
    if(0 == name.compare(MQTT_TOPIC_PARAM)) {
        mTopic = value;
        ar = ar_success;
    }
    
    if(0 == name.compare(MQTT_PORT_PARAM)) {
        try {
            mPort = std::stoi(value);
        ar = ar_success;
        }
        catch(...)  {
            SLOG_WARNING(mAliasName, ": invalid element value: ", MQTT_PORT_PARAM, value);
        }
    }
    
    if(0 == name.compare(MQTT_KEEP_ALIVE_PARAM)) {
        try {
            mKeepAlive = std::stoi(value);
        ar = ar_success;
        }
        catch(...)  {
            SLOG_WARNING(mAliasName, ": invalid element value: ", MQTT_KEEP_ALIVE_PARAM, value);
        }
        
    }
	
    return ar;
	
}

a291unit::ActionResult a291unit::MqttBroker::listConfiguration(std::ofstream& cfg_out) {
    cfg_out << '[' << CFG_MQTT_BROKER_USR_NAME << "]\n";
    cfg_out << MQTT_BROKER_PARAM << " = " << mHostAddress << '\n';
    cfg_out << MQTT_TOPIC_PARAM << " = " << mTopic << '\n';
    cfg_out << MQTT_PORT_PARAM << " = " << mPort << '\n';
    cfg_out << MQTT_KEEP_ALIVE_PARAM << " = " << mKeepAlive << '\n';

	return ar_success;
}

a291unit::ActionResult a291unit::MqttBroker::onCheckConfiguration() {
	ActionResult ar = ar_success;
	
	if(mHostAddress.empty()) {
		ar = ar_configurationError;
		mCurrentStatus = cs_disabled;
		SLOG_ERROR(mAliasName, " has been disabled. No host adress is configured");
	}
	
	return ar;
}

a291unit::ActionResult a291unit::MqttBroker::onInitialize() {
	if(cs_ready == mCurrentStatus) {
		mCurrentStatus = cs_idle;
	}
	
	return (mCurrentStatus == cs_idle) ? ar_success : ar_configurationError;
}

void a291unit::MqttBroker::setCurrentStatus(const CofigurabelStatus& newStatus) {
	mLastReadTimestamp = time(nullptr);
	mCurrentStatus = newStatus;
}

void a291unit::MqttBroker::removeServerTopic(std::string &messageTopic) {
	char **broker_topics;
	int broker_topic_count;
	mosquitto_sub_topic_tokenise(mTopic.c_str(), &broker_topics, &broker_topic_count);
	
	char **msg_topics;
	int msg_topic_count;
	mosquitto_sub_topic_tokenise(messageTopic.c_str(), &msg_topics, &msg_topic_count);
	
	messageTopic.clear();
	for(int i = broker_topic_count; i < msg_topic_count; i++) {
		if(!messageTopic.empty()) { messageTopic.append("/"); }
		messageTopic.append(msg_topics[i]);
	}
	
}
