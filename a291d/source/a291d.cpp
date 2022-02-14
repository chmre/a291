#include "a291d.h"
#include "log.h"
#include <unitManager.h>
#include <signal.h>
#include <unistd.h>

A291d::A291d() {
    m_isRunning = true;
    m_reload = false;
    m_terminate = false;
    m_mesure = true;
    
    signal(SIGINT, A291d::signalHandler);
    signal(SIGTERM, A291d::signalHandler);
    signal(SIGHUP, A291d::signalHandler);
    signal(SIGALRM, A291d::signalHandler);
    
    mUnitManager = new a291unit::UnitManager();
}

void A291d::setReloadFunction(std::function<void()> func) {
    m_reloadFunc = func;
}

bool A291d::IsRunning() {
    if (m_reload) {
        m_reload = false;
        m_reloadFunc();
    }
    return m_isRunning;
}

void A291d::signalHandler(int signal) {
    switch (signal) {
        case SIGINT:
        case SIGTERM: {
            LOG_INFO("Termination signal number [", signal, "] recived.");
            A291d::instance().m_terminate = true;
            break;
        }
        
        case SIGHUP: {
            LOG_INFO("Reload signal number [", signal, "] recived.");
            A291d::instance().m_reload = true;
            break;
        }

        case SIGALRM: {
            LOG_INFO("Timer signal number [", signal, "] recived.");
            A291d::instance().m_mesure = true;
            break;
        }
    }
}

void A291d::RunCycle() {
    if(m_terminate) {
        mUnitManager->cleanup();
        m_isRunning = false;
        return;
    }
    
    if(m_reload) { 
        mUnitManager->reloadConfiguration(mConfigFile);
        m_mesure = true;
    }

    if(m_mesure) {
        mUnitManager->readSensor();
        m_mesure = false;
        alarm(60);
    }
}

void A291d::Initialize() {
    mUnitManager->initialize();
    mUnitManager->reloadConfiguration(mConfigFile);
}

