#include "controler.h"
#include "gpioManager.h"
#include "configurabel.h"
#include "slog.h"
#include "a291unitDefinitions.h"

//a291unit::GpioManager* a291unit::Controler::mGpioControlerAssignement = new a291unit::GpioManager();

a291unit::Controler::Controler(std::string contolerTypeName, uint configurableSize) : 
mConfigurableCount(configurableSize), mControlerTypeName(contolerTypeName) {

//	mContolerSensorList = new ConfigurationList();
	
	mGpioPinCount = 0;
	for(int i=0; i<CONTROLER_GPIO_LIST_SIZE; i++) { mGpioPinList[i] = CFG_INVALID_PIN_ID; }

}

bool a291unit::Controler::isControlerType(std::string controlerTypeName) {
    return (mControlerTypeName.compare(controlerTypeName) == 0) ? true : false;
}

void a291unit::Controler::addGpioPin(uint8_t gpioPinNumber) {
	// if the pinn is invalid return 
	if(!a291unit::GpioManager::isValidGpioPin(gpioPinNumber)) return;
	
	// check if the pinn is allready in the list
	bool pinListed = false;
	for(int i=0; i<mGpioPinCount; i++) {
		if(mGpioPinList[i] == gpioPinNumber) {
			pinListed = true;
			break;
		}
	}
	
	// if the pin is not in the list add it
	if(!pinListed) {
		mGpioPinList[mGpioPinCount] = gpioPinNumber;
		mGpioPinCount++;
	}
}

void a291unit::Controler::disableConfigurabel(int onGpioPin) {
	// find the configuration having the gpio pin numbar
	Configurabel* c = getFirstConfigurabel();
	while(c != NULL) {
		if(c->hasGpioPin(onGpioPin)) {
			c->setCurrentStatus(cs_disabled);
			SLOG_ERROR("Unit manager disabled configurable ", c->getConfigurabelType(), " alias: ", c->getAliasName(), ". the GPIO PIN is in use");
		}
		
		c = getNextConfigurabel(c);
	}
}
