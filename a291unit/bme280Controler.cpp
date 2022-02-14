#include "bme280Controler.h"
#include "bme280Sensor.h"
#include "controler.h"
#include "slog.h"
#include "bme280.h"
#include "bme280_defs.h"

#include <algorithm>
#include <bcm2835.h>
#include <unistd.h>

a291unit::Bme280Controler::Bme280Controler() : 
Controler(CFG_BME280_CTRL_OBJ_NAME, CFG_BME280_CTRL_MAX_SENSOR) {
	mAliasName = CFG_BME280_CTRL_USR_NAME;
}

bool a291unit::Bme280Controler::listConfiguration(std::ofstream& cfg_out) {
	Configurabel* c = getFirstConfigurabel();
	while(c != NULL) {
		c->listConfiguration(cfg_out);
		c = getNextConfigurabel(c);
	}
	
    return true;
}

bool a291unit::Bme280Controler::addConfigurabel(Configurabel* configurable) {
    bool rtc = true;
    
    if(configurable->isConfigurabelTypeName(CFG_SENSOR_BME280_OBJ_NAME)) {
		// apend the sensor to the controler, if the retun configurable is null, append failed
		Configurabel* c = mContolerSensorList.append(configurable);

        if(c == NULL) {
            SLOG_ERROR(mAliasName, " the senslor list is full");
			rtc = false;
        }
		else {
			//add the gpio pins from the configurable to the controler
			for(int i=0; i<c->getGpioPinCount(); i++) {
				addGpioPin(c->getGpioPin(i));
			}
		
		}
    }
    else {
        // invalid sensor type
        SLOG_ERROR(mAliasName, " invalid sensor type -> ", configurable->getConfigurabelType());
		rtc = false;
    }
	
    
    return rtc;
}

a291unit::ActionResult a291unit::Bme280Controler::onInitialize() {
	ActionResult ar = ar_success;
	
	struct bme280_dev *bme280Dev;
	
	bcm2835_i2c_begin();
	bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);
	
	Bme280Sensor *sensor = (Bme280Sensor*) getFirstConfigurabel();
	
	while(sensor != NULL) {
		if(sensor->getCurrentStatus()== cs_ready) {
			// sensor accepted
			bme280Dev = sensor->getDevice();
			bme280Dev->delay_us = a291unit::Bme280Controler::i2c_delay_us;
			bme280Dev->write = a291unit::Bme280Controler::i2c_write;
			bme280Dev->read = a291unit::Bme280Controler::i2c_read;
			
			// Initialize the bme280 
			int8_t rslt = bme280_init(bme280Dev);
			if (rslt != BME280_OK)
			{
				SLOG_ERROR("initialize sensor ", sensor->getConfigurabelType(), " on PIN ", sensor->getPinConfiguration(), " FAILED");
				SLOG_ERROR("error code: ", std::to_string(rslt));
				ar = ar_configurationError;
			}
			else {
				SLOG_INFO("initialize sensor ", sensor->getConfigurabelType(), " on PIN ", sensor->getPinConfiguration(), " OK");
			}
			
			
			set_mode_weather_monitoring(bme280Dev);

			sensor->setDevice(bme280Dev);
			
			sensor->onInitialize();
		}
		else {
			SLOG_ERROR(sensor->getAliasName(), " can not be initialized. Sensor status is ", Configurabel::StatusToString(sensor->getCurrentStatus()));
		}
		
		sensor = (Bme280Sensor*) getNextConfigurabel(sensor);
	}

	return ar;
}

a291unit::ActionResult a291unit::Bme280Controler::onAquire() {
	ActionResult ar = ar_success;
	
	Bme280Sensor* sensor = (Bme280Sensor*) getFirstConfigurabel();
	while(sensor != NULL) {
		CofigurabelStatus cs = sensor->getCurrentStatus();
		switch(cs) {
			case cs_idle:
			read_sensor_data(sensor);
			break;
			
			case cs_sampling:
			// ignore sampling state as this indicates the time between samples
			break;

			default:
			ar = ar_unrecoverable;
			break;

		}

		sensor = (Bme280Sensor*) getNextConfigurabel(sensor);
	}
	
	return ar;
}

