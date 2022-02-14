#include "ds18b20SensorGroup.h"
#include "wireBusControler.h"

a291unit::Ds18b20SensorGroup::Ds18b20SensorGroup(uint8_t pinNbr, uint listSize) : mPinNbr(pinNbr), mListSize(listSize) {
	
	mSensorList = new Ds18b20Sensor*[mListSize];
    for(int i=0; i<mListSize; i++) { mSensorList[i] = NULL; }
	
    mSensorCursor = 0;
}

a291unit::Ds18b20Sensor* a291unit::Ds18b20SensorGroup::getSensor(bool first) {
    if(first) mSensorCursor = 0;

    Ds18b20Sensor* sensor = NULL;
    if(mSensorCursor < CFG_WBUS_CTRL_MAX_SENSOR) {
        sensor = mSensorList[mSensorCursor];
        if(sensor != NULL) mSensorCursor++;
    }
    
    return sensor;
}

bool a291unit::Ds18b20SensorGroup::addSensor(Ds18b20Sensor* sensorDevice) {
    int i;
    for(i=0; i<CFG_WBUS_CTRL_MAX_SENSOR; i++)  if(mSensorList[i] == NULL) break;
    
    if(i<CFG_WBUS_CTRL_MAX_SENSOR) {
        mSensorList[i] = sensorDevice;
        return true;
    }
    else {
        // sensor list is full
        return false;
    }
}

a291unit::Ds18b20Sensor* a291unit::Ds18b20SensorGroup::findSensor(RegistrationNumber* reistrationNumber) {
    Ds18b20Sensor* sensor = NULL;
    
    for(int i=0; i<CFG_WBUS_CTRL_MAX_SENSOR; i++)  {
        if(mSensorList[i] == NULL) break;
        
        if(mSensorList[i]->isRegistrationNumber(reistrationNumber)) {
            sensor = mSensorList[i];
            break;
        }
    }
    
    return sensor;
}

const uint a291unit::Ds18b20SensorGroup::getSensorCount(CofigurabelStatus hasStatus) {
	uint cnt = 0;
	Configurabel* sensor = getFirstSensor();
	while(sensor != NULL) {
		if(sensor->getCurrentStatus() == hasStatus) cnt++;
		sensor = getNextSensor();
	}
	
	return cnt;
}
