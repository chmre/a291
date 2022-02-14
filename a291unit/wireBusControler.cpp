#include "wireBusControler.h"
#include "slog.h"
#include "gpioManager.h"
#include "configurabel.h"
#include "unitManager.h"

#include <bcm2835.h>
#include <unistd.h>

a291unit::WireBusControler::WireBusControler() : 
Controler(CFG_WBUS_CTRL_OBJ_NAME, 0 ) {
	
    for(int i=0; i<CFG_WBUS_CTRL_MAX_GROUP; i++) mSensorGroupList[i] = NULL;
    
    mSensorGroupIterationIndex = 0;
	newGroup = true;
	
	mAliasName = CFG_WBUS_CTRL_USR_NAME;
}

a291unit::RegistrationNumber* a291unit::WireBusControler::scanWireBus(uint8_t gpioNbr) {
	const uint8_t connectionPin = a291unit::GpioManager::getConnectionPinNumber(gpioNbr);

    // initialize
	uint8_t id[OWC_ADRESS_BYTE_SIZE];
	for(int i=0; i<OWC_ADRESS_BYTE_SIZE; i++) { id[i] = 0; }
	
	uint idMask;
	
	int addressBit = 0;
	int bitIndex = 0;
    int byteIndex = 0;
	int lastBranchPos = 0;
	
    uint8_t scanDirection;
	
    uint8_t value;
    uint8_t complement;
   
    RegistrationNumber* registrationNbr = NULL;

	#ifdef DEBUG
	SLOG_DEBUG(getAliasName(), "; scan loop: ", scanCount);
	#endif

    if(!initPuls(gpioNbr)) {
        // no sensor responded
        SLOG_ERROR(getAliasName(), ": no device response on GPIO(", (int)gpioNbr, ") connection pin(", (int)connectionPin, ")");
		mScanIsActive = false;
    }
    else {
        // start device rom scan
        writeByte(gpioNbr, OWC_ROM_CMD_SEARCH);
        
        // rom bit read loop
        do {
			bitIndex = addressBit % 8;
			byteIndex = addressBit / 8;

            // read a bit and its complement
            value = readBit(gpioNbr);
            complement = readBit(gpioNbr);

			// evaluate the scan direction
			// the scan direction is the value to traverse on
			if(value == 0) {
				// adress value is 0
				if(complement == 0) {
					lastBranchPos = addressBit;
					
					// device forke; there is one device having 0 and another device having 1 at this address position.
					#ifdef DEBUG
					SLOG_DEBUG("Fork condition; address bit:", addressBit,
								" value:", (int) value,
								" complement:", (int) complement,
							    " last branche position:", mScanBranch,
							    " last branche direction:", mScanDirection);
					#endif

					// check to continue on the 0 or the 1 path
					// mScanLast1Branch: this is the fork position of the last discrepancy that was continued on the 1 direction
					// mScanLast0Branch: this is the fork position of the last discrepancy that was continued on the 0 direction
					
					// if(currentAddressPosition < mScanLast1Branch) 
					// then all discrepancy continue on path 1
					if(addressBit < mScanBranch) { 
						scanDirection = 1; 
					}
					
					// if(currentAddressPosition == mScanCurrentBranch)
					// this discrepancy was last handled before as direction 0,
					// this time it it has to be handeled as direction 1
					if(addressBit == mScanBranch) { 
						if(mScanDirection == 1) {
							scanDirection = 0;
							mScanDirection = 0;
						} else {
							scanDirection = 1;
							mScanDirection = 1;
						}
					}
					
					// if(currentAddressPosition > mScanLast1Branch)
					// this is a future branch all of them continue direction 0
					if(addressBit > mScanBranch) { 
						scanDirection = 0;
						// if the last branche was left direction 1 then this position is the next branch point
						if(mScanDirection == 1) { 
							mScanDirection = 0;
							mScanBranch = addressBit;
						}
					}

					#ifdef DEBUG
					SLOG_DEBUG("Fork result; address bit:", addressBit,
								" scanDirection:", (int) scanDirection,
								" new branche position:", mScanBranch,
								" mew branche direction:", mScanDirection);
					#endif
				} else {
					// all devices have a 0 ath this bit position
					scanDirection = 0;
				}
			
			} else {
				// at this point value == 1
				if(complement == 0) {
					// all devices have a 1 ath this bit position
					scanDirection = 1;
				} else {
					// error condition, no response from device
					SLOG_ERROR(getAliasName(), ": Scan read complement error on GPIO(", (int)gpioNbr, ") connection pin(", (int)connectionPin, ") bit number " , (int)addressBit);
					mScanIsActive = false;
					break;
				}
			}
						
			idMask = scanDirection << bitIndex;
			id[byteIndex] |= idMask;

			addressBit++;

            writeBit(gpioNbr, scanDirection);
			bcm2835_delay(OWC_RECOVERY_TIME);

		} while(addressBit < OWC_ADRESS_BIT_SIZE); // loop trough 8byte (64bit)

		if(lastBranchPos == mScanBranch & mScanDirection == 1) { mScanIsActive = false; }
		
		if(addressBit == OWC_ADRESS_BIT_SIZE) { registrationNbr = new RegistrationNumber(id); }
	}	
	
	#ifdef DEBUG
	scanCount++;
	#endif
			
	
	return registrationNbr;
	
}

