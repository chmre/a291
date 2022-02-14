#ifndef GPIOOUTPUT_H
#define GPIOOUTPUT_H

#include "configurabel.h"

#define GPIO_OUTPUT_NUMBER_OF_PIN_USED 1
#define GPIO_OUTPUT_NUMBER_OF_VALUES 1
#define GPIO_OUTPUT_NUMBER_OF_CONTROLS 1

namespace a291unit {
	
/**
 * @class 	GpioOutput
 * @author 	Müller Ernst
 * @date 	03/22/21
 * @file 	gpioOutput.h
 * @brief 	This class represents an MQTT controled digital output.
 * 			It can be used to control a relayoor other based on an MQTT message.
 */
	class MqttOutput : public Configurabel {
		
	private:
		bool mOutputChangePending;
		
	public:
		MqttOutput(int instanceNumber);
		
		// pure virtual functions
		virtual ActionResult setConfiguration(std::string name, std::string value);
		virtual ActionResult listConfiguration(std::ofstream &cfg_out);
		virtual ActionResult onCheckConfiguration();
		virtual ActionResult onInitialize();
		virtual ActionResult onRunCycle() {return ar_success;}
		
		virtual void setMqttSubscriptionValue(std::string subscriptionAlias, float toValue);
		virtual void setMqttReportPending() {}
		virtual MqttReportStatus getMqttReportStatus();
		
		// special output
		const bool changeOutput();
		const uint8_t outputLevel() { return (mSubscriptionValueList[0] == 0.0) ? 0 : 1; }
	};
}

#endif
