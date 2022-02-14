#ifndef BME280SENSOR_H
#define BME280SENSOR_H

#include "configurabel.h"
#include "bme280.h"

#define SENSOR_TYPE_BME280 0
#define BME280_NUMBER_OF_PIN_USED 2
#define BME280_NUMBER_OF_SENSOR_VALUES 3
#define BME280_HUMIDITY_VALUE_INDEX 0
#define BME280_TEMPERATUR_VALUE_INDEX 1
#define BME280_PRESSURE_VALUE_INDEX 2

#define BME280_SENSOR_READ_INTERVAL 10

namespace a291unit {
    
    class Bme280Sensor : public Configurabel {
	private:
        struct bme280_dev *mDevice;
        uint8_t mDeviceAddress = BME280_I2C_ADDR_PRIM;

	public:
		// constructor
		Bme280Sensor(int instanceNumber);
		
		// implement ConfigObject
		virtual ActionResult setConfiguration(std::string name, std::string value);
		virtual ActionResult listConfiguration(std::ofstream &cfg_out);
		
		// implement SensorDevice
		virtual ActionResult onCheckConfiguration();
		virtual ActionResult onInitialize() { mCurrentStatus = cs_idle; return ar_success;}
		virtual ActionResult onRunCycle() {return ar_success;}
	
		// getter and setter for the device structure
		virtual const CofigurabelStatus& getCurrentStatus();
		
		void setDevice(bme280_dev* mDevice) {this->mDevice = mDevice;}
		bme280_dev* getDevice() {return mDevice;}
		
		
		const uint8_t& getDeviceAddress() const {return mDeviceAddress;}
	};
	
}

#endif
