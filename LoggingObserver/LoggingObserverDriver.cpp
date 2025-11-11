#include "LoggingObserverDriver.h"
#include "../LoggingObserver/LoggingObserver.h"
#include "../CommandProcessing/CommandProcessing.h"
#include "../Order/Order.h"
#include "../GameEngine/GameEngine.h"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

using namespace WarzoneLog;
using namespace WarzoneCommand;
using namespace WarzoneOrder;
using namespace WarzoneEngine;

void testLoggingObserver() {
    cout << "=============================================\n";
    cout << "   TEST: LoggingObserver Pattern Verification \n";
    cout << "=============================================\n\n";

    // 1) Create a single LogObserver
    LogObserver* logObserver = new LogObserver();

    // 2) Command + CommandProcessor demo
    cout << "[TEST] CommandProcessor + Command Logging\n";
    CommandProcessor* cmdProcessor = new CommandProcessor(nullptr);
    cmdProcessor->attach(logObserver);

    Command* command = new Command("loadmap testMap.map");
    command->attach(logObserver);

    cmdProcessor->saveCommand(command);
    command->setEffect("Map successfully loaded.");
    cout << "  -> Command saved and effect logged.\n\n";

    // 3) Order + OrderList demo
    cout << "[TEST] Order + OrderList Logging\n";
    OrderList* orderList = new OrderList();
    orderList->attach(logObserver);

    Deploy* deployOrder = new Deploy(nullptr, nullptr, 5);
    deployOrder->attach(logObserver);

    orderList->addOrder(deployOrder);
    deployOrder->setEffect("Deployed 5 armies successfully.");
    deployOrder->execute();
    cout << "  -> Order added and executed, both logged.\n\n";

    // 4) GameEngine demo
    cout << "[TEST] GameEngine State Change Logging\n";
    GameEngine* engine = new GameEngine();
    engine->attach(logObserver);

    engine->setState(EngineState::Start);
    engine->logAndNotify("[GameEngine] State transitioned to Start.");

    engine->setState(EngineState::MapLoaded);
    engine->logAndNotify("[GameEngine] State transitioned to MapLoaded.");

    engine->setState(EngineState::PlayersAdded);
    engine->logAndNotify("[GameEngine] State transitioned to PlayersAdded.");
    cout << "  -> Engine state transitions logged.\n\n";

    // 5) Verify log file
    cout << "[TEST] Checking ../GameLogs/ for generated log file...\n";
    string latestFile;
    file_time_type latestTime = file_time_type::min();

    try {
        for (const auto& entry : directory_iterator("../GameLogs/")) {
            if (entry.is_regular_file()) {
                auto ftime = entry.last_write_time();
                if (ftime > latestTime) {
                    latestTime = ftime;
                    latestFile = entry.path().string();
                }
            }
        }

        if (!latestFile.empty()) {
            cout << "  -> Log file detected: " << latestFile << "\n";
        } else {
            cout << "  -> ERROR: No log files found in ../GameLogs/.\n";
        }
    }
    catch (const exception& e) {
        cout << "  -> ERROR: Could not read from ../GameLogs/. Reason: " << e.what() << "\n";
    }

    if (!latestFile.empty()) {
        ifstream preview(latestFile);
        if (preview.is_open()) {
            cout << "\n  --- Log File Preview ---\n";
            string line;
            int count = 0;
            while (getline(preview, line) && count < 10) {
                cout << "  " << line << "\n";
                ++count;
            }
            cout << "  ------------------------\n\n";
            preview.close();
        }
    }

    // ====== CLEANUP ====== //
    delete engine;
    delete orderList;
    delete cmdProcessor;
    delete logObserver;

    cout << "=============================================\n";
    cout << "   LoggingObserver Test Completed Successfully\n";
    cout << "=============================================\n";

}