#ifndef A291UNITDEFINITIONS_H
#define A291UNITDEFINITIONS_H

#include <bcm2835.h>

// the configuration file name
#define A291_CONFIGURATION_FILE "./a291.config"

/*
 * the supported configurable objects
 */
#define CFG_SENSOR_BME280_USR_NAME "bme280"
#define CFG_SENSOR_BME280_OBJ_NAME "BME280"

#define CFG_SENSOR_DHT11_USR_NAME "Dht11"
#define CFG_SENSOR_DHT11_OBJ_NAME "DHT11"

#define CFG_SENSOR_DHT22_USR_NAME "Dht22"
#define CFG_SENSOR_DHT22_OBJ_NAME "DHT22"

#define CFG_SENSOR_DS18B20_USR_NAME "Ds18b20"
#define CFG_SENSOR_DS18B20_OBJ_NAME "DS18B20"

#define CFG_GPIO_INPUT_USR_NAME "gpio in"
#define CFG_GPIO_INPUT_OBJ_NAME "GPIOIN"

#define CFG_MQTT_OUTPUT_USR_NAME "mqtt control"
#define CFG_MQTT_OUTPUT_OBJ_NAME "MQTTCONTROL"


/*
 * the supported configuration items
 */

// GPIO number configuration item
#define CFG_GPIO_PIN_USR_NAME "connection pin"
#define CFG_GPIO_PIN_OBJ_NAME "CONNECTIONPIN"

// configuration item for alias name of a configurabel opject
#define CFG_CONFIGURABEL_ALIAS_USR_NAME "alias name"
#define CFG_CONFIGURABEL_ALIAS_OBJ_NAME "ALIASNAME"

#define CFG_VALUE_HUMIDITY_USR_NAME "humidity name"
#define CFG_VALUE_HUMIDITY_OBJ_NAME "HUMIDITYNAME"
#define CFG_VALUE_TEMPERATURE_USR_NAME "teperature name"
#define CFG_VALUE_TEMPERATURE_OBJ_NAME "TEPERATURENAME"
#define CFG_VALUE_PPRESSURE_USR_NAME "pressure name"
#define CFG_VALUE_PPRESSURE_OBJ_NAME "PRESSURENAME"

#define CFG_DS18B20_REG_ID_USR_NAME "registration id"
#define CFG_DS18B20_REG_ID_OBJ_NAME "REGISTRATIONID"

#define CFG_GPIO_VALUE_USR_NAME "io name"
#define CFG_GPIO_VALUE_OBJ_NAME "IONAME"

// sensor correction values
#define CFG_CORRECTION_HUMIDITY_USR_NAME "humidity correction"
#define CFG_CORRECTION_HUMIDITY_OBJ_NAME "HUMIDITYCORRECTION"
#define CFG_CORRECTION_TEMPERATUR_USR_NAME "teperature correction"
#define CFG_CORRECTION_TEMPERATUR_OBJ_NAME "TEPERATURECORRECTION"
#define CFG_CORRECTION_PRESSURE_USR_NAME "pressure correction"
#define CFG_CORRECTION_PRESSURE_OBJ_NAME "PRESSURECORRECTION"

#define CFG_INVALID_PIN_ID 0xff

#define URTC_SUCCESS 0
#define URTC_FILE_NOT_FOUND -1
#define URTC_INVALID_CONFIG_OBJECT -2
#define URTC_INVALID_CONFIG_OPTION -3
#define URTC_DEVICE_CONTROLER_ERROR -4
#define URTC_CONTROLER_HAS_INVALID_PIN -5
#define URTC_SENSOR_INIT_FAIL -6
#define URTC_INITIALISATION_FAILED -7
#define URTC_BCM2835_INIT_FAILED -8


namespace a291unit {

    // attributes
    extern int DELAY_COUNT_1uS;

    enum SensorOperationResult 
    {
        noSample,
        success,
        responseTimeout,
        crcError,
		dataReadError,
		configurationError,
		signalError,
    };

    enum SensorStatus {
		configured,
        ready,
        idle,
		disabled,
    };

}

#endif