void a291unit::Bme280Controler::i2c_delay_us(uint32_t period, void* intf_ptr) {
    bcm2835_delayMicroseconds(period);
}


/**
 * @brief I2C read data
 * 
 * Data on the bus should be like
 * |------------+---------------------|
 * | I2C action | Data                |
 * |------------+---------------------|
 * | Start      | -                   |
 * | Write      | (reg_addr)          |
 * | Stop       | -                   |
 * | Start      | -                   |
 * | Read       | (reg_data[0])       |
 * | Read       | (....)              |
 * | Read       | (reg_data[len - 1]) |
 * | Stop       | -                   |
 * |------------+---------------------|
 * 
 * @param reg_addr
 * @param reg_data
 * @param len
 * @param intf_ptr
 * @return 
 */
int8_t a291unit::Bme280Controler::i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr) {
	uint8_t dev_addr = ((Bme280Sensor*)(intf_ptr))->getDeviceAddress();
	bcm2835_i2c_setSlaveAddress(dev_addr);

	uint8_t rtc = bcm2835_i2c_read_register_rs((char*)&reg_addr, (char*)reg_data, len);

    return (rtc == 0) ? BME280_OK : -1;
}

/**
 * @brief I2C write data to device
 * 
 * Data on the bus should be like
 * |------------+---------------------|
 * | I2C action | Data                |
 * |------------+---------------------|
 * | Start      | -                   |
 * | Write      | (reg_addr)          |
 * | Write      | (reg_data[0])       |
 * | Write      | (....)              |
 * | Write      | (reg_data[len - 1]) |
 * | Stop       | -                   |
 * |------------+---------------------|
 * 
 * @param reg_addr the register address
 * 
 * @param reg_data the register data array
 * 
 * @param len the number of bytes to be sent
 * 
 * @param intf_ptr pinter to the Bme280Sensor object
 * 
 * @return int8_t BME280_OK : BME280_E_COMM_FAIL
 */
int8_t a291unit::Bme280Controler::i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr) {
	uint8_t dev_addr = ((Bme280Sensor*)(intf_ptr))->getDeviceAddress();
	bcm2835_i2c_setSlaveAddress(dev_addr);

	uint8_t tmp_reg_data[len + 1];
	tmp_reg_data[0] = reg_addr;
	for(int i=0; i<len; i++) { tmp_reg_data[i+1] = reg_data[i]; }

	uint8_t rtc = bcm2835_i2c_write((char*)&tmp_reg_data, len + 1);
	
    return (rtc == BCM2835_I2C_REASON_OK) ? BME280_OK : BME280_E_COMM_FAIL; 
	
}

/* *
 * Recommended mode of operation: weather monitoring 
 * */
int8_t a291unit::Bme280Controler::set_mode_weather_monitoring(struct bme280_dev* bmeDevice) {
    /* Variable to define the result */
    int8_t rslt = BME280_OK;

    bmeDevice->settings.osr_h = BME280_OVERSAMPLING_1X;
    bmeDevice->settings.osr_p = BME280_NO_OVERSAMPLING;
    bmeDevice->settings.osr_t = BME280_OVERSAMPLING_1X;
    bmeDevice->settings.filter = BME280_FILTER_COEFF_OFF;

    uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
    rslt = bme280_set_sensor_settings(settings_sel, bmeDevice);
    if (rslt != BME280_OK)
    {
		SLOG_ERROR("Failed to set sensor settings (error: ", std::to_string(rslt), ")" );
    }
	
	return rslt;
	
}

