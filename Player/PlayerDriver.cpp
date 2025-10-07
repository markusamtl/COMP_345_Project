#include "PlayerDriver.h"

void testPlayer() {
    
    cout << "=== Testing Player with Brazil.map ===" << endl;

    // --- Load Map ---
    string mapPath = "../Map/test_maps/Brazil/Brazil.map";
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

    //Get empty hashmap for player
    unordered_map<Continent*, long long> playerContHashmap = gameMap -> buildEmptyContinentHashmap();

    // --- Setup Players ---
    Player* alice = new Player("Alice", playerContHashmap);
    Player* bob = new Player("Bob", playerContHashmap);

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

    Deck* deck = new Deck(2); //Assume 2 players for testing, Alice and Bob

    // --- Draw cards into Alice's and Bob's hands ---
    cout << "\nDrawing 20 cards into Alice's and Bob's hand..." << endl;
    for(size_t i = 0; i < deck -> getCards().size(); i++) {

        Card* drawnAlice = deck -> draw();
        Card* drawnBob = deck -> draw();

        if(drawnAlice != nullptr){

            alice -> getHand() -> addCardToHand(drawnAlice);
            cout << "Alice Drew: " << *drawnAlice << endl;

        }

        if(drawnBob != nullptr){

            bob -> getHand() -> addCardToHand(drawnBob);
            cout << "Bob Drew: " << *drawnBob << endl;

        }

    }

    // --- Issue random orders ---
    cout << endl << "=== Alice issues 5 random orders ===" << endl;
    for(int i = 0; i < 5; i++){

        alice -> issueOrder();
    }

    cout << endl << "=== Alice's Final State: ===" << endl;
    cout << *alice -> getPlayerOrders() << endl;

    cout << endl << "=== Bob issues 5 random orders ===" << endl;
    for(int i = 0; i < 5; i++){
        
        bob -> issueOrder();

    }

    cout << endl << "=== Bob's Final State: ===" << endl;
    cout << *bob -> getPlayerOrders() << endl;

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