uint8_t a291unit::WireBusControler::readBit(uint8_t pinNumber) {
    uint8_t b;
    int requestTime = OWC_BIT_READ_REQ_LOW_TIME;
    int readWait = OWC_BIT_READ_DATA_VALID_TIME;
    int idleTime = OWC_TIME_SLOT - readWait;

    bcm2835_gpio_write(pinNumber, HIGH);

	bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(pinNumber, LOW);
	bcm2835_delayMicroseconds(requestTime);
	bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_INPT);
	bcm2835_delayMicroseconds(readWait);
	b = bcm2835_gpio_lev(pinNumber);
	bcm2835_delayMicroseconds(idleTime);
    
    return b;
}

uint8_t a291unit::WireBusControler::readBit(uint8_t pinNumber, int nbrOfBits) {
    uint8_t byte = 0;
    
    for(int i=0; i<nbrOfBits; i++) {
        uint b = readBit(pinNumber);
        byte = byte | b << i;
    }
    
    return byte;
}

uint8_t a291unit::WireBusControler::readByte(uint8_t pinNumber) {
    uint8_t byte = 0;
    
    for(int i=0; i<8; i++) {
        uint b = readBit(pinNumber);
        byte = byte | b << i;
    }
    
    return byte;
}

void a291unit::WireBusControler::writeBit(uint8_t pinNumber, uint8_t bitValue) {
    int timeLow = (bitValue == 0) ? OWC_BIT_WRITE_0_LOW_TIME : OWC_BIT_WRITE_1_LOW_TIME;
    int timeHigh = OWC_TIME_SLOT - timeLow;

    bcm2835_gpio_write(pinNumber, HIGH);
    
	bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(pinNumber, LOW);
	bcm2835_delayMicroseconds(timeLow);
	bcm2835_gpio_fsel(pinNumber, BCM2835_GPIO_FSEL_INPT);
	bcm2835_delayMicroseconds(timeHigh);    
}

void a291unit::WireBusControler::writeByte(uint8_t pinNumber, uint8_t databyte) {
    for(int i=0; i<8; i++) {
        if(databyte & 1) 
        {
            writeBit(pinNumber, 1);
        }
        else
        {
            writeBit(pinNumber, 0);
        }

        databyte = databyte >> 1;
    }
}

/**
 * @brief send a low puls on the GPIO and checks the response on the same GPIO.
 * @param gpioNbr: the GPIO Number
 * @return true if the is a response; false if ther is no response 
 */
