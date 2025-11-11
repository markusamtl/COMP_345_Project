#pragma once
#include <list>
#include <string>

namespace WarzoneLog {

    /**
	* @brief Interface for loggable objects that can produce log strings.
	* 
	* Classes implementing this interface define how their state
	* or actions should be represented as text for logging purposes.
	*/
	class ILoggable {
	public:
		virtual ~ILoggable() = default;

		/**
		* @brief Returns a textual representation of the object for logging.
		* 
		* @return std::string Message to be logged.
		*/
		virtual std::string stringToLog() = 0;
	};


    /**
	* @brief Abstract base class representing an observer.
	* 
	* Observers watch a Subject and are notified of events through
	* the update() function when the Subject calls notify().
	*/
	class Observer {
	protected:
		Observer();

	public:
		virtual ~Observer() = default;

		/**
		* @brief Called when the Subject notifies its observers.
		* 
		* @param loggable Pointer to the object whose event is being logged.
		*/
		virtual void update(ILoggable* loggable) = 0;
	};


    /**
	* @brief Base class for any object that can be observed.
	* 
	* Maintains a list of observers that are notified when a loggable
	* event occurs. Classes inheriting from Subject can call notify()
	* to trigger logging or other observer actions.
	*/
	class Subject {
	private:
		std::list<Observer*> _observers; /**< List of attached observers. */

	public:
		Subject();
		virtual ~Subject();

		/**
		* @brief Attaches an observer to this subject.
		* 
		* @param o Pointer to the observer to attach.
		*/
		void attach(Observer* o);

		/**
		* @brief Detaches an observer from this subject.
		* 
		* @param o Pointer to the observer to detach.
		*/
		void detach(Observer* o);

		/**
		* @brief Notifies all attached observers of a change.
		* 
		* @param loggable Pointer to the loggable event source.
		*/
		void notify(ILoggable* loggable);
	};


    /**
	* @brief Concrete observer that writes loggable events to a timestamped file.
	* 
	* A log file is automatically created under "../GameLogs/" with a
	* timestamped filename. Each update() call appends a new entry
	* containing the time and message from ILoggable::stringToLog().
	*/
	class LogObserver : public Observer {
	private:
		std::string logFileName; /**< Full path to the log file. */

	public:

        /**
         * @brief Default Constructor
         * 
         */
		LogObserver();

        /**
         * @brief Destructor
         * 
         */
		~LogObserver() override = default;

		/**
		* @brief Writes the string returned by ILoggable::stringToLog() to the log file.
		* 
		* @param loggable Pointer to the object whose log string will be written.
		*/
		void update(ILoggable* loggable) override;
	};

}