int8_t a291unit::Bme280Controler::read_sensor_mode(struct bme280_dev* bmeDevice) {
	uint8_t sm;
	bme280_get_sensor_mode(&sm, bmeDevice);		
/*
	#ifdef DEBUG
	std::string name = ((Bme280Sensor*)(bmeDevice->intf_ptr))->getAliasName();
	switch(sm) {
		case 0:
		SLOG_INFO(name, ": ", std::to_string(sm), " Sleep mode");
		break;

		case 1:
		case 2:
		SLOG_INFO(name, ": ", std::to_string(sm), " Forced mode");
		break;
		
		case 3:
		SLOG_INFO(name, ": ", std::to_string(sm), " Normal mode");
		break;
		
	}
	#endif
*/	
	return sm;
}

void a291unit::Bme280Controler::read_sensor_data(Bme280Sensor* sensor) {
		
		/* Structure to get the pressure, temperature and humidity values */
		struct bme280_data comp_data;

		struct bme280_dev *bme280Dev = sensor->getDevice();

		/*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
		 *  and the oversampling configuration. */
		uint32_t req_delay = bme280_cal_meas_delay(&bme280Dev->settings);

        /* Set the sensor to forced mode */
        uint8_t rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, bme280Dev);
        if (rslt != BME280_OK)
        {
			SLOG_ERROR("Failed to set sensor mode (error: ", std::to_string(rslt), ")" );
			sensor->updateValues(NULL, 0, ar_responseTimeout);
        }
		
		else {
			read_sensor_mode(bme280Dev);
			
			/* Wait for the measurement to complete and print data */
			// dev.delay_us(req_delay, dev.intf_ptr);
			bme280Dev->delay_us(10000, bme280Dev);
			read_sensor_mode(bme280Dev);

			
			rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, bme280Dev);
			if (rslt != BME280_OK)
			{
				SLOG_ERROR("Failed to get sensor data (error: ", std::to_string(rslt), ")" );
				sensor->updateValues(NULL, 0, ar_responseTimeout);
			}
			else {
				set_sensor_data(bme280Dev, &comp_data);
			}		
		}
}

void a291unit::Bme280Controler::set_sensor_data(struct bme280_dev* bmeDevice, struct bme280_data* comp_data) {

	Bme280Sensor* sensor = (Bme280Sensor*)bmeDevice->intf_ptr;
	
	const int len = sensor->getValueCount();
	float *values = new float[len];
	
	#ifdef BME280_FLOAT_ENABLE
	values[BME280_HUMIDITY_VALUE_INDEX] = comp_data->humidity;
	values[BME280_TEMPERATUR_VALUE_INDEX] = comp_data->temperature;
	values[BME280_PRESSURE_VALUE_INDEX] = 0.01 * comp_data->pressure;
	#else
	#ifdef BME280_64BIT_ENABLE
	values[BME280_HUMIDITY_VALUE_INDEX] = 1.0f / 1024.0f * comp_data->humidity;
	values[BME280_TEMPERATUR_VALUE_INDEX] = 0.01f * comp_data->temperature;
	values[BME280_PRESSURE_VALUE_INDEX] = 0.0001f * comp_data->pressure;
	#else
	values[BME280_HUMIDITY_VALUE_INDEX] = 1.0f / 1024.0f * comp_data->humidity;
	values[BME280_TEMPERATUR_VALUE_INDEX] = 0.01f * comp_data->temperature;
	values[BME280_PRESSURE_VALUE_INDEX] = 0.01f * comp_data->pressure;
	#endif
	#endif

	sensor->updateValues(values, len, ar_success);
	delete values;
}

a291unit::ActionResult a291unit::Bme280Controler::onTerminate() {
	
	Bme280Sensor* sensor = (Bme280Sensor*) mContolerSensorList.getFirst();
	while(sensor != NULL) {
		sensor = (Bme280Sensor*) mContolerSensorList.getNext(sensor);
	}
	
    bcm2835_i2c_end();
	return ar_success;
}
