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
#include "LoggingObserver/LoggingObserver.h"
#include "PlayerStrategies/PlayerStrategies.h"
#include "PlayerStrategies/PlayerStrategiesDriver.h"
#include "GameEngine/TournamentDriver.h"

using namespace std;
using namespace WarzoneEngine;

int main(){
   
     testPlayerStrategies();
     testTournament();

     // Offer interactive console so user can type commands (e.g., `tournament ...`)
     cout << "\nWould you like to enter interactive mode to type commands? (y/n): ";
     char choice = 'n';
     if(!(cin >> choice)) { return 0; }
     if(tolower(choice) == 'y'){
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cout << "\nEntering interactive command processor. Type commands like 'loadmap', 'tournament', etc.\n";

          // Setup engine + logger and hand control to CommandProcessor
          GameEngine engine;
          WarzoneLog::LogObserver logger;
          engine.attach(&logger);

          WarzoneCommand::CommandProcessor cp(&engine);
          cp.runGame();

          engine.detach(&logger);
     }

     return 0;
}
