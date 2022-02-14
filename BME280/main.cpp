#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
#include <bme280.h>
#include <unistd.h>

/* Structure that contains identifier details used in example */
struct identifier
{
    /* Variable to hold device address */
    uint8_t dev_addr;

    /* Variable that contains file descriptor */
    int8_t fd;
};

void user_delay_us(uint32_t period, void *intf_ptr)
{
    /*
     * Return control or wait,
     * for a period amount of milliseconds
     */
    bcm2835_delayMicroseconds(period);
}

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
//    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    /*
     * The parameter intf_ptr can be used as a variable to store the I2C address of the device
     */

    /*
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
     */

	uint8_t* address = static_cast<uint8_t*>(intf_ptr);
	bcm2835_i2c_setSlaveAddress(*address);

	uint8_t rtc = bcm2835_i2c_read_register_rs((char*)&reg_addr, (char*)reg_data, len);
    return (rtc == 0) ? BME280_OK : -1;
}

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
//    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    /*
     * The parameter intf_ptr can be used as a variable to store the I2C address of the device
     */

    /*
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
     */

	uint8_t* dev_addr = static_cast<uint8_t*>(intf_ptr);
	bcm2835_i2c_setSlaveAddress(*dev_addr);

	uint8_t tmp_reg_data[len + 1];
	tmp_reg_data[0] = reg_addr;
	for(int i=0; i<len; i++) { tmp_reg_data[i+1] = reg_data[i]; }
	
	printf("I2C Write to: 0x%x len: %d data:", *dev_addr, len);
	for(int i=0; i<len+1; i++) {printf(" 0x%x", tmp_reg_data[i]);}
	printf("\n");
	
	uint8_t rtc = bcm2835_i2c_write((char*)&tmp_reg_data, len + 1);
	
    return (rtc == BCM2835_I2C_REASON_OK) ? BME280_OK : BME280_E_COMM_FAIL; 
}

/*!
 * @brief This API used to print the sensor temperature, pressure and humidity data.
 */
void print_sensor_data(struct bme280_data *comp_data)
{
    float temp, press, hum;

#ifdef BME280_FLOAT_ENABLE
    temp = comp_data->temperature;
    press = 0.01 * comp_data->pressure;
    hum = comp_data->humidity;
#else
#ifdef BME280_64BIT_ENABLE
    temp = 0.01f * comp_data->temperature;
    press = 0.0001f * comp_data->pressure;
    hum = 1.0f / 1024.0f * comp_data->humidity;
#else
    temp = 0.01f * comp_data->temperature;
    press = 0.01f * comp_data->pressure;
    hum = 1.0f / 1024.0f * comp_data->humidity;
#endif
#endif
    printf("Temperature: %0.2lf°C\nQFE: %0.2lfhPa\nHumitity: %0.2lf%%rH\n", temp, press, hum);
}


/*!
 * @brief This API reads the sensor temperature, pressure and humidity data in forced mode.
 */
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev)
{
    /* Variable to define the result */
    int8_t rslt = BME280_OK;

    /* Variable to define the selecting sensors */
    uint8_t settings_sel = 0;

    /* Variable to store minimum wait time between consecutive measurement in force mode */
    uint32_t req_delay;

    /* Structure to get the pressure, temperature and humidity values */
    struct bme280_data comp_data;

    /* Recommended mode of operation: Indoor navigation */
//    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
//    dev->settings.osr_p = BME280_OVERSAMPLING_16X;
//    dev->settings.osr_t = BME280_OVERSAMPLING_2X;
//    dev->settings.filter = BME280_FILTER_COEFF_16;

    /* Recommended mode of operation: weather monitoring */
    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_NO_OVERSAMPLING;
    dev->settings.osr_t = BME280_OVERSAMPLING_1X;
    dev->settings.filter = BME280_FILTER_COEFF_OFF;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
    rslt = bme280_set_sensor_settings(settings_sel, dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor settings (code %+d).", rslt);

        return rslt;
    }

    printf("Temperature, Pressure, Humidity\n");

    /*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
     *  and the oversampling configuration. */
    req_delay = bme280_cal_meas_delay(&dev->settings);

    /* Continuously stream sensor data */
    while (1)
    {
        /* Set the sensor to forced mode */
        rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
        if (rslt != BME280_OK)
        {
            fprintf(stderr, "Failed to set sensor mode (code %+d).", rslt);
            break;
        }

        /* Wait for the measurement to complete and print data */
        dev->delay_us(req_delay, dev->intf_ptr);
        rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
        if (rslt != BME280_OK)
        {
            fprintf(stderr, "Failed to get sensor data (code %+d).", rslt);
            break;
        }

        print_sensor_data(&comp_data);
		sleep(5);
    }

    return rslt;
}


