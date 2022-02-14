#include <chrono>
#include <iostream>
#include <libconfig.h++>
#include <thread>

#include "command-line-parser.h"
//#include "a291d.h"
#include "slog.h"
#include "unitManager.h"

// This function will be called when the a291 daemon receive a SIGHUP signal.
void reload() {
    SLOG_INFO("Reload function called.");
}

int main(int argc, char** argv) {
    std::string configFileName = "";;
    
    // Command line and config file example
    CommandLineParser commandLine(argc, argv);
    if (commandLine.cmdOptionExist("--config")) {
        configFileName = commandLine.getCmdOptionValue("--config");
        SLOG_INFO("Config file name = ", configFileName);
    }
    
    // The Daemon class is a singleton to avoid be instantiate more than once
    a291unit::UnitManager& unitManager = a291unit::UnitManager::instance();

    // Set the reload function to be called in case of receiving a SIGHUP signal
//    a291d.setReloadFunction(reload);

    // initialize service functionality
    unitManager.setConfigurationFile(configFileName);
//    a291d.Initialize();
    
    // A291d main loop
	unitManager.run();
//    while (a291d.IsRunning()) {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        a291d.RunCycle();
//    }

    SLOG_INFO("The a291d daemon process ended gracefully.");
}