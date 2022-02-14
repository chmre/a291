#include "dhtxxSensor.h"
#include "dhtControler.h"
//#include "configurabel.h"
#include "slog.h"

#include <algorithm>
#include <fstream>

a291unit::DhtxxSensor::DhtxxSensor(uint dhtSensorType, int instanceNumber) : mSensorType(dhtSensorType), 
Configurabel(instanceNumber, CFG_DHT_CTRL_OBJ_NAME, (dhtSensorType == SENSOR_TYPE_DHT11) ? CFG_SENSOR_DHT11_OBJ_NAME : CFG_SENSOR_DHT22_OBJ_NAME, DHT_NUMBER_OF_PIN_USED, DHT_NUMBER_OF_SENSOR_VALUES, 0){

	mNameList[DHT_HUMIDITY_SENSOR_INDEX] = "humidity";
	mNameList[DHT_TEMPERATUR_SENSOR_INDEX] = "temperature";
	mUnitList[DHT_HUMIDITY_SENSOR_INDEX] = "%rH";
	mUnitList[DHT_TEMPERATUR_SENSOR_INDEX] = "°C";
	
}

a291unit::ActionResult a291unit::DhtxxSensor::setConfiguration(std::string name, std::string value) {
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	ActionResult res = ar_configurationError;
    
    if(0 == name.compare(CFG_CONFIGURABEL_ALIAS_OBJ_NAME)) {
        mAliasName = value;
        res = ar_success;
    }

    if(0 == name.compare(CFG_VALUE_HUMIDITY_OBJ_NAME)) {
        mNameList[DHT_HUMIDITY_SENSOR_INDEX] = value;
        res = ar_success;
    }

    if(0 == name.compare(CFG_VALUE_TEMPERATURE_OBJ_NAME)) {
        mNameList[DHT_TEMPERATUR_SENSOR_INDEX] = value;
        res = ar_success;
    }

    if(0 == name.compare(CFG_CORRECTION_HUMIDITY_OBJ_NAME)) {
        try {
            float val = std::stof(value);
            mCorrectionList[DHT_HUMIDITY_SENSOR_INDEX] = val;
			res = ar_success;
        }
        catch(...)  {
            SLOG_ERROR(mAliasName, ": invalid configuration value ", name, " = ", value);
        }
        
    }
    
    if(0 == name.compare(CFG_CORRECTION_TEMPERATUR_OBJ_NAME)) {
        try {
            float val = std::stof(value);
            mCorrectionList[DHT_TEMPERATUR_SENSOR_INDEX] = val;
			res = ar_success;
        }
        catch(...)  {
            SLOG_ERROR(mAliasName, ": invalid configuration value ", name, " = ", value);
        }
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

a291unit::ActionResult a291unit::DhtxxSensor::listConfiguration(std::ofstream& cfg_out) {
    std::string type = (mSensorType == SENSOR_TYPE_DHT11) ? CFG_SENSOR_DHT11_USR_NAME : CFG_SENSOR_DHT22_USR_NAME;
    cfg_out << "\n# - - " << type << " " << mAliasName << " - -\n";
    cfg_out << '[' << type << "]\n";
    cfg_out << CFG_CONFIGURABEL_ALIAS_USR_NAME << " = " << mAliasName << '\n';
    cfg_out << CFG_GPIO_PIN_USR_NAME << " = " << getPinConfiguration() << '\n';
    cfg_out << CFG_VALUE_HUMIDITY_USR_NAME << " = " << mNameList[DHT_HUMIDITY_SENSOR_INDEX] << '\n';
    cfg_out << CFG_VALUE_TEMPERATURE_USR_NAME << " = " << mNameList[DHT_TEMPERATUR_SENSOR_INDEX] << '\n';
    cfg_out << CFG_CORRECTION_HUMIDITY_USR_NAME << " = " << mCorrectionList[DHT_HUMIDITY_SENSOR_INDEX] << '\n';
    cfg_out << CFG_CORRECTION_TEMPERATUR_USR_NAME << " = " << mCorrectionList[DHT_TEMPERATUR_SENSOR_INDEX] << '\n';
	
	return ar_success;
}

a291unit::ActionResult a291unit::DhtxxSensor::onCheckConfiguration() {
	ActionResult ar = ar_success;
	
	if(mAliasName.length() == 0) {
		mAliasName = mConfigurableType + "-" + std::to_string(mGpioPinList[0]);
	}
	
	if(!GpioManager::isGpioPin(mGpioPinList[0])) {
		SLOG_ERROR(mAliasName, " ", CFG_GPIO_PIN_USR_NAME, " = ", getPinConfiguration(), " is invalid");
		ar = ar_configurationError;
	}
	
	mCurrentStatus = (ar == ar_success) ? cs_ready : cs_disabled;

	return ar;
}

const a291unit::CofigurabelStatus& a291unit::DhtxxSensor::getCurrentStatus() {
	time_t TimeNow = time(nullptr);
	double elapsed_secs = double(TimeNow - mLastReadTimestamp);
	
	if(mCurrentStatus == cs_sampling &&  elapsed_secs >= 10) {
		mCurrentStatus = cs_idle;
	}
	
	return mCurrentStatus;
}
