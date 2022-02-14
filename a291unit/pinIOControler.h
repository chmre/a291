#ifndef PINIOCONTROLER_H
#define PINIOCONTROLER_H

#include "controler.h"
#include "gpioManager.h"

#define CFG_PIN_IO_CTRL_USR_NAME "Pin IO Controler"
#define CFG_PIN_IO_CTRL_OBJ_NAME "PINIOCONTROLER"
#define CFG_PIN_IO_CTRL_MAX_ACTUATOR 8

namespace a291unit {
	
	class PinIoControler : public Controler {
	
	private:
		GpioManager* mSensorAssignment;
		
		// keep the reference to the system width configurable list
		ConfigurationList* mSystemConfigurableList;

	public:
		PinIoControler();
		
        // implement controler virtual functions
        virtual bool addConfigurabel(Configurabel* configurable);
        virtual ActionResult onInitialize();
		virtual ActionResult onTerminate();
        virtual ActionResult onProcess();
		
        virtual ActionResult onAquire();

	};
}

#endif
