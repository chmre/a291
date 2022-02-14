#ifndef DHTCONTROLER_H
#define DHTCONTROLER_H

#include "controler.h"
#include "dhtxxSensor.h"

#include <list>

#define CFG_DHT_CTRL_USR_NAME "DHT Controler"
#define CFG_DHT_CTRL_OBJ_NAME "DHTCONTROLER"
#define CFG_DHT_CTRL_MAX_SENSOR 8


namespace a291unit {
    
    class DhtContoler : public Controler {
	
	protected: // attributes
		GpioManager* mSensorAssignment;
		bcm2835PUDControl mPudControl;
		
	private: 
		// functions
        bool sendRequestSignal(uint8_t pinNumber);
        bool mesureRequestSignal(uint8_t pinNumber);
        bool measureDataTiming(DhtxxSensor* sensor);
        uint8_t calculateCrc(uint8_t data[], int dataLen);        
        ActionResult InitializeSensor(DhtxxSensor * dhtSensor);
        ActionResult readSensor(DhtxxSensor * dhtSensor);
        void update_sensor_data(DhtxxSensor * sensor, uint8_t* data, uint len, ActionResult readResult);
		bool checkGpio(DhtxxSensor * sensor);
    
    public: // functions
        DhtContoler();

		// implement controler functions
        virtual bool addConfigurabel(Configurabel* configurable);
//        virtual bool listConfiguration(std::ofstream &cfg_out);
//		virtual ActionResult onCheckConfiguration(ConfigurationList* globalCfgList);
        virtual ActionResult onInitialize();
        virtual ActionResult onAquire();
    };
    
}

#endif
