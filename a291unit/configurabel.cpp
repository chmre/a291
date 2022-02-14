#include "configurabel.h"
#include "slog.h"
#include "gpioManager.h"

#include <vector>
#include <mosquitto.h>

a291unit::Configurabel::Configurabel(const int instanceNumber, const std::string relatedControler, const std::string configurableType, uint8_t numberOfGpioPins, const uint8_t valueCount, const uint8_t settingsCount) :
mControlerType(relatedControler), mConfigurableType(configurableType), mGpioPinCount(numberOfGpioPins), mValueCount(valueCount), mMqttSubscriptionCount(settingsCount), mInstanceNumber(instanceNumber) {

	// initialize object
	mAliasName = mConfigurableType + "-" + std::to_string(mInstanceNumber);
	
	// initialize GPIO pin 
	if(mGpioPinCount == 0) {
		mGpioPinList = NULL;
	}
	else {
		mGpioPinList = new uint8_t[mGpioPinCount];
		for(int i=0; i<mGpioPinCount; i++) mGpioPinList[i] = CFG_INVALID_PIN_ID;
	}
	
	// initialize last action status
	mCurrentStatus = cs_configured;
	mLastReadResult = ar_success;
	mLastReadTimestamp = 0;
	
	// initilaize sensor values
	mValueTimestamp = 0;
	if(mValueCount == 0) {
		// no gpio values
		mValueList = NULL;
		mCorrectionList = NULL;
		mNameList = NULL;
		mUnitList = NULL;
	}
	else {
		mValueList = new float[mValueCount];
		mCorrectionList = new float[mValueCount];
		mNameList = new std::string[mValueCount];
		mUnitList = new std::string[mValueCount];

		for(int i=0; i<mValueCount; i++){
			mValueList[i] = 0;
			mCorrectionList[i] = 0;
			mNameList[i] = "value-" + std::to_string(i);
			mUnitList[i] = "";
		}

	}
	
	mMqttReportType = rt_json;
	
	// initialize mqtt subscription values
	if(mMqttSubscriptionCount == 0) {
		// no subscriptions
        mSubscriptionValueList = NULL;
        mSubscriptionNameList = NULL;
	}
	else {
		// initialize subscriptions
		mSubscriptionValueList = new float[mMqttSubscriptionCount];
		mSubscriptionNameList = new std::string[mMqttSubscriptionCount];
		
		for(int i=0; i<mMqttSubscriptionCount; i++){
			mSubscriptionValueList[i] = 0;
			mSubscriptionNameList[i] = "set";
		}
		
	}
	
}


std::string a291unit::Configurabel::StatusToString(CofigurabelStatus status) {
    std::string txt;
    switch(status) {
        case cs_configured: txt = "configured"; break;
        case cs_ready: txt = "ready"; break;
        case cs_idle: txt = "idle"; break;
        case cs_sampling: txt = "sampling"; break;
        case cs_disabled: txt = "disable"; break;
        default: txt = "unknown status " + std::to_string(status); break;
    }
    
    return txt;	
}

std::string a291unit::Configurabel::ActionResultToString(a291unit::ActionResult result) {
    std::string txt;
    switch(result) {
        case ar_configurationError: txt = "configuration error"; break;
        case ar_unrecoverable: txt = "permanent error"; break;
        case ar_success : txt = "success"; break;
        case ar_responseTimeout: txt = "response timeout"; break;
        case ar_signalTimeout: txt = "signal timeout"; break;
        case ar_crcError: txt = "CRC error"; break;
        case ar_dataReadError: txt = "data read error"; break;
        case ar_signalError: txt = "signal error"; break;
        default: txt = "unknown result " + std::to_string(result); break;
    }
    
    return txt;
}

std::string a291unit::Configurabel::getPinConfiguration() {
    std::string txt = "";
	
	for(int i=0; i< mGpioPinCount; i++) {
		if(i>0) { txt.append(","); }
		int conectionPin = a291unit::GpioManager::getConnectionPinNumber(mGpioPinList[i]);
		txt.append(std::to_string(conectionPin));
	}
	
	return txt;
	
}

const uint8_t a291unit::Configurabel::getConnectionPin(int index) {
	uint pin = CFG_INVALID_PIN_ID;
	
	if((mGpioPinList != NULL)
	&& (index >= 0) 
	&& (index < mGpioPinCount) ) {
		pin = a291unit::GpioManager::getGpioNumber(mGpioPinList[index]);
	}
	
	return pin;
}

const uint8_t a291unit::Configurabel::getGpioPin(int index) {
	uint pin = CFG_INVALID_PIN_ID;
	
	if((mGpioPinList != NULL)
	&& (index >= 0) 
	&& (index < mGpioPinCount) ) {
		pin = mGpioPinList[index];
	}
	
	return pin;
}

bool a291unit::Configurabel::updateValues(const float* sensorValues, const int len, a291unit::ActionResult readResult) {
	bool isOk = true;
	
	if(readResult == ar_success) {
		if(len != mValueCount) {
			// invalid number of values
			isOk = false;
		}
		else {
			mLastReadTimestamp = time(nullptr);
			mValueTimestamp = mLastReadTimestamp;
			mLastReadResult = ar_success;
			for(int i=0; i<mValueCount; i++) {
				mValueList[i] = sensorValues[i];
			}
			setMqttReportPending();
		}
	}
	else {
		mLastReadTimestamp = time(nullptr);
		mLastReadResult = readResult;
		setMqttReportPending();
	}
	
	mCurrentStatus = cs_sampling;
	return isOk;
}

bool a291unit::Configurabel::setPinConfiguration(std::string pinConfiguration) {
	bool accepet = true;
	
	std::string buff{""};
	std::vector<std::string> v;
	const char c = ',';
	
	for(auto n:pinConfiguration)
	{
		if(n != c) buff+=n; else
		if(n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);	

	int i = 0;
	for(std::string n : v) {
		if(i < mGpioPinCount) {
			try {
				int conectionPin = std::stoi(n);
				int gpioPin = a291unit::GpioManager::getGpioNumber(conectionPin);
				mGpioPinList[i] = gpioPin;
			}
			catch(...)  {
				mGpioPinList[i] = CFG_INVALID_PIN_ID;
				accepet = false;
			}
			
		}

		i++;
		
	}
	
	if(i != mGpioPinCount) {
		// invalid pin configuration
		SLOG_ERROR("Invalid number of pins ", mConfigurableType, " excpect ", std::to_string(mGpioPinCount), " got ", pinConfiguration);
        accepet = false;
	} 
		
	return accepet;
}

const std::string a291unit::Configurabel::getSubscription(std::string topic, int index) {
	std::string subscription = topic + "/" + mAliasName + "/" + mSubscriptionNameList[index];
	return subscription;
}

bool a291unit::Configurabel::hasGpioPin(int gpioPinNumber) {
	bool hasPin = false;
	for(int i=0; i<mGpioPinCount; i++) {
		if(mGpioPinList[i] == gpioPinNumber) { hasPin = true; }
	}
	
	return hasPin;
}

bool a291unit::Configurabel::matchControl(std::string messageAlias) {
	bool match = false;
	
	for(int i=0; i<mMqttSubscriptionCount; i++) {
		std::string s = mAliasName + "/" + mSubscriptionNameList[i];
		mosquitto_topic_matches_sub(messageAlias.c_str(), s.c_str(), &match);
		if(match) {break;}
	}
	
	return match;
}
