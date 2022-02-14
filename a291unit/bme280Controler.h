#ifndef BME280CONTROLER_H
#define BME280CONTROLER_H

//#include "a291unitDefinitions.h"
#include "controler.h"
//#include "sensorDevice.h"
#include "bme280_defs.h"
#include "bme280Sensor.h"
//#include "configurabel.h"


#define CFG_BME280_CTRL_USR_NAME "i2c bme280 controler"
#define CFG_BME280_CTRL_OBJ_NAME "I2CBME280CONTROLER"
#define CFG_BME280_CTRL_MAX_SENSOR 4


namespace a291unit {
	
	class Bme280Controler : public Controler {
	private:
		static void i2c_delay_us(uint32_t period, void *intf_ptr);
		static int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
		static int8_t i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

		int8_t set_mode_weather_monitoring(struct bme280_dev *bmeDevice);
		void set_sensor_data(struct bme280_dev *bmeDevice, struct bme280_data *comp_data);
		int8_t read_sensor_mode(struct bme280_dev *bmeDevice);
		void read_sensor_data(Bme280Sensor* sensor);

	protected:
	
	public:
		// constructor
		Bme280Controler();
		
        // controler implementations
        virtual bool addConfigurabel(Configurabel* configurable);
        virtual bool listConfiguration(std::ofstream &cfg_out);
//		virtual ActionResult onCheckConfiguration(ConfigurationList* configurabelList);
        virtual ActionResult onInitialize();
        virtual ActionResult onAquire();
		virtual ActionResult onTerminate();

	};
	
}

#endif
