#include "gpioInput.h"
#include "a291unitDefinitions.h"
#include "pinIOControler.h"
#include "slog.h"

#include <algorithm>
#include <iostream>
#include <fstream>

a291unit::GpioInput::GpioInput(int instanceNumber) :
Configurabel(instanceNumber, CFG_PIN_IO_CTRL_OBJ_NAME, CFG_GPIO_INPUT_OBJ_NAME, GPIO_INPUT_NUMBER_OF_PIN_USED, GPIO_INPUT_NUMBER_OF_VALUES, 0) {
	mAliasName = CFG_GPIO_INPUT_USR_NAME;
    mAliasName += "-" + std::to_string(mInstanceNumber);
	
	mNameList[0] = "input";
	mMqttReportType = rt_value;
}

a291unit::ActionResult a291unit::GpioInput::setConfiguration(std::string name, std::string value) {
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

a291unit::ActionResult a291unit::GpioInput::listConfiguration(std::ofstream& cfg_out) {
    cfg_out << "\n# - - " << CFG_GPIO_INPUT_USR_NAME << " " << mAliasName << " - -\n";
    cfg_out << '[' << CFG_GPIO_INPUT_USR_NAME << "]\n";
    cfg_out << CFG_CONFIGURABEL_ALIAS_USR_NAME << " = " << mAliasName << '\n';
    cfg_out << CFG_GPIO_PIN_USR_NAME << " = " << getPinConfiguration() << '\n';
    cfg_out << CFG_GPIO_INPUT_USR_NAME << " = " << mNameList[0] << '\n';
	
	return ar_success;
}

a291unit::ActionResult a291unit::GpioInput::onCheckConfiguration() {
	ActionResult ar = ar_success;
	
	if(!GpioManager::isGpioPin(mGpioPinList[0])) {
		SLOG_ERROR(mAliasName, " ", CFG_GPIO_PIN_USR_NAME, " = ", getPinConfiguration(), " is invalid");
		ar = ar_configurationError;
	}
	
	mCurrentStatus = (ar == ar_success) ? cs_ready : cs_disabled;

	return ar;
}

a291unit::ActionResult a291unit::GpioInput::onInitialize() {
	if(mCurrentStatus == cs_ready) {
		mCurrentStatus = cs_idle;
	}

	return ar_success;
}

bool a291unit::GpioInput::updateValues(const float* sensorValues, const int len, a291unit::ActionResult readResult) {
	if(sensorValues[0] != mValueList[0]) {
		mMqttReportStatus = rs_reportPending;
	}
	
	mValueList[0] = sensorValues[0];
	
	return true;
}
