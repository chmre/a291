#include "dhtControler.h"
#include "slog.h"
#include "configurationList.h"

#include <algorithm>
#include <bcm2835.h>
#include <unistd.h>
#include <ostream>
#include <iostream>

#define DHT_CTRL_NBR_OF_PIN 1
#define DHT_CTRL_MAX_DEVICE 1

	

a291unit::DhtContoler::DhtContoler() : 
Controler(CFG_DHT_CTRL_OBJ_NAME, CFG_DHT_CTRL_MAX_SENSOR) {
	mSensorAssignment = new GpioManager();

	// presume external pull up resistor
	mPudControl = BCM2835_GPIO_PUD_OFF;
	
	mAliasName = CFG_DHT_CTRL_USR_NAME;
}
/*
bool a291unit::DhtContoler::listConfiguration(std::ofstream &cfg_out) {
	Configurabel* cfg = mContolerSensorList->getFirst();
	while(cfg != NULL) {
		cfg->listConfiguration(cfg_out);
		cfg = mContolerSensorList->getNext();
	}
	
    return true;
}
*/
bool a291unit::DhtContoler::addConfigurabel(Configurabel* configurable) {
    bool rtc = false;
    
	if(configurable->getGpioPin(0) == CFG_INVALID_PIN_ID) {
		SLOG_ERROR("Invalid configuration [", configurable->getConfigurabelType(), "] missing mandatory token >", CFG_GPIO_PIN_USR_NAME, "<");
		return URTC_INVALID_CONFIG_OPTION;
    }
	
	if(configurable->isConfigurabelTypeName(CFG_SENSOR_DHT11_OBJ_NAME) 
	|| configurable->isConfigurabelTypeName(CFG_SENSOR_DHT22_OBJ_NAME)) {		
		if(mContolerSensorList.append(configurable) == NULL) {
			// list is full
            SLOG_ERROR(CFG_DHT_CTRL_USR_NAME, " the sensor list is full");
		}
		else {
			//append the gpio pins of the added sensor
			addGpioPin(configurable->getGpioPin(0));
			rtc = true;
		}

	}
	else {
		// invalid sensor type
		SLOG_ERROR(CFG_DHT_CTRL_USR_NAME, " invalid sensor type. ", configurable->getConfigurabelType(), " is not supported");
	}
	
    return rtc;
}

a291unit::ActionResult a291unit::DhtContoler::onInitialize() {
	ActionResult rtc = ar_success;

	DhtxxSensor* sensor = (DhtxxSensor*) mContolerSensorList.getFirst();
	
	while(sensor != NULL) {
		if(sensor->getCurrentStatus() == cs_ready) {
			rtc = InitializeSensor(sensor);
			if(rtc != ar_success) {
				SLOG_ERROR("initialize sensor ", sensor->getAliasName(), " on PIN ", sensor->getPinConfiguration(), " FAILED");
			}
			#ifdef DEBUG
			else {
				SLOG_DEBUG("initialize sensor ", sensor->getAliasName(), " on PIN ", sensor->getPinConfiguration(), " OK");
			}
			#endif
		}
		
		sensor = (DhtxxSensor*) mContolerSensorList.getNext(sensor);
	}

    return rtc;
}

a291unit::ActionResult a291unit::DhtContoler::onAquire() {

	DhtxxSensor* sensor = (DhtxxSensor*) mContolerSensorList.getFirst();
	while(sensor != NULL) {

		CofigurabelStatus status = sensor->getCurrentStatus(); 
        switch(status) {
            case cs_ready:
                SLOG_WARNING("Sensor ", sensor->getAliasName(), " is not initialized");
                InitializeSensor(sensor);
                break;
                
            case cs_idle:
                readSensor(sensor);
                break;
			
            case cs_sampling:
			case cs_disabled:
				// DHTxx sensors do autosampling, after read need to wait
                break;
			
			default:
                SLOG_WARNING("Sensor ", sensor->getAliasName(), " is ", a291unit::Configurabel::StatusToString(status));
				break;
			
        }
		
		sensor = (DhtxxSensor*) mContolerSensorList.getNext(sensor);
    }
	
	return ar_success;
}

