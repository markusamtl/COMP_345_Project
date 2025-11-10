#pragma once
#include <list>
#include <string>

namespace WarzoneLog {

    class ILoggable{
        public:
            virtual std::string stringToLog() = 0;
    };

    class Observer{
        public:
            ~Observer() = default;
            virtual void update(ILoggable* loggable) = 0;
        protected:
            Observer();
    };

    class Subject{
        public:
            ~Subject();
            void attach(Observer* o);
            void detach(Observer* o);
            void notify(ILoggable* loggable);
            Subject();
        private:
            std::list<Observer*>* _observers;
    };

    class LogObserver : public Observer {
        public:
            LogObserver();
            void update(ILoggable* loggable) override;
            ~LogObserver() = default;
    };
}