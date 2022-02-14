#include "bme280Sensor.h"
#include "bme280Controler.h"
#include "slog.h"

#include <algorithm>
#include <fstream>

a291unit::Bme280Sensor::Bme280Sensor(int instanceNumber) : 
Configurabel(instanceNumber, CFG_BME280_CTRL_OBJ_NAME, CFG_SENSOR_BME280_OBJ_NAME, BME280_NUMBER_OF_PIN_USED, BME280_NUMBER_OF_SENSOR_VALUES, 0) {

    mNameList[BME280_TEMPERATUR_VALUE_INDEX] = "temperature";
    mNameList[BME280_HUMIDITY_VALUE_INDEX] = "humidity";
    mNameList[BME280_PRESSURE_VALUE_INDEX] = "pressure";
	
    mValueList[BME280_TEMPERATUR_VALUE_INDEX] = 0;
    mValueList[BME280_HUMIDITY_VALUE_INDEX] = 0;
    mValueList[BME280_PRESSURE_VALUE_INDEX] = 0;

    mUnitList[BME280_HUMIDITY_VALUE_INDEX] = "%";
    mUnitList[BME280_TEMPERATUR_VALUE_INDEX] = "°C";
    mUnitList[BME280_PRESSURE_VALUE_INDEX] = "hPa";
    
    mLastReadResult = ar_success;
	
	mDevice = new struct bme280_dev;
	mDevice->intf_ptr = this;
	mDevice->intf = BME280_I2C_INTF;
	mDevice->chip_id = 0;
	
	mCurrentStatus = cs_configured;
	
}

a291unit::ActionResult a291unit::Bme280Sensor::setConfiguration(std::string name, std::string value) {
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	ActionResult ar = ar_configurationError;
 
    if(0 == name.compare(CFG_CONFIGURABEL_ALIAS_OBJ_NAME)) {
        mAliasName = value;
        ar = ar_success;
    }

    if(0 == name.compare(CFG_VALUE_HUMIDITY_OBJ_NAME)) {
        mNameList[BME280_HUMIDITY_VALUE_INDEX] = value;
        ar = ar_success;
    }

    if(0 == name.compare(CFG_VALUE_TEMPERATURE_OBJ_NAME)) {
        mNameList[BME280_TEMPERATUR_VALUE_INDEX] = value;
        ar = ar_success;
    }

    if(0 == name.compare(CFG_VALUE_PPRESSURE_OBJ_NAME)) {
        mNameList[BME280_PRESSURE_VALUE_INDEX] = value;
        ar = ar_success;
    }

    if(0 == name.compare(CFG_GPIO_PIN_OBJ_NAME)) {
		if(setPinConfiguration(value)) ar = ar_success;
    }

	if(ar != ar_success) {
		SLOG_ERROR(mAliasName, ": invalid configuration element ", name, " = ", value);
	}

    return ar; 
}

a291unit::ActionResult a291unit::Bme280Sensor::listConfiguration(std::ofstream& cfg_out) {
    cfg_out << "\n# - - " << CFG_SENSOR_BME280_USR_NAME << " " << mAliasName << " - -\n";
    cfg_out << '[' << CFG_SENSOR_BME280_USR_NAME << "]\n";
    cfg_out << CFG_CONFIGURABEL_ALIAS_USR_NAME << " = " << mAliasName << '\n';
    cfg_out << CFG_GPIO_PIN_USR_NAME << " = " << getPinConfiguration() << '\n';
    cfg_out << CFG_VALUE_HUMIDITY_USR_NAME << " = " << mNameList[BME280_HUMIDITY_VALUE_INDEX] << '\n';
    cfg_out << CFG_VALUE_TEMPERATURE_USR_NAME << " = " << mNameList[BME280_TEMPERATUR_VALUE_INDEX] << '\n';
    cfg_out << CFG_VALUE_PPRESSURE_USR_NAME << " = " << mNameList[BME280_PRESSURE_VALUE_INDEX] << '\n';

    return ar_success;
}

a291unit::ActionResult a291unit::Bme280Sensor::onCheckConfiguration() {
	bool configOk = true;
	
	uint8_t gpio_0 = getGpioPin(0);
	uint8_t gpio_1 = getGpioPin(1);
	
	if(gpio_0 == gpio_1) configOk = false; // sda and scl are different pin numbers
	
	if(! (gpio_0 == 2 || gpio_0 == 3)) configOk = false; // pin is not sda or scl
	if(! (gpio_1 == 2 || gpio_1 == 3)) configOk = false; // pin is not sda or scl
	
	if(configOk) {
		mCurrentStatus = cs_ready;
	}
	else {
		SLOG_ERROR(mAliasName, " invalid connection PIN configuration ", getPinConfiguration(), " must be 3,5");
		mCurrentStatus =  cs_disabled;
	}
	
	return (configOk) ? ar_success : ar_configurationError;
}

const a291unit::CofigurabelStatus& a291unit::Bme280Sensor::getCurrentStatus() {
	time_t TimeNow = time(nullptr);
	double elapsed_secs = double(TimeNow - mLastReadTimestamp);
	
	if(mCurrentStatus == cs_sampling && elapsed_secs >= BME280_SENSOR_READ_INTERVAL) {
		mCurrentStatus = cs_idle;
	}
	
	return mCurrentStatus;
}
