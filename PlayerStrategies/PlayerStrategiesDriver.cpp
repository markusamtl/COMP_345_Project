#include <iostream>
#include <sstream>
#include "../Player/Player.h"
#include "../PlayerStrategies/PlayerStrategies.h"
#include "../Map/Map.h"
#include "../Order/Order.h"
#include "../Card/Card.h"

using namespace std;
using namespace WarzonePlayer;
using namespace WarzonePlayerStrategy;

void testPlayerStrategies()
{
    cout << "===============================" << endl;
    cout << "     TEST: Player Strategies    " << endl;
    cout << "===============================" << endl
         << endl;

    // ----------------------------------------------------
    // 1) Create simple territories to test strategies
    // ----------------------------------------------------
    Continent *c = new Continent("C1", 5);
    Territory *t1 = new Territory("T1", 1, 1, c);
    Territory *t2 = new Territory("T2", 2, 2, c);
    Territory *t3 = new Territory("T3", 3, 3, c);
    Territory *t4 = new Territory("T4", 4, 4, c);

    // Set neighbors
    t1->addNeighbor(t2);
    t2->addNeighbor(t1);
    t2->addNeighbor(t3);
    t3->addNeighbor(t2);
    t3->addNeighbor(t4);
    t4->addNeighbor(t3);

    // ----------------------------------------------------
    // 2) Create players with different strategies
    // ----------------------------------------------------
    Player *human = new Player("Human", {});
    Player *aggressive = new Player("Aggro", {});
    Player *benevolent = new Player("Bene", {});
    Player *neutral = new Player("Neutral", {});
    Player *cheater = new Player("Cheat", {});

    // Assign strategies
    human->setStrategyType(PlayerStrategyType::HUMAN);
    aggressive->setStrategyType(PlayerStrategyType::AGGRESSIVE);
    benevolent->setStrategyType(PlayerStrategyType::BENEVOLENT);
    neutral->setStrategyType(PlayerStrategyType::NEUTRAL);
    cheater->setStrategyType(PlayerStrategyType::CHEATER);

    // ----------------------------------------------------
    // 3) Give them territories
    // ----------------------------------------------------
    human->addOwnedTerritories(t1);
    aggressive->addOwnedTerritories(t2);
    benevolent->addOwnedTerritories(t3);
    neutral->addOwnedTerritories(t4);

    // Reinforcements for testing
    human->setReinforcementPool(5);
    aggressive->setReinforcementPool(10);
    benevolent->setReinforcementPool(10);
    neutral->setReinforcementPool(5);
    cheater->setReinforcementPool(0);

    Deck *deck = new Deck();

    // ----------------------------------------------------
    // 4) SHOW STRATEGIES BEHAVIOR
    // ----------------------------------------------------
    cout << "\n--- Human Strategy Output ---\n";
    cout << human->issueOrder(false, deck, nullptr) << endl;

    cout << "\n--- Aggressive Strategy Output ---\n";
    cout << aggressive->issueOrder(false, deck, nullptr) << endl;

    cout << "\n--- Benevolent Strategy Output ---\n";
    cout << benevolent->issueOrder(false, deck, nullptr) << endl;

    cout << "\n--- Neutral Strategy Output (should do NOTHING) ---\n";
    cout << neutral->issueOrder(false, deck, nullptr) << endl;

    // ----------------------------------------------------
    // 5) Demonstrate Neutral → Aggressive switching
    // ----------------------------------------------------
    cout << "\n--- Demonstrating Neutral → Aggressive Switch ---\n";

    // Aggressive attacks Neutral:
    WarzoneOrder::Advance attackDemo(aggressive, t2, t4, 5);
    cout << "Executing attack (Aggro attacks Neutral)...\n";
    attackDemo.execute();

    cout << "Neutral's new strategy type: "
         << neutral->getStrategyTypeString() << endl
         << endl;

    // ----------------------------------------------------
    // 6) Demonstrate Cheater Strategy (instant conquest)
    // ----------------------------------------------------
    cout << "\n--- Cheater Strategy Output (Instant Conquest) ---\n";
    cout << cheater->issueOrder(false, deck, nullptr) << endl;

    cout << "\n===============================" << endl;
    cout << "     END OF STRATEGY TEST      " << endl;
    cout << "===============================" << endl;
}