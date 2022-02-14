#include "unitManager.h"
#include "a291unitDefinitions.h"
#include "configurabel.h"
#include "dhtControler.h"
#include "dhtxxSensor.h"
#include "wireBusControler.h"
#include "ds18b20Sensor.h"
#include "bme280Sensor.h"
#include "bme280Controler.h"
#include "mqttClient.h"
#include "mqttBroker.h"
#include "slog.h"
#include "gpioInput.h"
#include "mqttOutput.h"
#include "pinIOControler.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <bcm2835.h>
#include <mosquitto.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <typeinfo>

int a291unit::DELAY_COUNT_1uS = 1;

void a291unit::UnitManager::signalHandler(int signal) {
    switch (signal) {
        case SIGTERM: 
			printf("Handle signal SIGTERM\n");
			a291unit::UnitManager::instance().requestTerminate();
            break;
        
        case SIGHUP: 
			printf("Handle signal SIGHUP\n");
			a291unit::UnitManager::instance().requestConfigLoad();
            break;

		case SIGUSR1:
			printf("Handle signal SIGUSR1\n");
			a291unit::UnitManager::instance().requestConfigDump();
            break;		
    }
}

a291unit::ConfigurationList a291unit::UnitManager::mSystemConfigurabelList;

a291unit::UnitManager::UnitManager() {
    signal(SIGTERM, a291unit::UnitManager::signalHandler);
    signal(SIGHUP, a291unit::UnitManager::signalHandler);
    signal(SIGUSR1, a291unit::UnitManager::signalHandler);

	for(int i=0; i<UMA_CONTROLER_RANGE; i++) { mSystemContolerList[i] = NULL; }
	
	mControlerIndex = 0;
	
	op_ConfigDump = false;
	op_ConfigLoad = true;
	op_ConfigReload = false;
	op_run = true;
	
	mTaskControl = tc_loadConfig;
	mCycleControl =cc_Aquire;
	
}

void a291unit::UnitManager::run() {
	um_Start();
	
	while(op_run) {
		um_UpdateTaskControl();
		
		switch(mTaskControl) {
			case tc_loadConfig: 
				um_LoadConfiguration();
				mCycleControl = cc_Aquire;
				break;
			
			case tc_dumpConfig: 
				um_DumpConfig(); 
				mCycleControl = cc_Aquire;
				break;
			
			case tc_runCycle:
				switch(mCycleControl) {
					case cc_Aquire: 
						um_Aquire();
						mCycleControl = cc_Process;
						break;
						
					case cc_Process: 
						um_Process(); 
						mCycleControl = cc_Report;
						break;
						
					case cc_Report: 
						um_Report(); 
						mCycleControl = cc_Aquire;
						break;
				}
			break;
			
			default:
				SLOG_ERROR("Invalid task state termination");
				op_run = false;
				break;
		}
	}
	
	um_Terminate();
}

void a291unit::UnitManager::um_Start() {
    SLOG_INFO("UnitManager initializeing");
    
    if(!bcm2835_init())
    {
        SLOG_ERROR("BCM2835 Init faile");
		op_run = false;
    }

	// try reading time,if the time read is 0 then not runing as root
    uint64_t t = bcm2835_st_read();
	if(t== 0) {
		SLOG_ERROR("NOT ROOT; can not execute in user mode");
		op_run = false;
	}

    // try evaluat 1uS counter value
    uint32_t tLong = UINT32_MAX;
    uint64_t tTime = time(NULL);
    while(tLong > 0) tLong--;
    uint64_t dTime = (time(NULL) - tTime) + 1;
    a291unit::DELAY_COUNT_1uS = UINT32_MAX / 1000000 / dTime;
    
    SLOG_INFO("Evaluation of uSecond tick count: ", a291unit::DELAY_COUNT_1uS, " count per uSecond");
	
}

