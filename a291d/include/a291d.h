#ifndef A291D_H
#define A291D_H

#include <csignal>
#include <functional>
#include <unitManager.h>

class A291d {
   public:
    static A291d& instance() {
        static A291d instance;
        return instance;
    }

    void setReloadFunction(std::function<void()> func);
    
    // MQTT client unit functions
    void Initialize();
    void RunCycle();
    void setConfigFile(const std::string& configFile) {this->mConfigFile = configFile;}

    bool IsRunning();

   private:
    std::function<void()> m_reloadFunc;
    bool m_isRunning;
    bool m_reload;
    bool m_terminate;
    bool m_mesure;
    a291unit::UnitManager* mUnitManager;
    std::string mConfigFile;
    
    A291d();
    A291d(A291d const&) = delete;
    void operator=(A291d const&) = delete;

    void Reload();
        
    static void signalHandler(int signal);
};

#endif  // A291D_H