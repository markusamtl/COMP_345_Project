// OrdersDriver.cpp
#include "Order.h"
#include "../Map/Map.h"
#include "../Player/Player.h"

using namespace WarzoneOrder;
using namespace WarzonePlayer;
using namespace WarzoneMap;

void testOrderList() {
    std::cout << "=== Testing OrderList and Orders with Brazil.map ===" << std::endl;

    // --- Load Map ---
    std::string mapPath = "Map/test_maps/Brazil/Brazil.map";
    MapLoader loader;

    if (loader.importMapInfo(mapPath) != 0) {
        std::cerr << "Map import failed!" << std::endl;
        return;
    }

    auto [status, gameMap] = loader.loadMap();
    if (status != 0 || gameMap == nullptr) {
        std::cerr << "Map build failed!" << std::endl;
        return;
    }

    if (!gameMap->validate()) {
        std::cerr << "Map validation failed!" << std::endl;
        return;
    }

    std::cout << "Map loaded successfully: " << mapPath << std::endl;

    // --- Setup Players ---
    Player* alice = new Player("Alice");
    Player* bob   = new Player("Bob");
    Player* neutral = new Player("Neutral");

    // Get some territories
    std::vector<Territory*> allTerritories = gameMap->getTerritories();
    if (allTerritories.size() < 5) {
        std::cerr << "Brazil map does not have enough territories for testing!" << std::endl;
        return;
    }

    Territory* terrA = allTerritories[0]; // Amazonas
    Territory* terrB = allTerritories[1];
    Territory* terrC = allTerritories[2];
    Territory* terrD = allTerritories[3];
    Territory* terrE = allTerritories[4];
    Territory* terrF = allTerritories[5];
    Territory* terrG = allTerritories[6];


    // Assign ownership
    alice -> addOwnedTerritories(terrA);
    alice -> addOwnedTerritories(terrB);
    bob -> addOwnedTerritories(terrC);
    bob -> addOwnedTerritories(terrD);
    alice -> addOwnedTerritories(terrF);
    alice -> addOwnedTerritories(terrG);

    terrA->setNumArmies(10);
    terrB->setNumArmies(5);
    terrC->setNumArmies(8);
    terrD->setNumArmies(12);
    terrE->setNumArmies(3);
    terrF->setNumArmies(300);
    terrG->setNumArmies(3000);

    // --- Create OrderList ---
    OrderList orders;

    // Add Deploy order for Alice
    orders.addOrder(new Deploy(alice, terrA, 5));

    // Add Advance order (Alice attacks Bob)
    orders.addOrder(new Advance(alice, terrA, terrB, 7));

    // Add Bomb order (Alice bombs Bob’s territory)
    orders.addOrder(new Bomb(alice, terrD));

    // Add Blockade order (Alice blockades her own territory, goes Neutral)
    orders.addOrder(new Blockade(alice, terrB, neutral));

    // Add Airlift order (Alice moves troops between her territories)
    orders.addOrder(new Airlift(alice, terrF, terrG, 100));

    // Add Negotiate order (Alice negotiates with Bob)
    orders.addOrder(new Negotiate(alice, bob));

    // --- Print OrderList ---
    cout << orders << endl;

    // --- Execute Orders ---
    for (Order* o : *orders.getOrders()) {
        o->execute();
    }

    // --- Print OrderList ---
    cout << endl << orders << endl;

    // --- Cleanup ---
    delete alice;
    delete bob;
    delete neutral;
    delete gameMap;
}

int main() {

    testOrderList();
    return 0;
}
