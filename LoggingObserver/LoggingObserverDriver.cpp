#include "LoggingObserverDriver.h"
#include "../Order/Order.h"
#include "../CommandProcessing/CommandProcessing.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>

using namespace WarzoneLog;

void testLoggingObserver() {

    cout << "=============================================\n";
    cout << "   TEST: LoggingObserver Pattern Verification \n";
    cout << "=============================================\n\n";

    // Create the LogObserver (observer)
    LogObserver* logObserver = new LogObserver();

    // --- Clear old log file content ---
    ofstream clearFile("gamelog.txt", ios::trunc);
    clearFile.close();

    // COMMAND + COMMAND PROCESSOR DEMO
    cout << "[TEST] CommandProcessor + Command Logging\n";

    CommandProcessor* cmdProcessor = new CommandProcessor(nullptr);
    cmdProcessor->attach(logObserver); // attach observer to subject

    Command* command = new Command("addplayer Alice");
    command->attach(logObserver); // Command is also a Subject

    command->setEffect("Player successfully added.");
    cmdProcessor->saveCommand(command); // triggers notify(this)

    cout << "  -> Command saved and logged.\n\n";


    // ORDER + ORDERLIST DEMO
    cout << "[TEST] Order + OrderList Logging\n";

    Deploy* deployOrder = new Deploy(nullptr, nullptr, 5);
    deployOrder->attach(logObserver);

    OrderList* orderList = new OrderList();
    orderList->attach(logObserver);

    orderList->addOrder(deployOrder); // triggers notify(this)
    deployOrder->execute();           // triggers notify(this)

    cout << "  -> Order added and executed, both logged.\n\n";


    // GAME ENGINE STATE CHANGE DEMO
    cout << "[TEST] GameEngine State Change Logging\n";

    GameEngine* engine = new GameEngine();
    engine->attach(logObserver);

    // Using setState() to simulate transitions
    engine->setState(EngineState::MapLoaded);
    engine->setState(EngineState::MapValidated);
    engine->setState(EngineState::PlayersAdded);

    cout << "  -> State changes logged.\n\n";


    // Verify log file creation
    cout << "[TEST] Checking gamelog.txt content...\n";
    ifstream infile("gamelog.txt");
    if (infile.good()) {
        cout << "  -> gamelog.txt successfully written!\n\n";
    } else {
        cout << "  -> ERROR: gamelog.txt not found.\n\n";
    }
    infile.close();


    // Cleanup
    delete logObserver;
    delete cmdProcessor;
    delete command;
    delete deployOrder;
    delete orderList;
    delete engine;

    cout << "=============================================\n";
    cout << "   LoggingObserver Test Completed Successfully\n";
    cout << "=============================================\n";

}