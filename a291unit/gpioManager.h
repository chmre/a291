#ifndef GPIOMANAGER_H
#define GPIOMANAGER_H

#include "controler.h"

#include <unistd.h>

// there are 27 GPIO PINS from 0 to 26
#define CFG_NUMBER_OF_GPIO_PIN 27 


namespace a291unit {
	
	static const unsigned int GpioNbr_to_PinNbr[] = {
	27, 28,  3,  5,  7, 29, 31, 26, 24,
	21, 19, 23, 32, 33,  8, 10, 36, 11,
	12, 35, 38, 40, 15, 16,	18, 22, 37 };

	
	class GpioManager {
	private:
		
		void* mGpioAssignmentList[CFG_NUMBER_OF_GPIO_PIN];
		
	public:
		static int getConnectionPinNumber(unsigned int GpioPinNumber) { return (GpioPinNumber > CFG_NUMBER_OF_GPIO_PIN) ? -1 : GpioNbr_to_PinNbr[GpioPinNumber]; }
		static int getGpioNumber(unsigned int connectionPinNumber);
		static bool isValidGpioPin(int gpioNumber) { return (gpioNumber >= 0 && gpioNumber < CFG_NUMBER_OF_GPIO_PIN) ? true : false; }
		static bool setPullup(int gpioNumber);
		
		GpioManager();
		bool allocateConnectionPin(int connetorPinNumber, void* owner);
		bool allocateGpioPin(int gpioPinNumber, void* owner);
		
		bool allocateConnectionPin(int connetorPinNumber_1, int connetorPinNumber_2, void* owner);
		bool allocateGpioPin(int gpioPinNumber_1, int gpioPinNumber_2, void* owner);

		
		void releaseAllocations();
		void releaseAllocations(void* owner);
		void releaseAllocations(int gpioNumber);

		static bool isGpioPin(int connetorPinNumber);
		
		void* getAllocatedControler(int gpioNumber);
		
	};
}

#endif