void a291unit::UnitManager::um_UpdateTaskControl() {
	if(op_ConfigLoad) {
		mTaskControl = tc_loadConfig;
		op_ConfigLoad = false;
	}
	else if(op_ConfigDump) {
		mTaskControl = tc_dumpConfig;
		op_ConfigDump = false;
	}
	else {
		mTaskControl = tc_runCycle;
	}
}

void a291unit::UnitManager::um_DumpConfig() {
    SLOG_INFO("UnitManager: dump configuration");
	std::string dmpFile = mConfigurationFile + ".dump";

	std::ofstream cfg_out;
	cfg_out.open(dmpFile);


    cfg_out << "\n# = = = begin of configuration = = =\n\n";
	
	Configurabel* c = mSystemConfigurabelList.getFirst();
	while(c != NULL) {
		c->listConfiguration(cfg_out);
		c = mSystemConfigurabelList.getNext(c);
	}
        
    cfg_out << "\n# = = = end of configuration = = =\n\n";
	
	cfg_out.close();
	
}


void a291unit::UnitManager::um_Terminate() {
    SLOG_INFO("UnitManager termination cleanup");
	
	Controler* c = getFirstControler();
	while(c != NULL) {
		if(ar_success != c->onTerminate()) {
			SLOG_ERROR(c->getControlerTypeName(), " failed to terminate");
		}	
		
		c = getNextControler();
	}
	
    bcm2835_close();
}

void a291unit::UnitManager::um_Aquire() {
	Controler* c = getFirstControler();
	while(c != NULL ) {
		c->onAquire();
		c = getNextControler();
	}
}

void a291unit::UnitManager::um_Process() {
	Controler* c = getFirstControler();
	while(c != NULL ) {
		c->onProcess();
		c = getNextControler();
	}
}

void a291unit::UnitManager::um_Report() {
	Controler* c = getFirstControler();
	while(c != NULL ) {
		c->onReport();
		c = getNextControler();
	}
}


a291unit::Configurabel* a291unit::UnitManager::createConfigObject(std::string objId, int instanceNumber) {
    std::string name =  objId.substr(1, objId.length()-2);
    Configurabel* cfgObj = NULL;
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
   
    if(0 == name.compare(CFG_MQTT_BROKER_OBJ_NAME)) {
        cfgObj = new MqttBroker(instanceNumber);
    }

    if(0 == name.compare(CFG_SENSOR_DHT11_OBJ_NAME)) {
        cfgObj = new DhtxxSensor(SENSOR_TYPE_DHT11, instanceNumber);
    }

    if(0 == name.compare(CFG_SENSOR_DHT22_OBJ_NAME)) {
        cfgObj = new DhtxxSensor(SENSOR_TYPE_DHT22, instanceNumber);
    }

    if(0 == name.compare(CFG_SENSOR_BME280_OBJ_NAME)) {
        cfgObj = new Bme280Sensor(instanceNumber);
    }

    if(0 == name.compare(CFG_SENSOR_DS18B20_OBJ_NAME)) {
        cfgObj = new Ds18b20Sensor(instanceNumber);
    }

    if(0 == name.compare(CFG_GPIO_INPUT_OBJ_NAME)) {
        cfgObj = new GpioInput(instanceNumber);
    }

    if(0 == name.compare(CFG_MQTT_OUTPUT_OBJ_NAME)) {
        cfgObj = new MqttOutput(instanceNumber);
    }

	if(cfgObj == NULL) {
		SLOG_ERROR("Unknown configuration object ", objId);
	}
    return cfgObj;
}

