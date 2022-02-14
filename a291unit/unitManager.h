#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include "controler.h"
#include "configurationList.h"
#include "gpioManager.h"

#define UMA_CONTROLER_RANGE 16


namespace a291unit {

    class UnitManager {
		
    private:
		enum TaskControl {
			tc_loadConfig,
			tc_dumpConfig,
			tc_runCycle,
		};
		TaskControl mTaskControl;
		
		enum CycleControl {
			cc_Aquire,
			cc_Process,
			cc_Report,
		};
		CycleControl mCycleControl;
		
		// GPIO allocations
		a291unit::GpioManager mGpioManager;
		
		// special operations
		bool op_ConfigLoad;
		bool op_ConfigReload;
		bool op_ConfigDump;
		bool op_run;
		
		// unit manager core functions
		void um_Start();
		void um_Terminate();
		void um_UpdateTaskControl();
		void um_LoadConfiguration();
		void um_DumpConfig();
		void um_Aquire();
		void um_Process();
		void um_Report();
 		
		// unit manager configuration functions
		void cfg_InitializeConfiguration();
		void cfg_LoadConfigurationFile();
		void cfg_StartConfiguration();

        // configuration helper functions
		void cfg_loadFromFile();
        Configurabel* createConfigObject(std::string objId, int instanceNumber);
        bool addSensorToControler(Configurabel* sensor);
		
		// configuration file
		std::string mConfigurationFile;
		
		// list of all configurables
		static ConfigurationList mSystemConfigurabelList;
				
        // attributes
		int mControlerIndex;
        Controler* mSystemContolerList[UMA_CONTROLER_RANGE];
		
        void initialize();
            
        Controler* getFirstControler();
        Controler* getNextControler();

        // constructor
        UnitManager();
		UnitManager(UnitManager const&) = delete;
		void operator=(UnitManager const&) = delete;
		
    public: 
		static UnitManager& instance() {
			static UnitManager instance;
			return instance;
		}
	
		static void signalHandler(int signal);
	
		static Configurabel* getFirstConfigurable() {return mSystemConfigurabelList.getFirst(); }
		static Configurabel* getNextConfigurable(Configurabel* nextOf) {return mSystemConfigurabelList.getNext(nextOf); }
		static void appendConfigurabel(Configurabel* newConfigurabel) { mSystemConfigurabelList.append(newConfigurabel);}
		static int getConfigurabelCount() {return mSystemConfigurabelList.getListCount();}
		// controles
		void setConfigurationFile(const std::string& loadFile) { this->mConfigurationFile = loadFile;}
		void requestConfigDump() { this->op_ConfigDump = true;}
		void requestConfigLoad();
		void requestTerminate();
		
        // functions
        void run();
            
    };
}

#endif
