#ifndef CONTROLER_H
#define CONTROLER_H

//#include "sensorDevice.h"
#include "gpioManager.h"
#include "configurabel.h"
#include "configurationList.h"

#include <stdint.h>
#include <iostream>

#define CONTROLER_GPIO_LIST_SIZE 26

class GpioManager;

namespace a291unit {

    /**
     * @class  Controler
     * @author Ernst Müller
     * @date   03/24/21
     * @file   controler.h
     * @brief  This is the base class for all controlers. a controler is the driving part of each configurable.
     */
    class Controler {
		
	/*
	 * Constructor
	 */
	protected:
        Controler(std::string contolerTypeName, uint configurableSize);
		
		
	/*
	 * controler type
	 */
	protected:
        const std::string mControlerTypeName;
		
    public: 
        virtual bool isControlerType(std::string controlerTypeName);
        virtual const std::string& getControlerTypeName() const { return mControlerTypeName; }


	/*
	 * controler alias name
	 */
	protected:
        std::string mAliasName;

	public:
        virtual const std::string& getAliasName() const {return mAliasName;}


	/*
	 * All controlers share the GPIO system.
	 * a Gpio pin is exclusive used by only one controler.
	 * If a controler does not need a GPIO pin then no entry in this list is required.
	 */
	protected:
//		static GpioManager* mGpioControlerAssignement;


	/*
	 * configurable list managed by the controler
	 */
	protected:
		const uint mConfigurableCount;
		ConfigurationList mContolerSensorList;
		uint8_t mGpioPinCount;
		uint8_t mGpioPinList[CONTROLER_GPIO_LIST_SIZE];
		void addGpioPin(uint8_t gpioPinNumber);
		
    public: 
        virtual bool addConfigurabel(Configurabel* configurable) = 0;

		const uint8_t& getGpioPinCount() const {return mGpioPinCount;}
		const uint8_t& getGpioPin(int index) const {return mGpioPinList[index];}

//        virtual bool listConfiguration(std::ofstream &cfg_out) = 0;
		const uint& getConfigurableCount() const { return mConfigurableCount; }
		
		virtual Configurabel* getFirstConfigurabel() { return mContolerSensorList.getFirst(); }
		virtual Configurabel* getNextConfigurabel(Configurabel* nextOf) { return mContolerSensorList.getNext(nextOf); }
		
		virtual void disableConfigurabel(int onGpioPin);
		
//		virtual const int pinList[] getGpioPinList(const int& numberOfPins) = 0;
	/*
	 * Virtual function to be implemented by a child class.
	 * If the child class dos not need to do anything on a specific function id must return ar_success
	 */
    public: 
		/**
		 * @brief All configuration is loaded and assigned to the controler.
		 *        The controler must validate its its operation status and the operation status
		 *        of the assigned configurables.
		 * 
		 * @param globalCfgList -> the Global Configurable list of the system.
		 *                      at this point the list is build, therfore the controler must add all its configurable to this list.
		 *                      if required, the controler may keep the globalCfgList for later use.
		 */
//		virtual ActionResult onCheckConfiguration(ConfigurationList* globalCfgList) = 0;

        /**
         * @brief All configurations have been checked. The controler is to initialize and initialize all its configurable.
         */
        virtual ActionResult onInitialize() = 0;

        /**
         * @brief step 1 of the process loop
		 *        The contoler is to aquire the values of all its configurables
		 * 
         * @return ar_success or appropriate
         */
        virtual ActionResult onAquire() { return ar_success; }
		
        /**
         * @brief step 2 of the process loop, all values are up to date
		 *        The contoler is to process the values of all its configurables
		 * 
         * @return ar_success or appropriate
         */
        virtual ActionResult onProcess() { return ar_success; }
		
        /**
         * @brief step 3 of the process loop, all values are up to date and processing is complete
		 *        The contoler may report any values as required
		 * 
         * @return ar_success or appropriate
         */
        virtual ActionResult onReport() { return ar_success; }
		
		
		/**
		 * @brief the system is shut down. The controler must clean up and free any resources allocated
		 * 
		 * @return ar_success or appropriate
		 */
		virtual ActionResult onTerminate() { return ar_success; }
		
		
    };
}

#endif