#ifndef GPIOINPUT_H
#define GPIOINPUT_H

#include "configurabel.h"

#define GPIO_INPUT_NUMBER_OF_PIN_USED 1
#define GPIO_INPUT_NUMBER_OF_VALUES 1

namespace a291unit {
	
	class GpioInput : public Configurabel {
		
	private:

		
	public:
		GpioInput(int instanceNumber);
		
		// pure virtual functions
		virtual ActionResult setConfiguration(std::string name, std::string value);
		virtual ActionResult listConfiguration(std::ofstream &cfg_out);
		virtual ActionResult onCheckConfiguration();
		virtual ActionResult onInitialize();
		virtual ActionResult onRunCycle() {return ar_success;}
		
		virtual void setMqttReportPending() {} // ignor report pending and handle it in updateValues()
        virtual bool updateValues(const float *sensorValues, const int len, a291unit::ActionResult readResult);
		
	};
	
}


#endif