bool a291unit::DhtContoler::sendRequestSignal(uint8_t pinNumber) {
	bool responseOk = true;
    long responseTimeout = DHT_RESPONSE_TIMEOUT * a291unit::DELAY_COUNT_1uS;
    
    // request the sensor data
    bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_delay(DHT_REQUEST_PULS_LOW_TIME_18mS);
    bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_INPT);
	bcm2835_delayMicroseconds(1);

    // wait for sensor start bit
	long responseWait = 0;
	while((bcm2835_gpio_lev(pinNumber) != 0) && (responseWait<responseTimeout)) {
		responseWait++;
	}

	if(responseWait == responseTimeout) {
		responseOk = false;
	}
	else {
		while(bcm2835_gpio_lev(pinNumber) == 0) continue;
	}
	
	return responseOk;
	
}

bool a291unit::DhtContoler::measureDataTiming(a291unit::DhtxxSensor* sensor) {
	bool sensorOk = true;
	uint8_t pinNumber = sensor->getGpioPin(0);
    long pulsTimeout = DHT_BIT_PULS_TIMEOUT * a291unit::DELAY_COUNT_1uS;
    long pCnt = 0;
    const int nbrOfBits = 41;
    int j;
    uint64_t lTime[nbrOfBits];
    uint64_t hTime[nbrOfBits];

    for(int i=0; i<nbrOfBits; i++) {
        lTime[i] = 0;
        hTime[i] = 0;
    }
	
    sendRequestSignal(pinNumber);

    // wait for data start
    int startWait = 0;
    while(bcm2835_gpio_lev(pinNumber) == 1 && startWait < pulsTimeout) startWait++;

	//
	// mesure the bit timing from a read
	//
	uint64_t tb = bcm2835_st_read();
    for(j=0; j<nbrOfBits; j++)
    {
        while(bcm2835_gpio_lev(pinNumber) == 0) continue;
		lTime[j] = bcm2835_st_read() - tb;
		tb = bcm2835_st_read();
        while(bcm2835_gpio_lev(pinNumber) == 1 && pCnt < pulsTimeout) pCnt++;
		pCnt = 0;
		hTime[j] = bcm2835_st_read() - tb;
		tb = bcm2835_st_read();
    }

	//
	// calculate bit read timing
	//
	long maxBitReadTime = (DHT_DATA_START_LOW_TIME + DHT_DATA_VALUE_1_TIME) + (DHT_DATA_START_LOW_TIME + DHT_DATA_VALUE_1_TIME) / 10;
	long hval_0 = 0, hval_1 = 0, cnt_0 = 0, cnt_1 = 0;
	long lval = 0, cnt = 0;
    for(j=0; j<nbrOfBits; j++) {
		if(lTime[j] + hTime[j] < maxBitReadTime) {
			lval += lTime[j];
			cnt++;
			
			if(hTime[j] < (long)(DHT_DATA_VALUE_0_TIME * 1.1)) {
				hval_0 += hTime[j];
				cnt_0++;
			}
			else {
				hval_1 += hTime[j];
				cnt_1++;
			}
		}
	}
	
	uint lowPulesAvg = lval / cnt;
	uint valuePules_0 = hval_0 / cnt_0;
	uint valuePules_1 = hval_1 / cnt_1;

	if(lowPulesAvg > (long)(DHT_DATA_START_LOW_TIME * 0.9) && lowPulesAvg < (long)(DHT_DATA_START_LOW_TIME * 1.1)) {
		#ifdef DEBUG
		SLOG_DEBUG(sensor->getAliasName(), " read bit start pulse OK (", std::to_string(lowPulesAvg), " uSec)");
		#endif
	}
	else {
		SLOG_WARNING(sensor->getAliasName(), " read bit start pulse out of spec (", std::to_string(lowPulesAvg), " uSec)");
	}

	if(valuePules_0 > (long)(DHT_DATA_VALUE_0_TIME * 0.9) && valuePules_0 < (long)(DHT_DATA_VALUE_0_TIME * 1.1) ) {
		#ifdef DEBUG
		SLOG_DEBUG(sensor->getAliasName(), " data bit 0 pulse OK (", std::to_string(valuePules_0), " uSec)");
		#endif
	}
	else {
		SLOG_WARNING(sensor->getAliasName(), " data bit 0 pulse out of spec (", std::to_string(valuePules_0), " uSec)");
	}
	
	if(valuePules_1 > (long)(DHT_DATA_VALUE_1_TIME * 0.9) && valuePules_1 < (long)(DHT_DATA_VALUE_1_TIME * 1.1) ) {
		#ifdef DEBUG
		SLOG_DEBUG(sensor->getAliasName(), " data bit 1 pulse OK (", std::to_string(valuePules_1), " uSec)");
		#endif
	}
	else {
		SLOG_WARNING(sensor->getAliasName(), " data bit 1 pulse out of spec (", std::to_string(valuePules_1), " uSec)");
	}
	

    sleep(DHT_WAIT_AFTER_SAMPLE);
	
	return sensorOk;
}

