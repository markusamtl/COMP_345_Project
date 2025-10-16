#include <iostream>
#include "Map/MapDriver.h"
#include "Player/PlayerDriver.h"
#include "Order/OrderDriver.h"
#include "Card/CardDriver.h"
#include "CommandProcessing/CommandProcessingDriver.h"
#include "CommandProcessing/CommandProcessing.h"
#include "GameEngine/GameEngineDriver.h"
#include "GameEngine/GameEngine.h"


using namespace std;
using namespace WarzoneEngine;

int main() {

    //PART 1
    testCommandProcessor();

    //PART 2
    testStartupPhase();
    
    //PART 3
    testMainGameLoop();
    
    //PART 4
    testOrderExecution();
    
    return 0;

}
