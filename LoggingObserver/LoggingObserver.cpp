#include "LoggingObserver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include "../Order/Order.h"

using namespace std;
namespace fs = std::filesystem;
using WarzoneOrder::TimeUtil;

namespace WarzoneLog {

    /*-------------------------------------- Observer --------------------------------------*/

    Observer::Observer(){}

    /*-------------------------------------- Subject ---------------------------------------*/

    Subject::Subject() = default;

    Subject::~Subject(){
        if(!_observers.empty()){
            for(Observer*& obs : _observers){
                obs = nullptr;
            }
            _observers.clear();
        }
    }


    void Subject::attach(Observer* o){
        if(o != nullptr){
            _observers.push_back(o);
        }
    }

    void Subject::detach(Observer* o){
        _observers.remove(o);
    }

    void Subject::notify(ILoggable* loggable){
        for(Observer* o : _observers){
            if(o != nullptr){
                o->update(loggable);
            }
        }
    }

    /*------------------------------------ LogObserver -------------------------------------*/

    LogObserver::LogObserver() : Observer() {

        //----------------------------- Time & Directory Setup ------------------------------//
        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm localTime{};

        #ifdef _WIN32
            localtime_s(&localTime, &currentTime);
        #else
            localtime_r(&currentTime, &localTime);
        #endif

        string filePath = "../GameLogs/";

        try{
            filesystem::create_directories(filePath);
        }
        catch(const filesystem::filesystem_error& e){
            cerr << "[LogObserver] ERROR: Failed to create directory '" << filePath
                 << "'. Reason: " << e.what() << endl;
            return;
        }

        //----------------------------- Build Timestamped Filename ---------------------------//
        string currentMS = to_string(TimeUtil::getSystemTimeMillis() % 1000);
        string currentNS = to_string(TimeUtil::getSystemTimeNano() % 1000000);

        ostringstream filenameBuilder;
        filenameBuilder << filePath
                        << "Y"  << (localTime.tm_year + 1900)
                        << "_M" << setw(2) << setfill('0') << (localTime.tm_mon + 1)
                        << "_D" << setw(2) << setfill('0') << localTime.tm_mday
                        << "_H" << setw(2) << setfill('0') << localTime.tm_hour
                        << "_M" << setw(2) << setfill('0') << localTime.tm_min
                        << "_S" << setw(2) << setfill('0') << localTime.tm_sec
                        << "_MS" << currentMS << "_NS" << currentNS
                        << "_GAMELOG.txt";

        logFileName = filenameBuilder.str();

        //----------------------------- Create New Log File ----------------------------------//
        ofstream logFile(logFileName);
        if(!logFile.is_open()){
            cerr << "[LogObserver] ERROR: Could not open " << logFileName << " for writing." << endl;
            return;
        }

        logFile << "=============================================\n";
        logFile << "           WARZONE GAME LOG STARTED          \n";
        logFile << "=============================================\n";
        logFile.close();

        cout << "[LogObserver] Logging started in: " << logFileName << endl;
    }


    /**
	* @brief Writes a log entry with timestamp and message from ILoggable.
	* 
	* Each call appends one line to the log file in the format:
	* [HH:MM:SS.mmm] Message
	*/
    void LogObserver::update(ILoggable* loggable){

        if(!loggable){ return; }

        //----------------------------- Current Timestamp ------------------------------------//
        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm localTime{};

        #ifdef _WIN32
            localtime_s(&localTime, &currentTime);
        #else
            localtime_r(&currentTime, &localTime);
        #endif

        string currentMS = to_string(TimeUtil::getSystemTimeMillis() % 1000);

        //----------------------------- Append Log Entry ------------------------------------//
        ofstream logFile(logFileName, ios::app);
        if(!logFile.is_open()){
            cerr << "[LogObserver] ERROR: Could not open " << logFileName << " for appending." << endl;
            return;
        }

        logFile << "[" << setw(2) << setfill('0') << localTime.tm_hour << ":"
                << setw(2) << setfill('0') << localTime.tm_min << ":"
                << setw(2) << setfill('0') << localTime.tm_sec << "."
                << setw(3) << setfill('0') << currentMS << "] "
                << loggable->stringToLog() << endl;

        logFile.close();
    }

}
