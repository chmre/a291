#ifndef DHTXXSENSOR_H
#define DHTXXSENSOR_H

//#include "dhtControler.h"
#include "configurabel.h"

#define SENSOR_TYPE_DHT11 0
#define SENSOR_TYPE_DHT22 1
#define DHT_NUMBER_OF_PIN_USED 1
#define DHT_NUMBER_OF_SENSOR_VALUES 2
#define DHT_HUMIDITY_SENSOR_INDEX 0
#define DHT_TEMPERATUR_SENSOR_INDEX 1

// dht11 & DHT22 response timout in mS
#define DHT_RESPONSE_TIMEOUT 100

// host start signal is low for at least 18mS
#define DHT_REQUEST_PULS_LOW_TIME_18mS 18

// data puls length for a bit is maximal 70uS, the timeout is set somwath higher
#define DHT_BIT_PULS_TIMEOUT 400

#define DHT_DATA_START_LOW_TIME 50
#define DHT_DATA_VALUE_0_TIME 27
#define DHT_DATA_VALUE_1_TIME 70

// the time delay between sampling in seconds
#define DHT_WAIT_AFTER_SAMPLE 5

// init pulse width
#define DHT_INIT_PULS_WIDTH 80

namespace a291unit {
	
	class DhtxxSensor : public Configurabel {
	private:
		uint mSensorType;
		
	public:
		DhtxxSensor(uint dhtSensorType, int instanceNumber);

		// overwrite status report
		virtual const CofigurabelStatus& getCurrentStatus();

		// pure virtual configurable functions
		virtual ActionResult setConfiguration(std::string name, std::string value);
		virtual ActionResult listConfiguration(std::ofstream &cfg_out);
		virtual ActionResult onCheckConfiguration();
		virtual ActionResult onInitialize() { mCurrentStatus = (mCurrentStatus == cs_ready) ? cs_idle : mCurrentStatus; return ar_success;}
		virtual ActionResult onRunCycle() { return ar_success; }

	};
	
}

#endif