uint8_t a291unit::DhtContoler::calculateCrc(uint8_t data[], int dataLen) {
    uint8_t crc = 0;
    for(int i=0; i<dataLen; i++) {
        crc += data[i];
    }
    
    return crc;
}

a291unit::ActionResult a291unit::DhtContoler::readSensor(a291unit::DhtxxSensor* dhtSensor) {
	dhtSensor->setCurrentStatus(cs_sampling);
    uint8_t pin = dhtSensor->getGpioPin(0);
	uint readDelay = DHT_DATA_START_LOW_TIME + DHT_DATA_VALUE_0_TIME;
	uint timeOut = readDelay * a291unit::DELAY_COUNT_1uS;
	uint tmoCount;
	uint rxCnt;

	const uint8_t minTime = (uint8_t)(DHT_DATA_VALUE_0_TIME * 0.9);
	const uint8_t maxTime = (uint8_t)(DHT_DATA_VALUE_1_TIME * 1.1);
	const uint8_t valTime = (uint8_t)((DHT_DATA_VALUE_1_TIME + DHT_DATA_VALUE_0_TIME) / 2);
	const uint rawLength = 5;
	uint8_t rawData[rawLength];
	for(int i = 0; i < rawLength; i++ ) {rawData[i] = 0;}
    
    const int nbrOfBit = 40;
    uint8_t rx[nbrOfBit];
    for(int i=0; i<nbrOfBit; i++) rx[i] = 0;    
    uint64_t tb;
	
    ActionResult opRes = ar_success;

	//
	// send read request and receive sensor data
	//
    if(!sendRequestSignal(pin)) {
		SLOG_ERROR("Response timeout ", dhtSensor->getAliasName(), "no signal from sensor");
        opRes = ar_responseTimeout;
    }
    else {
        for(rxCnt=0; rxCnt<=nbrOfBit; rxCnt++) {
			// wait for data start begin, that is a transition from high to low
			tmoCount = 0;
			while((bcm2835_gpio_lev(pin) == 1) && tmoCount < timeOut) tmoCount++;
			
			if(rxCnt > 0) {
				rx[rxCnt-1] = (bcm2835_st_read() - tb) & 0xff;
			}
			
			// wait untill data bit start signal end , signal goes back to heigh
			while(bcm2835_gpio_lev(pin) == 0) continue;
			
			// start of value heigh
			tb = bcm2835_st_read();
		}
    }
    
	//
	// receive done check result
	//
	#ifdef DEBUG
	SLOG_DEBUG("receive done: bit read ", std::to_string(rxCnt - 1));
	#endif
	
	for(int i = 0; i < nbrOfBit; i++) {
		if(rx[i] < minTime || rx[i] > maxTime) {
			// the data read are invalid
			opRes = ar_dataReadError;
			SLOG_WARNING(dhtSensor->getAliasName(), " read fail: excpected 40 bit received ", std::to_string(i));
			SLOG_INFO(dhtSensor->getAliasName(), " bit number ", i, " time was ", (uint)(rx[i]), " uSec");
			break;
		}
		else {
			// process raw data
			uint byte = i / 8;
			uint bit = 8 - (i % 8);
			uint8_t bv = (rx[i] <= valTime) ? 0 : 1 << (bit - 1);
			rawData[byte] |= bv;
		}
	}
	
	if(opRes == ar_success) {
		uint8_t crc = calculateCrc(rawData, 4);
		if(crc != rawData[4]) {
			opRes = ar_crcError;
			SLOG_WARNING("Sensor ", dhtSensor->getAliasName(), " CRC error");
			#ifdef DEBUG
			SLOG_DEBUG("Sensor ", dhtSensor->getAliasName(), " CRC error: received ", (uint)(rawData[4]), " calculated ", (uint)crc);
			for(int i = 0; i < rawLength; i++) {
				SLOG_DEBUG("Byte ", i, ": ", (uint)(rawData[i]));
			}
			#endif
		}
		else {
			#ifdef DEBUG
			SLOG_DEBUG("Sensor ", dhtSensor->getAliasName(), " CRC check ok");
			#endif
		}
	}
	
	update_sensor_data(dhtSensor, rawData, rawLength, opRes);

	return opRes;
}

