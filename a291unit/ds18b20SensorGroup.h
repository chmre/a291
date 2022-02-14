#ifndef DS18B20SENSORGROUP_H
#define DS18B20SENSORGROUP_H

#include "configurabel.h"
#include "ds18b20Sensor.h"

#include <stdint.h>

namespace a291unit {

        class Ds18b20SensorGroup {

		private: // attributes
            const uint8_t mPinNbr;
			const unsigned int mListSize;
            Ds18b20Sensor** mSensorList;

			const uint getSensorCount(CofigurabelStatus hasStatus);

            // sensor itterator
            int mSensorCursor;
            Ds18b20Sensor * getSensor(bool first);

        public:
            // constructor
            Ds18b20SensorGroup(uint8_t pinNbr, uint listSize);
            
            Ds18b20Sensor* findSensor(RegistrationNumber* reistrationNumber);
            
            // getter and setter
            const uint8_t& getPinNbr() const {return mPinNbr;}
            
            virtual bool addSensor(Ds18b20Sensor* sensorDevice);
			virtual Ds18b20Sensor* getFirstSensor() { return getSensor(true); };
			virtual Ds18b20Sensor* getNextSensor() { return getSensor(false); };
			
        };
}


#endif