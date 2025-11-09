#include "LoggingObserver.h"
#include <list>
#include <iostream>
#include <fstream>

namespace WarzoneLog {

    //=============================== Observer Class ===============================//

    Observer::Observer() {}

    //=============================== Subject Class ================================//

    Subject::Subject() {
        this->_observers = new std::list<Observer*>();
    }

    Subject::~Subject() {
        delete this->_observers;
    }

    void Subject::attach(Observer* o) {
        this->_observers->push_back(o);
    }

    void Subject::detach(Observer* o) {
        this->_observers->remove(o);
    }

    void Subject::notify(ILoggable* loggable) {
        for(Observer* o : *(this->_observers)){
            o->update(loggable);
        }
    }

    //============================ LogObserver Class ==============================//

    LogObserver::LogObserver() : Observer() {}

    void LogObserver::update(ILoggable* loggable) {
        if (loggable == nullptr)
            return;

        // Append to file
        std::ofstream logFile("gamelog.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << loggable->stringToLog() << std::endl;
            logFile.close();
        } else {
            std::cerr << "[LogObserver] ERROR: Could not open gamelog.txt for writing." << std::endl;
        }
    }
}