bool a291unit::UnitManager::addSensorToControler(Configurabel* sensor) {
    bool rtc = false;
    std::string objName = sensor->getControlerType();

    // find the index of a maching controler or an empty space
    int listIndex;
    Controler* controler = NULL;
    for(listIndex=0; listIndex<UMA_CONTROLER_RANGE; listIndex++) {
        if(mSystemContolerList[listIndex] == NULL || mSystemContolerList[listIndex]->isControlerType(sensor->getControlerType())) {
			controler = mSystemContolerList[listIndex];
			break;
		}
    }

	if(controler == NULL) {
		// no controler loadet 
		if(listIndex<UMA_CONTROLER_RANGE) {
			// has slot, add cntroler
			if(sensor->getControlerType().compare(CFG_DHT_CTRL_OBJ_NAME) == 0){
				mSystemContolerList[listIndex] = new DhtContoler();
				SLOG_INFO("Item controler ", CFG_DHT_CTRL_USR_NAME, " addet.");
			}
			
			if(sensor->getControlerType().compare(CFG_WBUS_CTRL_OBJ_NAME) == 0){
				mSystemContolerList[listIndex] = new WireBusControler();
				SLOG_INFO("Item controler ", CFG_WBUS_CTRL_USR_NAME, " addet.");
			}
	   
			if(sensor->getControlerType().compare(CFG_BME280_CTRL_OBJ_NAME) == 0){
				mSystemContolerList[listIndex] = new Bme280Controler();
				SLOG_INFO("Item controler ", CFG_BME280_CTRL_USR_NAME, " addet.");
			}
	   
			if(sensor->getControlerType().compare(CFG_MQTT_CLIENT_OBJ_NAME) == 0){
				mSystemContolerList[listIndex] = new MqttClient();
				SLOG_INFO("Item controler ", CFG_MQTT_CLIENT_USR_NAME, " addet.");
			}
	   
			if(sensor->getControlerType().compare(CFG_PIN_IO_CTRL_OBJ_NAME) == 0){
				mSystemContolerList[listIndex] = new PinIoControler();
				SLOG_INFO("Item controler ", CFG_PIN_IO_CTRL_USR_NAME, " addet.");
			}
	   
	   
			if(mSystemContolerList[listIndex] == NULL) {
				// no controler to support the item
				SLOG_ERROR("Unsupporte configuration item, item_type: ", sensor->getConfigurabelType(), " alias: ", sensor->getAliasName(), " controler_type: ", sensor->getControlerType() );
			}
			
			controler = mSystemContolerList[listIndex];

		}
		else {
			// no slot, error condition
			SLOG_ERROR("Unit Manager Failed to add controler ", sensor->getControlerType(), " maximum numpers of controlers reached");		
		}
	}

	if(controler != NULL) {
		// add the item to the controler
		SLOG_INFO("Item ", sensor->getConfigurabelType(), " alias ", sensor->getAliasName(), " addet to ", sensor->getControlerType());
		rtc = controler->addConfigurabel(sensor);
	}
	else {
		// no controler for the item
		SLOG_ERROR("Unsupporte configuration item, item_type: ", sensor->getConfigurabelType(), " alias: ", sensor->getAliasName(), " controler_type: ", sensor->getControlerType() );
	}
        
    return rtc;
}

void a291unit::UnitManager::initialize() {

	Controler* c = getFirstControler();
	while(c != NULL) {
		if(ar_success != c->onInitialize()) {
			SLOG_ERROR(c->getControlerTypeName(), " failed to initialize");
		}	
		
		c = getNextControler();
	}
    
}

a291unit::Controler* a291unit::UnitManager::getFirstControler() {
	mControlerIndex = 0;
	return getNextControler();
}

a291unit::Controler* a291unit::UnitManager::getNextControler() {
	Controler* c = NULL;
	if(mControlerIndex < UMA_CONTROLER_RANGE) {
		c = mSystemContolerList[mControlerIndex];
		if(c != NULL) mControlerIndex++;
	}
	
	return c;
}

void a291unit::UnitManager::requestTerminate() {
	SLOG_INFO("Termination request received");
	op_run = false;
}

void a291unit::UnitManager::requestConfigLoad() {
	SLOG_INFO("Configuration reload request received");
	op_ConfigLoad = true;
}