bool a291unit::WireBusControler::initPuls(uint8_t gpioNbr) {
    long timeout = 2 * OWC_RESET_TIME_LOW * a291unit::DELAY_COUNT_1uS;
	
    bcm2835_gpio_write(gpioNbr, HIGH);
    bcm2835_gpio_fsel(gpioNbr, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(gpioNbr, LOW);
    bcm2835_delayMicroseconds(OWC_RESET_TIME_LOW);
    bcm2835_gpio_fsel(gpioNbr, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(2);
	
	while(bcm2835_gpio_lev(gpioNbr) == HIGH & timeout > 0) {
		timeout--;
	}
	
	while(bcm2835_gpio_lev(gpioNbr) == LOW);
    bcm2835_delay(2);

	return (timeout == 0) ? false : true;
}

bool a291unit::WireBusControler::addConfigurabel(Configurabel* configurable) {
    bool rtc = false;
    
    if(configurable->isConfigurabelTypeName(CFG_SENSOR_DS18B20_OBJ_NAME)) {
        Ds18b20SensorGroup* sensorGroup = getSensorGroup(configurable->getGpioPin(0));
        rtc = sensorGroup->addSensor((Ds18b20Sensor*)configurable);

		//append the gpio pins of the added sensor
		addGpioPin(configurable->getGpioPin(0));

    }
    else {
        // invalid sensor type
        SLOG_ERROR(CFG_WBUS_CTRL_USR_NAME, " invalid sensor type -> ", configurable->getConfigurabelType());
    }
    
    return rtc;
}

a291unit::ActionResult a291unit::WireBusControler::onInitialize() {
    int delayCnt = a291unit::DELAY_COUNT_1uS;
	ActionResult ar = ar_success;
	
	//
	// initialize the GPIO pin for each sensor group
	// each group has 0 or more sensors
	// each sensor in a group has the same GPIO pin
	//
    for(int i=0; i<CFG_WBUS_CTRL_MAX_GROUP; i++) {
        if(mSensorGroupList[i] == NULL) break;
		initializeGroup(mSensorGroupList[i]);
		scaneForDevices(mSensorGroupList[i]);
    }
    
    return ar;
}

a291unit::ActionResult a291unit::WireBusControler::onAquire() {
    
	Ds18b20Sensor* sensor = (Ds18b20Sensor*) getFirstConfigurabel();
	while(sensor != NULL) {
		// do sensor 
		if(sensor->getCurrentStatus() == cs_idle) { 
			readTemperature(sensor);
			sensor->setCurrentStatus(cs_sampling);
		}

		sensor = (Ds18b20Sensor*) getNextConfigurabel();
	}
		

	return ar_success;
}

a291unit::Ds18b20SensorGroup* a291unit::WireBusControler::getSensorGroup(uint8_t gpioPin) {
    int i;
    for(i=0; i<CFG_WBUS_CTRL_MAX_GROUP; i++) {
        if((mSensorGroupList[i] == NULL) || (mSensorGroupList[i]->getPinNbr() == gpioPin)) break;
    }
    
    if(mSensorGroupList[i] == NULL && i<CFG_WBUS_CTRL_MAX_GROUP) {
        mSensorGroupList[i] = new Ds18b20SensorGroup(gpioPin, CFG_WBUS_CTRL_MAX_SENSOR);
    }
    
    return (i<CFG_WBUS_CTRL_MAX_GROUP) ? mSensorGroupList[i] : NULL;
}

int a291unit::WireBusControler::pullScratchpad(uint8_t pinNumber, uint8_t* data, uint8_t len) {
    for(int i=0; i<len; i++) {data[i] = 0;}
    
    // issue function command
    writeByte(pinNumber, OWC_FNC_CMD_READ_SCRATCHPAD);
    
    // readback from scratchpad
    for(int i=0; i<9; i++) {
        data[i] = readByte(pinNumber);
    }
    
    return 0;
}

int a291unit::WireBusControler::selectDevice(Ds18b20Sensor* sensor) {
    uint8_t pinNumber = sensor->getGpioPin(0);
    const uint8_t* regId = sensor->getRegistrationNumber()->getRegistrationId();
    
    initPuls(pinNumber);
    
    writeByte(pinNumber, OWC_ROM_CMD_MATCH);
    for(int i=0; i<8; i++) { writeByte(pinNumber, regId[i]);}
    
    return 0;
}

bool a291unit::WireBusControler::isExternalPowerd(Ds18b20Sensor* sensor) {
    uint8_t pinNumber = sensor->getGpioPin(0);

    selectDevice(sensor);

    // request sensor to report power source
    writeByte(pinNumber, OWC_FNC_CMD_READ_POWER_SUPPLY);

    int delayCnt = a291unit::DELAY_COUNT_1uS;
    while(delayCnt>0) {delayCnt--;}
    bool external = (readBit(pinNumber) == 1) ? true : false;
    
    return external;    
}

a291unit::SensorOperationResult a291unit::WireBusControler::readTemperature(Ds18b20Sensor* sensor) {
    uint8_t pinNumber = sensor->getGpioPin(0);

    selectDevice(sensor);

    // request sensor to mesure temeratur
    writeByte(pinNumber, OWC_FNC_CMD_CONVERT);
    
    int settle = a291unit::DELAY_COUNT_1uS;
    while(settle>0) {settle--;}
    
    // wait for the sensor to compleete the mesurment
    int i;
    const long checkDelay = DS18B_CONVERT_CHECK_mS * 1000 * a291unit::DELAY_COUNT_1uS;
    for(i=0; i<=DS18B_CONVERT_LOOP; i++) {
        long delayCnt = checkDelay;
        while(delayCnt>0) {delayCnt--;}
        if(readBit(pinNumber) == 1) break;
    }

    if(i > 0 && i < DS18B_CONVERT_LOOP) {
        const int dataLen = 9;
        uint8_t data[dataLen];
        for(int i=0; i<9; i++) {data[i] = 0;}

        // converting completet
        selectDevice(sensor);
        pullScratchpad(pinNumber, data, dataLen);
		set_sensor_data(sensor, data, dataLen);
        return success;
    }
    else {
        // converting failed
        return dataReadError;
    }
}

void a291unit::WireBusControler::set_sensor_data(Ds18b20Sensor* sensor, const uint8_t rawData[9], const int rawDataLen) {
    uint8_t crc = 0;
    for(int i=0; i<rawDataLen; i++) {
        crc = a291unit::ds_crc_add(crc, rawData[i]);
    }
    
    if(crc == 0) {
        // valid data
        int t1 = rawData[0];
        int t2 = rawData[1];
        int16_t templ = (t2 << 8 | t1);
		float val = (float)templ / 16;
		sensor->updateValues(&val, 1, ar_success);
    }
    else {
        // invalid data
		sensor->updateValues(NULL, 0, ar_crcError);
    }
}

int a291unit::WireBusControler::initializeGroup(Ds18b20SensorGroup* sensorGroup) {
	int rtc = URTC_SUCCESS;

	const uint8_t gpio = sensorGroup->getPinNbr();
	const int pin = a291unit::GpioManager::getConnectionPinNumber(gpio);
	if(a291unit::GpioManager::setPullup(gpio)) {
		SLOG_INFO(getAliasName(), ": GPIO(", (int) gpio, ") Connection pin(", pin, ") external pullup resistor detected");
	} else {
		SLOG_INFO(getAliasName(), ": GPIO(", (int) gpio, ") Connection pin(", pin, ") internal pullup resistor enabled");
	}
	
	return rtc;
}

a291unit::Configurabel* a291unit::WireBusControler::getFirstConfigurabel() {
	mSensorGroupIterationIndex = 0;
	newGroup = true;
	return getNextConfigurabel();
}

a291unit::Configurabel* a291unit::WireBusControler::getNextConfigurabel() {
	Configurabel* c = NULL;

	// check the index is valid and a sensor group at the current index exists
	if(mSensorGroupIterationIndex < CFG_WBUS_CTRL_MAX_GROUP && mSensorGroupList[mSensorGroupIterationIndex] != NULL) {
		if(newGroup) {
			c = mSensorGroupList[mSensorGroupIterationIndex]->getFirstSensor();
			newGroup = false;
		}
		else {
			c = mSensorGroupList[mSensorGroupIterationIndex]->getNextSensor();
		}
		
		// if there is no sensor, then get the first sensor from the next group
		if(c == NULL) {
			mSensorGroupIterationIndex++;
			if(mSensorGroupIterationIndex < CFG_WBUS_CTRL_MAX_GROUP && mSensorGroupList[mSensorGroupIterationIndex] != NULL) {
				c = mSensorGroupList[mSensorGroupIterationIndex]->getFirstSensor();
			}
		}
	}
	
	return c;
}

int a291unit::WireBusControler::scaneForDevices(Ds18b20SensorGroup* sensorGroup) {
	int cnt = 0;

	const uint8_t gpioNumber = sensorGroup->getPinNbr();
	const uint8_t pinNumber = a291unit::GpioManager::getConnectionPinNumber(gpioNumber);
	
	SLOG_INFO(getAliasName(), ": Begin device scan on GPIO(", (int)gpioNumber, ") connection pin(", (int)pinNumber, ")" );

	// initialize the device scan
	#ifdef DEBUG
	scanCount = 1;
	#endif
	
	mScanBranch = 0;
	mScanDirection = 1;
	
	mScanIsActive = true;

	RegistrationNumber* rNbr = NULL;
	
	while(mScanIsActive) {
		rNbr = scanWireBus(gpioNumber);
		
		if(rNbr != NULL) {
			cnt++;
			
			Ds18b20Sensor* sensor = sensorGroup->findSensor(rNbr);
			if(sensor == NULL) {
				// no sensor configuration with this id is configured
				int CfgNbr = a291unit::UnitManager::getConfigurabelCount() + 1;
				sensor = new Ds18b20Sensor(CfgNbr);
				sensor->setRegistrationNumber(rNbr);
				sensor->setAliasName(rNbr->getFamilyName() + "/_" + std::to_string(CfgNbr));
				std::string conPinNbr = std::to_string(a291unit::GpioManager::getConnectionPinNumber(gpioNumber));
				sensor->setPinConfiguration(conPinNbr);
				sensor->setCurrentStatus(cs_idle);
				sensorGroup->addSensor(sensor);
				a291unit::UnitManager::appendConfigurabel(sensor);
				SLOG_WARNING(getAliasName(), ": Found unconfigured device on GPIO(", (int)gpioNumber, ") connection pin(", (int)pinNumber, ") type: ", rNbr->getFamilyName(), " id: ", rNbr->getRegistrationString());
				SLOG_INFO(getAliasName(), ": added sensor ", sensor->getAliasName(), " id: ", rNbr->getRegistrationString());
			}
			else {
				// sensor verified
				SLOG_INFO(getAliasName(), ": Found ", sensor->getAliasName()," on GPIO(", (int)gpioNumber,  ") connection pin( ",(int)pinNumber, ") having id: ", rNbr->getRegistrationString());
				sensor->found();
			}
			
		}
		
	}

	SLOG_INFO(getAliasName(), ": End of device scan on GPIO(", (int)gpioNumber, ") connection pin(", (int)pinNumber, ")" );
	
	return cnt;
}

