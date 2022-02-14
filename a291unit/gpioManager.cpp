#include "gpioManager.h"
#include "slog.h"

a291unit::GpioManager::GpioManager() {
	for(int i = 0; i < CFG_NUMBER_OF_GPIO_PIN; i++) { mGpioAssignmentList[i] = NULL; }
}

bool a291unit::GpioManager::allocateConnectionPin(int connetorPinNumber, void* owner) {
	int gpioPin = getGpioNumber(connetorPinNumber);
	return allocateGpioPin(gpioPin, owner);
}

bool a291unit::GpioManager::allocateGpioPin(int gpioPinNumber, void* owner) {
	bool granted = false;
	if(gpioPinNumber >= 0 && gpioPinNumber < CFG_NUMBER_OF_GPIO_PIN) {
		
		if(mGpioAssignmentList[gpioPinNumber] == NULL) {
			mGpioAssignmentList[gpioPinNumber] = owner;
			granted = true;
		}
		else {
			if(mGpioAssignmentList[gpioPinNumber] == owner) {
				granted = true;
			}
		} 
	}
	else {
		SLOG_WARNING("GpioManager: the pin number ", gpioPinNumber, " is not a valid GPIO pin number.");
	}
	
	return granted;
}

bool a291unit::GpioManager::allocateConnectionPin(int connetorPinNumber_1, int connetorPinNumber_2, void* owner) {
	bool granted = false;
	
	int gpioPin_1 = getGpioNumber(connetorPinNumber_1);
	int gpioPin_2 = getGpioNumber(connetorPinNumber_2);
	
	return allocateGpioPin(gpioPin_1, gpioPin_2, owner);
}

bool a291unit::GpioManager::allocateGpioPin(int gpioPinNumber_1, int gpioPinNumber_2, void* owner) {
	bool granted = false;

	bool pinValid = true;
	if(gpioPinNumber_1 == gpioPinNumber_2) pinValid = false;
	if(gpioPinNumber_1 < 0 || gpioPinNumber_1 >= CFG_NUMBER_OF_GPIO_PIN) pinValid = false;
	if(gpioPinNumber_2 < 0 || gpioPinNumber_2 >= CFG_NUMBER_OF_GPIO_PIN) pinValid = false;
	
	
	
	if((pinValid) 
	&& (mGpioAssignmentList[gpioPinNumber_1] == NULL)
	&& (mGpioAssignmentList[gpioPinNumber_2] == NULL) ) {
		granted = true;
		mGpioAssignmentList[gpioPinNumber_1] = owner;
		mGpioAssignmentList[gpioPinNumber_2] = owner;
	}
	else {	
		if((mGpioAssignmentList[gpioPinNumber_1] != owner)
		|| (mGpioAssignmentList[gpioPinNumber_2] != owner) ){
				granted = false;
		}
	}
	
	return granted;
}


void* a291unit::GpioManager::getAllocatedControler(int gpioNumber) {
	if(isGpioPin(gpioNumber)) {
		return mGpioAssignmentList[gpioNumber];
	}
	
	return NULL;
}

int a291unit::GpioManager::getGpioNumber(unsigned int connectionPinNumber) {
	int gpioPinNbr = -1;
	for(int i=0; i < CFG_NUMBER_OF_GPIO_PIN; i++) {
		if(GpioNbr_to_PinNbr[i] == connectionPinNumber) {
			gpioPinNbr = i;
			break;
		}
	}
	
	return gpioPinNbr;
}

bool a291unit::GpioManager::isGpioPin(int connetorPinNumber) {
	uint gpioPin = getGpioNumber(connetorPinNumber);
	return (gpioPin < 0) ? false : true;
}


void a291unit::GpioManager::releaseAllocations() {
	for(int i = 0; i < CFG_NUMBER_OF_GPIO_PIN; i++) { 
		mGpioAssignmentList[i] = NULL;
	}
}

void a291unit::GpioManager::releaseAllocations(void* owner) {
	for(int i = 0; i < CFG_NUMBER_OF_GPIO_PIN; i++) { 
		if(mGpioAssignmentList[i] == owner) mGpioAssignmentList[i] = NULL; 
	}
}

void a291unit::GpioManager::releaseAllocations(int gpioNumber) {
	if(isGpioPin(gpioNumber)) {
		mGpioAssignmentList[gpioNumber] = NULL;
	}
}

/**
 * @brief checks if there is a pull up resistor on the GPIO pin.
 *        if no pull up is present then sets the chip internal pullup. 
 * @param gpioNumber: the GPIO number
 * @return true:  if an external pull up is present
 *         false: the internal pullup is used
 */
bool a291unit::GpioManager::setPullup(int gpioNumber) {
	bool isExternal = true;
	const uint8_t pin = a291unit::GpioManager::getConnectionPinNumber(gpioNumber);
	
	bcm2835_gpio_set_pud(gpioNumber, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_fsel(gpioNumber, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(gpioNumber, LOW);

	bcm2835_gpio_fsel(gpioNumber, BCM2835_GPIO_FSEL_INPT);
	bcm2835_delayMicroseconds(200);
	
	if(bcm2835_gpio_lev(gpioNumber) == 0) {
		bcm2835_gpio_set_pud(gpioNumber, BCM2835_GPIO_PUD_UP);
		isExternal = false;
	}
	
	return isExternal;
}
