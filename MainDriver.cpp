#include <iostream>
#include "Map/MapDriver.h"
#include "Player/PlayerDriver.h"
#include "Order/OrderDriver.h"
#include "Card/CardDriver.h"
#include "GameEngine/GameEngineDriver.h"
#include "GameEngine/GameEngine.h"


using namespace std;
using namespace WarzoneEngine;

int main() {
    
    testOrderExecution();
    testStartupPhase();
    testMainGameLoop();
    
    return 0;

}