a291unit::ActionResult a291unit::DhtContoler::InitializeSensor(DhtxxSensor* dhtSensor) {
	ActionResult ar = ar_success;
	
	// initialize GPIO pin
	uint8_t pin = dhtSensor->getGpioPin(0);
	
	SLOG_INFO("Initializing ", dhtSensor->getAliasName(), " on pin ", std::to_string(pin));
	
	if(pin == CFG_INVALID_PIN_ID) {
		SLOG_ERROR("Invalid PIN configuration ", dhtSensor->getAliasName(), " ", dhtSensor->getPinConfiguration());
		dhtSensor->setCurrentStatus(cs_disabled);
		ar = ar_configurationError;
	}
	
	if(dhtSensor->getCurrentStatus() == cs_ready) {
		if(checkGpio(dhtSensor)) {
			bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(pin, HIGH);
			bcm2835_gpio_write(pin, LOW);

			bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
			bcm2835_delayMicroseconds(2);

			// check sensor response timing
			if(mesureRequestSignal(pin)) {
				//response from sensor
				if(measureDataTiming(dhtSensor)) dhtSensor->onInitialize();
			}
			else {
				// no sensor resonse
				SLOG_ERROR("Sensor ", dhtSensor->getAliasName(), " response timeout");
			}
			
			ar = (dhtSensor->getCurrentStatus() == cs_idle) ? ar_success : ar_responseTimeout;
		}
		else {
			dhtSensor->setCurrentStatus(cs_disabled);
			SLOG_ERROR("Sensor ", dhtSensor->getAliasName(), " is not connected to pin", (uint)pin);
			ar = ar_signalError;
		}
	}
	
	return ar;
}

