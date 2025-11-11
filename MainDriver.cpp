#include <iostream>
#include "Map/MapDriver.h"
#include "Player/PlayerDriver.h"
#include "Order/OrderDriver.h"
#include "Card/CardDriver.h"
#include "CommandProcessing/CommandProcessingDriver.h"
#include "CommandProcessing/CommandProcessing.h"
#include "GameEngine/GameEngineDriver.h"
#include "GameEngine/GameEngine.h"
#include "LoggingObserver/LoggingObserverDriver.h"

using namespace std;
using namespace WarzoneEngine;

int main(){
    int choice = 0;

    while(true){
        cout << "\n===========================================" << endl;
        cout << "      WARZONE PROJECT A2 TEST MENU" << endl;
        cout << "===========================================" << endl;
        cout << "1. Test Command Processor" << endl;
        cout << "2. Test Startup Phase" << endl;
        cout << "3. Test Main Game Loop" << endl;
        cout << "4. Test Order Execution" << endl;
        cout << "5. Test Observer Class" << endl;
        cout << "0. Exit" << endl;
        cout << "-------------------------------------------" << endl;
        cout << "Select an option: ";
        cin >> choice;
        cout << endl;

        if(cin.fail()){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }

        switch(choice){
            case 1:
                testCommandProcessor();
                break;

            case 2:
                testStartupPhase();
                break;

            case 3:
                testMainGameLoop();
                break;

            case 4:
                testOrderExecution();
                break;

            case 5:
                testLoggingObserver();
                break;

            case 0:
                cout << "Exiting program..." << endl;
                return 0;

            default:
                cout << "Invalid choice. Please select a valid option." << endl;
                break;
        }
    }

    return 0;
}
