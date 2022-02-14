#ifndef DS18B20SENSOR_H
#define DS18B20SENSOR_H

#include "configurabel.h"
#include "registrationNumber.h"

#define DS18B20_NUMBER_OF_PIN_USED 1
#define DS18B20_NUMBER_OF_SENSOR_VALUES 1


namespace a291unit {
    
    class Ds18b20Sensor : public Configurabel {

    private: 
		// atributes
		RegistrationNumber* mRegistrationNumber;
            
	public: // functions
		Ds18b20Sensor(int instanceNumber);
		
		// implement pure virtual function from Configurable
		virtual ActionResult setConfiguration(std::string name, std::string value);
		virtual ActionResult listConfiguration(std::ofstream &cfg_out);
		virtual ActionResult onCheckConfiguration();
		virtual ActionResult onInitialize() { return ar_success; }
		virtual ActionResult onRunCycle() { return ar_success; }
		
		virtual const CofigurabelStatus& getCurrentStatus();
		
            
//            virtual const std::string getControlerObjectName() {return std::string(CFG_WBUS_CTRL_OBJ_NAME);}
		virtual void found() { mCurrentStatus = cs_idle; }

		// getter and setter
		void setRegistrationNumber(RegistrationNumber* registrationNumber) {this->mRegistrationNumber = registrationNumber; mCurrentStatus = cs_ready;}
		RegistrationNumber* getRegistrationNumber() {return mRegistrationNumber;}
		bool isRegistrationNumber(RegistrationNumber* registrationNumber);
            
    };

}  
    


#endif
