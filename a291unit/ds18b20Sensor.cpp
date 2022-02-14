#include "ds18b20Sensor.h"
#include "wireBusControler.h"
#include "slog.h"

#include <algorithm>
#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>


a291unit::Ds18b20Sensor::Ds18b20Sensor(int instanceNumber) :
Configurabel(instanceNumber, CFG_WBUS_CTRL_OBJ_NAME, CFG_SENSOR_DS18B20_OBJ_NAME, DS18B20_NUMBER_OF_PIN_USED, DS18B20_NUMBER_OF_SENSOR_VALUES, 0) {
    mNameList[0] = "temperatur";
    mUnitList[0] = "°C";
	mValueList[0] = 0;
	mCorrectionList[0] = 0;
    mRegistrationNumber = NULL;
	
	mCurrentStatus = cs_configured;
	mLastReadResult = ar_responseTimeout;
}

a291unit::ActionResult a291unit::Ds18b20Sensor::setConfiguration(std::string name, std::string value) {
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	ActionResult ar = ar_configurationError;
//    bool accepet = false;
    
    if(0 == name.compare(CFG_CONFIGURABEL_ALIAS_OBJ_NAME)) {
        mAliasName = value;
        ar = ar_success;
    }

    if(0 == name.compare(CFG_VALUE_TEMPERATURE_OBJ_NAME)) {
        mNameList[0] = value;
        ar = ar_success;
    }

    if(0 == name.compare(CFG_CORRECTION_TEMPERATUR_OBJ_NAME)) {
        try {
            float val = std::stof(value);
			mCorrectionList[0] = val;
			ar = ar_success;
        }
        catch(...)  {
            #ifdef DEBUG
            std::cout << CFG_CORRECTION_TEMPERATUR_USR_NAME << ": invalid value " << value << '\n';
            #endif
			ar = ar_success;
        }
        
    }
    
    if(0 == name.compare(CFG_GPIO_PIN_OBJ_NAME)) {
		if(setPinConfiguration(value)) ar = ar_success;
    }
    
    
    if(0 == name.compare(CFG_DS18B20_REG_ID_OBJ_NAME)) {
        uint8_t regId[RNBR_BYT_SIZE];
        int index = RNBR_BYT_SIZE - 1;
        std::string byte;
        std::istringstream byteStream(value);
        while (std::getline(byteStream, byte, ':')) {
            uint8_t rv = stoi(byte, 0, 16);
            regId[index] = rv;
            index--;
        }
        
        mRegistrationNumber = new RegistrationNumber(regId);
        ar = ar_success;
    }

    if(ar != ar_success) {
		SLOG_WARNING(CFG_SENSOR_DS18B20_USR_NAME, ": invalid configuration element -> ", name);
	}
   
    return ar; 
}

a291unit::ActionResult a291unit::Ds18b20Sensor::listConfiguration(std::ofstream &cfg_out) {
    cfg_out << "\n# - - " << mConfigurableType << " " << mAliasName << " - -\n";
    cfg_out << '[' << CFG_SENSOR_DS18B20_USR_NAME << "]\n";
    cfg_out << CFG_CONFIGURABEL_ALIAS_USR_NAME << " = " << mAliasName << '\n';
    cfg_out << CFG_GPIO_PIN_USR_NAME << " = " << getPinConfiguration() << '\n';
    cfg_out << CFG_VALUE_TEMPERATURE_USR_NAME << " = " << mNameList[0] << '\n';
    cfg_out << CFG_CORRECTION_TEMPERATUR_USR_NAME << " = " << mCorrectionList[0] << '\n';
    
    std::string regNbr = (mRegistrationNumber == NULL) ? "#" : "";
    regNbr += CFG_DS18B20_REG_ID_USR_NAME;
    regNbr += " = ";
    if(mRegistrationNumber == NULL) {
        regNbr += "xx:xx:xx:xx:xx:xx:xx:xx";
    }
    else {
        regNbr += mRegistrationNumber->getRegistrationString();
    }
    cfg_out << regNbr << '\n';

    return ar_success;
    
}

bool a291unit::Ds18b20Sensor::isRegistrationNumber(RegistrationNumber* registrationNumber) {
    if(mRegistrationNumber == NULL) {
        return false;
    }
    else {
        return mRegistrationNumber->compareMatch(registrationNumber);
    }
}

a291unit::ActionResult a291unit::Ds18b20Sensor::onCheckConfiguration() {
	ActionResult ar = ar_success;
	
	if(getGpioPin(0) == CFG_INVALID_PIN_ID) {
		SLOG_ERROR(mAliasName, " configuration: >", CFG_GPIO_PIN_USR_NAME, "< is not defined");
		ar = ar_configurationError;
	}
	
	if(mRegistrationNumber == NULL) {
		SLOG_ERROR(mAliasName, " configuration: >", CFG_DS18B20_REG_ID_USR_NAME, "< is not defined");
		ar = ar_configurationError;
	}

	mCurrentStatus = (ar == ar_success) ? cs_ready : cs_disabled;
	
	return ar;;
}

const a291unit::CofigurabelStatus& a291unit::Ds18b20Sensor::getCurrentStatus() {
	time_t TimeNow = time(nullptr);
	double elapsed_secs = double(TimeNow - mLastReadTimestamp);
	
	if(mCurrentStatus == cs_sampling && elapsed_secs >= 10) {
		mCurrentStatus = cs_idle;
	}
	
	return mCurrentStatus;
}