void print_config_register(struct bme280_dev *dev) {

	uint8_t regSettings;
	bme280_get_regs(BME280_CONFIG_ADDR, &regSettings, 1, dev);

	printf("Config read: 0x%x = 0x%x\n", BME280_CONFIG_ADDR, regSettings);
	
}

void print_status_register(struct bme280_dev *dev) {

	uint8_t regSettings;
	bme280_get_regs(BME280_STATUS_REG_ADDR, &regSettings, 1, dev);

	printf("Status reg: 0x%x = 0x%x\n", BME280_STATUS_REG_ADDR, regSettings);
	
}

void print_ctrl_hum_register(struct bme280_dev *dev) {

	uint8_t regSettings;
	bme280_get_regs(BME280_CTRL_HUM_ADDR, &regSettings, 1, dev);

	printf("Control humidity reg: 0x%x = 0x%x\n", BME280_CTRL_HUM_ADDR, regSettings);
	
}

void print_sensor_mode(struct bme280_dev *dev) {

	uint8_t sm;
	bme280_get_sensor_mode(&sm, dev);		

	switch(sm) {
		case 0:
		printf("Sensor mode: (0x%x) Sleep mode\n", sm);
		break;

		case 1:
		case 2:
		printf("Sensor mode: (0x%x) Forced mode\n", sm);
		break;
		
		case 3:
		printf("Sensor mode: (0x%x) Normal mode\n", sm);
		break;
		
	}
	
}

/* *
 * Recommended mode of operation: weather monitoring 
 * */
int8_t set_mode_weather_monitoring(struct bme280_dev *dev){
    /* Variable to define the result */
    int8_t rslt = BME280_OK;

    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_NO_OVERSAMPLING;
    dev->settings.osr_t = BME280_OVERSAMPLING_1X;
    dev->settings.filter = BME280_FILTER_COEFF_OFF;

    uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
    rslt = bme280_set_sensor_settings(settings_sel, dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor settings (code %+d).", rslt);
    }
	
	return rslt;
}



int main(int argc, char **argv)
{
    if(bcm2835_init()) {
        int8_t rslt = BME280_OK;
        uint8_t dev_addr = BME280_I2C_ADDR_PRIM;

		struct identifier id;
		id.dev_addr = dev_addr;

        struct bme280_dev dev;
        dev.intf_ptr = &id;
        dev.intf = BME280_I2C_INTF;
        dev.read = user_i2c_read;
        dev.write = user_i2c_write;
        dev.delay_us = user_delay_us;
		dev.chip_id = 0;
		
		printf("Device address: 0x%x\n", ((identifier*)dev.intf_ptr)->dev_addr);
		printf("excpected chip ID: 0x%x\n", BME280_CHIP_ID);
		printf("initialized chip ID: 0x%x\n", dev.chip_id);
		
        bcm2835_i2c_begin();
        bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);

		// Initialize the bme280 
		rslt = bme280_init(&dev);
		if (rslt != BME280_OK)
		{
			fprintf(stderr, "Failed to initialize the device (code %+d).\n", rslt);
			printf("readback chip ID: 0x%x\n", dev.chip_id);
			exit(1);
		}
		
		printf("found chip ID: 0x%x\n", dev.chip_id);
  
		print_status_register(&dev);
		
		print_sensor_mode(&dev);
		
		printf("Setting mode weather station\n");
		set_mode_weather_monitoring(&dev);
		print_sensor_mode(&dev);
		
		/* Structure to get the pressure, temperature and humidity values */
		struct bme280_data comp_data;

		/*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
		 *  and the oversampling configuration. */
		uint32_t req_delay = bme280_cal_meas_delay(&dev.settings);

        /* Set the sensor to forced mode */
        rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
        if (rslt != BME280_OK)
        {
            fprintf(stderr, "Failed to set sensor mode (code %+d).", rslt);
        }
		
		print_sensor_mode(&dev);
		
        /* Wait for the measurement to complete and print data */
        // dev.delay_us(req_delay, dev.intf_ptr);
		dev.delay_us(10000, dev.intf_ptr);
		print_sensor_mode(&dev);

		
        rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
        if (rslt != BME280_OK)
        {
            fprintf(stderr, "Failed to get sensor data (code %+d).", rslt);
        }

        print_sensor_data(&comp_data);

        bcm2835_i2c_end();
        
    } else {
        printf("BCM2835 init failed\n");
    }

	return 0;
}
