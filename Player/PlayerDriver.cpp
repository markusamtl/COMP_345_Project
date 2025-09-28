#include "PlayerDriver.h"

void testPlayer() {
    cout << "=== Testing Player with Brazil.map ===" << endl;

    // --- Load Map ---
    string mapPath = "Map/test_maps/Brazil/Brazil.map";
    MapLoader loader;

    if (loader.importMapInfo(mapPath) != 0) {

        cerr << "Map import failed!" << endl;
        return;

    }

    pair<bool, Map*> loadMapResults = loader.loadMap();

    if (loadMapResults.first != 0 || loadMapResults.second == nullptr) {

        cerr << "Map build failed!" << endl;
        return;

    }

    Map* gameMap = loadMapResults.second;

    if (!gameMap->validate()) {

        cerr << "Map validation failed!" << endl;
        return;

    }

    cout << "Map loaded successfully: " << mapPath << endl;

    // --- Setup Players ---
    Player* alice   = new Player("Alice");
    Player* bob     = new Player("Bob");

    // Get Brazil territories
    vector<Territory*> allTerritories = gameMap->getTerritories();
    if (allTerritories.size() < 5) {

        cerr << "Not enough territories for testing!" << endl;
        return;

    }

    // Assign some territories
    Territory* terrA = allTerritories[0]; // ID 1
    Territory* terrB = allTerritories[1]; // ID 2
    Territory* terrC = allTerritories[2]; // ID 3
    Territory* terrD = allTerritories[3]; // ID 4
    Territory* terrE = allTerritories[4]; // ID 5

    alice -> addOwnedTerritories(terrA);
    alice -> addOwnedTerritories(terrB);
    bob -> addOwnedTerritories(terrC);
    bob -> addOwnedTerritories(terrD);
    alice -> addOwnedTerritories(terrE);

    terrA->setNumArmies(10);
    terrB->setNumArmies(20);
    terrC->setNumArmies(15);
    terrD->setNumArmies(25);
    terrE->setNumArmies(5);

    // --- Show initial state ---
    cout << endl << "=== Player's Initial State: ===" << endl;
    cout << "For Alice\n "<< *alice << endl;
    cout << "For Bob\n" << *bob << endl;

    // --- Issue random orders ---
    cout << endl << "=== Alice issues 5 random orders ===" << endl;
    for(int i = 0; i < 5; i++){

        cout << "Calling issueOrder" << endl;
        alice -> issueOrder();
        cout << endl;

    }

    cout << endl << "=== Bob issues 5 random orders ===" << endl;
    for(int i = 0; i < 5; i++){
        
        cout << "Calling issueOrder" << endl;
        bob -> issueOrder();
        cout << endl;

    }

    // --- Show final state ---
    cout << endl << "=== Player's Final State: ===" << endl;
    cout << "For Alice\n "<< *alice << endl;
    cout << "For Bob\n" << *bob << endl;

    // --- Show attack/defend options ---

    cout << endl << "=== Territories to Attack: Alice ===" << endl;
    alice -> toAttackPrint();

    cout << endl << "=== Results of toAttack method for Alice: ===" << endl;
    for(Territory* t : alice -> toAttack()) { cout << *t << endl; }

    cout << endl << "=== Territories to Attack: Bob ===" << endl;
    bob -> toAttackPrint();

    cout << endl << "=== Results of toAttack method for Bob: ===" << endl;
    for(Territory* t : bob -> toAttack()) { cout << *t << endl; }

    cout << endl << "=== Territories to Defend: Alice ===" << endl;
    alice -> toDefendPrint();

    cout << endl << "=== Results of toDefend method for Alice: ===" << endl;
    for(Territory* t : alice -> toDefend()) { cout << *t << endl; }

    cout << endl << "=== Territories to Defend: Bob ===" << endl;
    bob -> toDefendPrint();

    cout << endl << "=== Results of toDefend method for Bob: ===" << endl;
    for(Territory* t : bob -> toDefend()) { cout << *t << endl; }

    // --- Cleanup ---
    delete alice;
    delete bob;
    delete gameMap;

}

int main() {

    testPlayer();
    return 0;

}
