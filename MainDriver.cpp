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
#include "PlayerStrategies/PlayerStrategies.h"
#include "PlayerStrategies/PlayerStrategiesDriver.h"
#include "GameEngine/TournamentDriver.h"

using namespace std;
using namespace WarzoneEngine;

int main(){
   
     testPlayerStrategies();

     testTournament();

     return 0;
}
