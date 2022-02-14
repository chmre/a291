#include "pinIOControler.h"
#include "gpioInput.h"
#include "mqttOutput.h"
#include "slog.h"

#include <algorithm>

a291unit::PinIoControler::PinIoControler() :
Controler(CFG_PIN_IO_CTRL_OBJ_NAME, CFG_PIN_IO_CTRL_MAX_ACTUATOR) {
	mSensorAssignment = new GpioManager();
	mAliasName = CFG_PIN_IO_CTRL_USR_NAME;
}

bool a291unit::PinIoControler::addConfigurabel(Configurabel* configurable) {
    bool rtc = false;
    
	if(configurable->getGpioPin(0) == CFG_INVALID_PIN_ID) {
		SLOG_ERROR("Invalid configuration [", configurable->getConfigurabelType(), "] missing mandatory token >", CFG_GPIO_PIN_USR_NAME, "<");
    }
	
	if( (configurable->isConfigurabelTypeName(CFG_GPIO_INPUT_OBJ_NAME)) 
	|| (configurable->isConfigurabelTypeName(CFG_MQTT_OUTPUT_OBJ_NAME))	){		
		if(mContolerSensorList.append(configurable) == NULL) {
			// list is full
            SLOG_ERROR(CFG_PIN_IO_CTRL_USR_NAME, " the sensor list is full");
		}
		else {
			//append the gpio pins of the added sensor
			for(int i=0; i<configurable->getGpioPinCount(); i++) {
				addGpioPin(configurable->getGpioPin(i));
			}
			rtc = true;
		}

	}
	else {
		// invalid sensor type
		SLOG_ERROR(CFG_PIN_IO_CTRL_USR_NAME, " invalid sensor type. ", configurable->getConfigurabelType(), " is not supported");
	}
	
    return rtc;
}
/*
bool a291unit::PinIoControler::listConfiguration(std::ofstream& cfg_out) {
	Configurabel* cfg = mContolerSensorList->getFirst();
	while(cfg != NULL) {
		cfg->listConfiguration(cfg_out);
		cfg = mContolerSensorList->getNext();
	}
	return true;
}

a291unit::ActionResult a291unit::PinIoControler::onCheckConfiguration(ConfigurationList* globalCfgList) {
	ActionResult ar = ar_success;
	mSystemConfigurableList = globalCfgList;
	
	Configurabel* c = getFirstConfigurabel();
	while(c != NULL) {
		uint pin = (uint)c->getConnectionPin(0);
		if(mGpioControlerAssignement->allocateConnectionPin(pin, this)) {
			// ok pin allocated
			c->onCheckConfiguration();
			globalCfgList->append(c);
		}
		else {
			// pin is in use
			c->setCurrentStatus(cs_disabled);
			if(mGpioControlerAssignement->isGpioPin(pin)) {
				Controler* otherControler = (Controler*)mGpioControlerAssignement->getAllocatedControler(pin);
				SLOG_ERROR(c->getAliasName(), " has been disabled. The Connection pin ", pin, " is in use by ", otherControler->getAliasName());
			}
			else {
				SLOG_ERROR(c->getAliasName(), " has been disabled. The Connection pin ", pin, " is not a GPIO PIN");
			}
			ar = ar_configurationError;			
		}
		c = getNextConfigurabel();
	}
	
	return ar;
}
*/

a291unit::ActionResult a291unit::PinIoControler::onInitialize() {
	Configurabel* c = getFirstConfigurabel();
	while(c != NULL) {
		uint8_t pin = c->getGpioPin(0);

		if (c->isConfigurabelTypeName(CFG_GPIO_INPUT_OBJ_NAME)) {
			bcm2835_gpio_pud(BCM2835_GPIO_PUD_UP);
			bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);	
		}
		
		if (c->isConfigurabelTypeName(CFG_MQTT_OUTPUT_OBJ_NAME)) {
			bcm2835_gpio_pud(BCM2835_GPIO_PUD_OFF);
			bcm2835_gpio_clr(pin);
			bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);	
		}
		
		c->onInitialize();
		
		float *values = new float[c->getValueCount()];
		values[0] = (bcm2835_gpio_lev(pin) == LOW) ? 0 : 1;
		c->updateValues(values, c->getValueCount(), ar_success);
		delete values;
		
		
		c = getNextConfigurabel(c);
	}
	return ar_success;
}

a291unit::ActionResult a291unit::PinIoControler::onAquire() {
	
	Configurabel* c = getFirstConfigurabel();
	while(c != NULL) {
//		const int len = c->getValueCount();
		float *values = new float[c->getValueCount()];
		values[0] = (bcm2835_gpio_lev(c->getGpioPin(0)) == LOW) ? 0 : 1;
		c->updateValues(values, c->getValueCount(), ar_success);
		
		delete values;
		
		c = getNextConfigurabel(c);
	}
	
	return ar_success;
}

a291unit::ActionResult a291unit::PinIoControler::onTerminate() {
	#ifdef DEBUG
	SLOG_DEBUG(mAliasName, " onTerminate() called");
	#endif
//	a291unit::Controler::mGpioControlerAssignement->releaseAllConnecionPin(this);

	GpioInput* sensor = (GpioInput*) mContolerSensorList.getFirst();
	while(sensor != NULL) {
		bcm2835_gpio_clr(sensor->getGpioPin(0));
		sensor->~GpioInput();
		sensor = (GpioInput*) mContolerSensorList.getNext(sensor);
	}
//	mContolerSensorList = NULL;

	return ar_success;
}

a291unit::ActionResult a291unit::PinIoControler::onProcess() {
	
	Configurabel* c = getFirstConfigurabel();
	while(c != NULL) {
		if(c->isConfigurabelTypeName(CFG_MQTT_OUTPUT_OBJ_NAME)) {
			MqttOutput* cOut = (MqttOutput*)c;
			if(cOut->changeOutput()) {
				// change output level
				uint8_t pin = cOut->getGpioPin(0);
				if(1 == cOut->outputLevel() ) {
					bcm2835_gpio_set(pin);
				}
				else {
					bcm2835_gpio_clr(pin);
				}

				float *values = new float[cOut->getValueCount()];
				values[0] = (bcm2835_gpio_lev(pin) == LOW) ? 0 : 1;
				c->updateValues(values, cOut->getValueCount(), ar_success);
				delete values;
			}
		}
		c = getNextConfigurabel(c);
		
	}
	
	return ar_success;
}