void a291unit::UnitManager::cfg_InitializeConfiguration() {
    for(int i=0; i<UMA_CONTROLER_RANGE; i++) {
		if(mSystemContolerList[i] != NULL) {
			mSystemContolerList[i]->onTerminate();
			delete mSystemContolerList[i];
		}
		mSystemContolerList[i] = NULL;
	
	}

	mSystemConfigurabelList.deleteAllContent();
	
	mGpioManager.releaseAllocations();
	
}

void a291unit::UnitManager::um_LoadConfiguration() {
	cfg_InitializeConfiguration();
	cfg_LoadConfigurationFile();
	cfg_StartConfiguration();
	
}

void a291unit::UnitManager::cfg_LoadConfigurationFile() {

	int cfgObjCnt = 0;
    Configurabel* cfg = NULL;
	
    // std::ifstream is RAII, i.e. no need to call close
    std::ifstream cFile (mConfigurationFile);
    if (cFile.is_open()) {
        int lineCnt = 0;
        std::string line;
        while(getline(cFile, line)) {
            lineCnt++;
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

            if(line[0] != '#' && !line.empty()) {
                if((char) line[0] == '[') {
                    // begin config object
                    if(cfg != NULL) {
                        // save the config object before creating a new one
						SLOG_INFO(cfg->getConfigurabelType(), " configuration for ", cfg->getAliasName(), " loadet.");
						cfg->onCheckConfiguration();
						mSystemConfigurabelList.append(cfg);
						cfgObjCnt++;
                    }
                    cfg = createConfigObject(line, cfgObjCnt);
                }
                else {
                    // config parameter
                    auto delimiterPos = line.find("=");
                    std::string name = line.substr(0, delimiterPos);
                    std::string value = line.substr(delimiterPos + 1);
                    if(cfg != NULL) {
                        if(cfg->setConfiguration(name, value) != ar_success) {
                            SLOG_ERROR("Configuration error: ", cfg->getConfigurabelType(), " does not support element named " , name);
                        }

                    }
                }
            }

        }
        
        // save the last config object
        if(cfg != NULL) {
			SLOG_INFO(cfg->getConfigurabelType(), " configuration for ", cfg->getAliasName(), " loadet.");
			cfg->onCheckConfiguration();
			mSystemConfigurabelList.append(cfg);
        }
        
		const int items = mSystemConfigurabelList.getListCount();
		if(items == 0) {
			SLOG_ERROR("No items loadet from ", mConfigurationFile.c_str());
			op_run = false;
		}
		else {
			SLOG_INFO(std::to_string(items), " configurable items loadet from ", mConfigurationFile.c_str() );
		}
    }
    else {
        SLOG_ERROR("Configuration file not found > ", mConfigurationFile.c_str());
		op_run = false;
    }    
	
}

void a291unit::UnitManager::cfg_StartConfiguration() {

	// assign configurables to controlers
	Configurabel* cfg = mSystemConfigurabelList.getFirst();
	while(cfg != NULL) {
		addSensorToControler(cfg);
		cfg = mSystemConfigurabelList.getNext(cfg);
	}
	
	// check GPIO assignments
	Controler* ctr = getFirstControler();
	while(ctr != NULL) {
		for(int i=0; i<ctr->getGpioPinCount(); i++) {
			int pin = ctr->getGpioPin(i);
			if(!mGpioManager.allocateGpioPin(pin, ctr)) {
				// allocation denied
				ctr->disableConfigurabel(ctr->getGpioPin(i));
			}
		}
		ctr = getNextControler();
	}
	
	// log GPIO pin Assignment
	for(int i=0; i<CFG_NUMBER_OF_GPIO_PIN; i++) {
		ctr = (Controler*)mGpioManager.getAllocatedControler(i);
		if(ctr != NULL) {
			SLOG_INFO("Connection assignement: GPIO(", i, ") Connection pin(", a291unit::GpioManager::getConnectionPinNumber(i), ") is allocated to: ", ctr->getAliasName());
		}
	}
	
	
	// initialize controlers
	ctr = getFirstControler();
	while(ctr != NULL) {		
		ctr->onInitialize();
		ctr = getNextControler();
	}
}