bool a291unit::DhtContoler::mesureRequestSignal(uint8_t pinNumber) {
	bool initPulsOk = true;
    long responseTimeout = 2 * DHT_RESPONSE_TIMEOUT * a291unit::DELAY_COUNT_1uS;
	
	uint64_t tb;
	uint64_t te;
	
    // request the sensor data
    bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_delay(DHT_REQUEST_PULS_LOW_TIME_18mS);
    bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_INPT);
	bcm2835_delayMicroseconds(1);

	// mesure the response time
    tb = bcm2835_st_read();

    // wait for sensor start bit
    long responseWait;
    for(responseWait=0; responseWait<responseTimeout; responseWait++) {
        if(bcm2835_gpio_lev(pinNumber) == 0) break;
    }
	
    te = bcm2835_st_read();
	
	if(responseWait == responseTimeout) {
		SLOG_ERROR(mAliasName, " Failed: Response timeout ", std::to_string(te -tb), "uSec");
		initPulsOk = false;
	}
	#ifdef DEBUG
	else {
		long tm = te -tb;
		SLOG_DEBUG(mAliasName, " OK: Response time ", std::to_string(tm), "uSec"); 
	}
	#endif
	
	// settle sensor before next test
	bcm2835_delay(2000);
	
	// mesure the start signal length
    // request the sensor data
    bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_delay(DHT_REQUEST_PULS_LOW_TIME_18mS);
    bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_INPT);
	bcm2835_delayMicroseconds(1);

    // wait for sensor start bit
    for(responseWait=0; responseWait<responseTimeout; responseWait++) {
        if(bcm2835_gpio_lev(pinNumber) == 0) break;
    }
	
    tb = bcm2835_st_read();
    while(bcm2835_gpio_lev(pinNumber) == 0) continue;
    te = bcm2835_st_read();

	if(responseWait == responseTimeout) {
		SLOG_ERROR(mAliasName, " Failed: Response timeout"); 
		initPulsOk = false;
	}
	else {
		long min = DHT_INIT_PULS_WIDTH - DHT_INIT_PULS_WIDTH / 10;
		long max = DHT_INIT_PULS_WIDTH + DHT_INIT_PULS_WIDTH / 10;
		long tm = te -tb;
		if(tm>= min & tm<= max) {
			#ifdef DEBUG
			SLOG_DEBUG(mAliasName, " OK: Signal time ", std::to_string(tm), "uSec"); 
			#endif
		}
		else {
			SLOG_WARNING(mAliasName, " Response signal time out of range ", std::to_string(tm)); 
		}
	}
	
	// settle sensor before next test
	bcm2835_delay(2000);
	
	return initPulsOk;

}

void a291unit::DhtContoler::update_sensor_data(DhtxxSensor* sensor, uint8_t* data, uint len, ActionResult readResult) {
	int size = sensor->getValueCount();
	float *values = new float[size];

	if(readResult == ar_success) {
		// update values
		if(sensor->getConfigurabelType().compare(CFG_SENSOR_DHT11_OBJ_NAME) == 0) {
			// DHT11 coversion
			values[DHT_HUMIDITY_SENSOR_INDEX] = (float)(data[0]) + (float)(data[1]) / 10.0;
			values[DHT_TEMPERATUR_SENSOR_INDEX] = (float)(data[2]) + (float)(data[3]) / 10.0;
		}
		else {
			// DHT22 conversion
			values[DHT_HUMIDITY_SENSOR_INDEX] = (float)(data[0] << 8 | data[1]) / 10.0;
			values[DHT_TEMPERATUR_SENSOR_INDEX] = (float)(data[2] << 8 | data[3]) / 10.0;
		}

		sensor->updateValues(values, size, readResult);
	}
	else {
		// update read status
		sensor->updateValues(NULL, 0, readResult);
	}
}

bool a291unit::DhtContoler::checkGpio(DhtxxSensor* sensor) {
	bool isConnected = true;
	uint8_t pin = sensor->getGpioPin(0);

	bcm2835_gpio_fsel(pin, BCM2835_GPIO_PUD_OFF);
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
	bcm2835_delayMicroseconds(1);
	
	if(bcm2835_gpio_lev(pin) == HIGH) {
		#ifdef DEBUG
		SLOG_DEBUG(sensor->getAliasName(), " GPIO pin ", uint(pin), " has external pull up resistor");
		#endif
	}
	else {
		SLOG_WARNING(sensor->getAliasName(), " GPIO pin ", uint(pin), " is not connected");
		isConnected = false;
	}
	
	return isConnected;
}

	

