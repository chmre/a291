#include "mqttOutput.h"
#include "pinIOControler.h"
#include "slog.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <mosquitto.h>


a291unit::MqttOutput::MqttOutput(int instanceNumber) :
Configurabel(instanceNumber, CFG_PIN_IO_CTRL_OBJ_NAME, CFG_MQTT_OUTPUT_OBJ_NAME, GPIO_OUTPUT_NUMBER_OF_PIN_USED, GPIO_OUTPUT_NUMBER_OF_VALUES, GPIO_OUTPUT_NUMBER_OF_CONTROLS) {
	mOutputChangePending = false;
	mSubscriptionNameList[0] = "set";
	mMqttReportType = rt_value;
}

a291unit::ActionResult a291unit::MqttOutput::setConfiguration(std::string name, std::string value) {
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	ActionResult res = ar_configurationError;
    
    if(0 == name.compare(CFG_CONFIGURABEL_ALIAS_OBJ_NAME)) {
        mAliasName = value;
        res = ar_success;
    }

    if(0 == name.compare(CFG_GPIO_VALUE_OBJ_NAME)) {
        mNameList[0] = value;
        res = ar_success;
    }

    if(0 == name.compare(CFG_GPIO_PIN_OBJ_NAME)) {
		uint8_t pin = CFG_INVALID_PIN_ID;
		
		try {
			uint8_t pin = std::stoi(value);
			mGpioPinList[0] = a291unit::GpioManager::getGpioNumber(pin);
			res = ar_success;
		}
		catch(...) {
            SLOG_ERROR(mAliasName, ": invalid pin number ", name, " = ", value);
		}
    }
    
	if(res != ar_success ){
		SLOG_ERROR(mAliasName, ": invalid configuration element ", name, " = ", value);
	}
	
    return res; 
}

a291unit::ActionResult a291unit::MqttOutput::listConfiguration(std::ofstream& cfg_out) {
    cfg_out << "\n# - - " << CFG_MQTT_OUTPUT_USR_NAME << " " << mAliasName << " - -\n";
    cfg_out << '[' << CFG_MQTT_OUTPUT_USR_NAME << "]\n";
    cfg_out << CFG_CONFIGURABEL_ALIAS_USR_NAME << " = " << mAliasName << '\n';
    cfg_out << CFG_GPIO_PIN_USR_NAME << " = " << getPinConfiguration() << '\n';
    cfg_out << CFG_GPIO_VALUE_USR_NAME << " = " << mNameList[0] << '\n';
	
	return ar_success;
}

a291unit::ActionResult a291unit::MqttOutput::onCheckConfiguration() {
	ActionResult ar = ar_success;
	
	if(!GpioManager::isGpioPin(mGpioPinList[0])) {
		SLOG_ERROR(mAliasName, " ", CFG_GPIO_PIN_USR_NAME, " = ", getPinConfiguration(), " is invalid");
		ar = ar_configurationError;
	}
	
	mCurrentStatus = (ar == ar_success) ? cs_ready : cs_disabled;

	return ar;
}

a291unit::ActionResult a291unit::MqttOutput::onInitialize() {
	if(mCurrentStatus == cs_ready) {
		mCurrentStatus = cs_idle;
	}

	return ar_success;
}

void a291unit::MqttOutput::setMqttSubscriptionValue(std::string subscriptionAlias, float toValue) {
	bool match = false;
	
	for(int i=0; i<mMqttSubscriptionCount; i++) {
		std::string s = mAliasName + "/" + mSubscriptionNameList[i];
		mosquitto_topic_matches_sub(subscriptionAlias.c_str(), s.c_str(), &match);
		if(match) {
			mSubscriptionValueList[i] = toValue;
			#ifdef DEBUG
			SLOG_DEBUG("Gpio Output MQTT value ", mSubscriptionNameList[i], " to ", toValue);
			#endif
			break;
		}
	}
		
	if(!match) {
		SLOG_ERROR("Could not find subscription: ", subscriptionAlias);
	}
}

const bool a291unit::MqttOutput::changeOutput() {
	mOutputChangePending = (mValueList[0] == mSubscriptionValueList[0]) ? false : true;
	return mOutputChangePending;
}

a291unit::MqttReportStatus a291unit::MqttOutput::getMqttReportStatus() {
	if(mOutputChangePending) {
		mMqttReportStatus = (mValueList[0] == mSubscriptionValueList[0]) ? rs_reportPending : rs_noReport;
	}
	
	return mMqttReportStatus;
}
