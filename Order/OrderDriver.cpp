// OrdersDriver.cpp
#include "OrderDriver.h"


void testOrderList() {

    cout << "=== Testing OrderList and Orders with Brazil.map ===" << std::endl;

    // --- Load Map ---
    std::string mapPath = "../Map/test_maps/Brazil/Brazil.map";
    MapLoader loader;

    if (loader.importMapInfo(mapPath) != 0) {
        
        cerr << "Map import failed!" << std::endl;
        return;
    
    }

    pair<bool, Map*> loadMapResults = loader.loadMap();

    if (loadMapResults.first != 0 || loadMapResults.second == nullptr) {

        std::cerr << "Map build failed!" << std::endl;
        return;

    }

    Map* gameMap = loadMapResults.second;

    //This should work, since validate is in the loadMap() method. Just have it in case
    if (!gameMap->validate()) {

        cerr << "Map validation failed!" << std::endl;
        return;

    }

    cout << "Map loaded successfully: " << mapPath << std::endl;

    //Get empty hashmap for player
    unordered_map<Continent*, long long> playerContHashmap = gameMap -> buildEmptyContinentHashmap();

    // --- Setup Players ---
    Player* alice = new Player("Alice", playerContHashmap);
    Player* bob = new Player("Bob", playerContHashmap);
    Player* neutral = new Player("Neutral", playerContHashmap);

    // Get Brazil territories
    vector<Territory*> allTerritories = gameMap -> getTerritories();

    if(allTerritories.size() < 5) {

        cerr << "Map does not have enough territories for testing!" << std::endl;
        return;
    
    }

    //All in Amazonas
    Territory* terrA = allTerritories[0]; //ID 1. Neighbours: 2
    Territory* terrB = allTerritories[1]; //ID 2. Neighbours: 1, 3, 63(Not in scope)
    Territory* terrC = allTerritories[2]; //ID 3. Neighbours: 2, 4, 7, 8(Not in scope) 
    Territory* terrD = allTerritories[3]; //ID 4. Neighbours: 3, 5
    Territory* terrE = allTerritories[4]; //ID 5. Neighbours: 4, 6, 7
    Territory* terrF = allTerritories[5]; //ID 6. Neighbours: 5, 12(Not in scope) 
    Territory* terrG = allTerritories[6]; //ID 7. Neighbours: 3, 5, 10(Not in scope)
    Territory* terrH = allTerritories[7]; //ID 8. Neighbours: 3, 9(Not in scope), 10(Not in scope), 63(Not in scope)


    // Assign ownership
    alice -> addOwnedTerritories(terrA); //ID 1: Alice
    alice -> addOwnedTerritories(terrB); //ID 2: Alice
    bob -> addOwnedTerritories(terrC); //ID 3: Bob
    bob -> addOwnedTerritories(terrD); //ID 4: Bob
    alice -> addOwnedTerritories(terrE); //ID 5: Alice
    alice -> addOwnedTerritories(terrF); //ID 6: Alice
    alice -> addOwnedTerritories(terrG); //ID 7: Alice
    alice -> addOwnedTerritories(terrH); //ID 8: Alice


    terrA->setNumArmies(10);
    terrB->setNumArmies(500);
    terrC->setNumArmies(8);
    terrD->setNumArmies(12);
    terrE->setNumArmies(3);
    terrF->setNumArmies(300);
    terrG->setNumArmies(3000);
    terrH->setNumArmies(100);

    //Check Alice's, Bob's, and Neutral's territories
    cout << endl << "=== Viewing owned territories prior to execution ===" << endl;
    cout << "Alice's territories: "<< alice -> getOwnedTerritories();
    cout << "Bob's territories: "<< bob -> getOwnedTerritories();
    cout << "Neutral player's territories: "<< neutral -> getOwnedTerritories() << endl;

    //Check how many armies bob has on TerrD
    cout << "Check how many armies Bob has on Territory 4. He has " << terrD -> getNumArmies() << " armies on 4" << endl << endl;

    //See what Alice and Bob can attack
    cout << endl << "=== Viewing what Alice and Bob can attack, prior to execution ===" << endl;
    alice -> toAttackPrint();
    bob -> toAttackPrint();
    cout << endl;
    
    //See Bob and Alice's information
    cout << endl << "=== Viewing Player info for Alice and Bob, prior to execution ===" << endl;
    cout << *alice << endl << endl;
    cout << *bob << endl << endl;

    cout << endl << "=== Add orders for Alice ===" << endl;

    //Add Deploy order for Alice
    alice -> getPlayerOrders() -> addOrder(new Deploy(alice, terrA, 5));
    cout << endl <<*(alice -> getPlayerOrders() -> getOrders()[0]) << endl;

    //Add Advance order (Alice moves 7 of her armies from terrA to terrB)
    alice -> getPlayerOrders() -> addOrder(new Advance(alice, terrA, terrB, 7));
    cout << *(alice -> getPlayerOrders() -> getOrders()[1]) << endl;

    //Add Advance order (Alice attacks Bob on terrC with 200 of her armies)
    alice -> getPlayerOrders() -> addOrder(new Advance(alice, terrB, terrC, 200));
    cout << *(alice -> getPlayerOrders() -> getOrders()[2]) << endl;


    // Add Bomb order (Alice bombs Bob’s territory)
    alice -> getPlayerOrders() -> addOrder(new Bomb(alice, terrD));
    cout << *(alice -> getPlayerOrders() -> getOrders()[3]) << endl << endl;

    // Add Blockade order (Alice blockades her own territory, goes Neutral)
    alice -> getPlayerOrders() -> addOrder(new Blockade(alice, terrB, neutral));
    cout << *(alice -> getPlayerOrders() -> getOrders()[4]) << endl;

    // Add Airlift order (Alice moves troops between her territories)
    alice -> getPlayerOrders() -> addOrder(new Airlift(alice, terrF, terrG, 100));
    cout << *(alice -> getPlayerOrders() -> getOrders()[5]) << endl;

    // Add Negotiate order (Alice negotiates with Bob)
    alice -> getPlayerOrders() -> addOrder(new Negotiate(alice, bob));
    cout << *(alice -> getPlayerOrders() -> getOrders()[6]) << endl << endl;

    cout << "=== Adding intentionally invalid orders ===" << endl;

    //Invalid Deploy (Alice tries to deploy to Bob's territory)
    alice -> getPlayerOrders() -> addOrder(new Deploy(alice, terrD, 5));
    cout << *(alice -> getPlayerOrders() -> getOrders()[7]) << endl;

    //Invalid Advance (Alice tries to her armies between nonconnected territories)
    alice -> getPlayerOrders() -> addOrder(new Advance(alice, terrG, terrH, 9999));
    cout << *(alice -> getPlayerOrders() -> getOrders()[8]) << endl;

    //Invalid Bomb (Alice tries to bomb her own territory)
    alice -> getPlayerOrders() -> addOrder(new Bomb(alice, terrA));
    cout << *(alice -> getPlayerOrders() -> getOrders()[9]) << endl;

    //Invalid Blockade (Alice tries to blockade a territory she doesn't own)
    alice -> getPlayerOrders() -> addOrder(new Blockade(alice, terrD, neutral));
    cout << *(alice -> getPlayerOrders() -> getOrders()[10]) << endl;

    //Invalid Airlift (Alice tries to airlift her armies into Bob's territory)
    alice -> getPlayerOrders() -> addOrder(new Airlift(alice, terrA, terrD, 3));
    cout << *(alice -> getPlayerOrders() -> getOrders()[11]) << endl;

    //Invalid Negotiate (Alice tries to negotiate with herself)
    alice -> getPlayerOrders() -> addOrder(new Negotiate(alice, alice));
    cout << *(alice -> getPlayerOrders() -> getOrders()[12]) << endl << endl;

    //Print Alice's OrderList (Note that no effect was added, since the Order was not executed) 
    cout << endl << "Viewing Alice's orders list: "<< *(alice -> getPlayerOrders()) << endl;

    //Execute Orders
    cout << endl << "=== Execute Orders: ===" << endl << endl;
    while(alice -> getPlayerOrders()-> getOrders().empty() == false) {

        Order* tempOrder = alice -> getPlayerOrders() -> getOrders()[0]; //Get order
        tempOrder -> execute(); //Execute order
        alice -> getPlayerOrders() -> removeOrder(0); //Remove order after execution

    }

    //See Bob and Alice's information
    cout << endl << "=== Viewing Player info for Alice and Bob, after execution ===" << endl;
    cout << *alice << endl << endl;
    cout << *bob << endl << endl;

    //Check Alice's, Bob's, and Neutral's territories
    cout << endl << "=== Viewing owned territories after execution ===" << endl;
    cout << "Alice's territories: "<< alice -> getOwnedTerritories();
    cout << "Bob's territories: "<< bob -> getOwnedTerritories();
    cout << "Neutral player's territories: "<< neutral -> getOwnedTerritories() << endl;

    //Check how many armies bob has on TerrD
    cout << "Check how many armies Bob has on Territory 4. He has " << terrD -> getNumArmies() << " armies on 4" << endl << endl;

    //See what Alice and Bob can attack
    cout << endl << "=== Viewing what Alice and Bob can attack, after execution ===" << endl;
    alice -> toAttackPrint();
    bob -> toAttackPrint();

    //Print Alice's OrderList
    cout << endl  << endl << "=== View Alice's OrderList (To be empty): ===" << endl << endl 
         << *(alice -> getPlayerOrders()) << endl;

    // Cleanup
    delete alice;
    delete bob;
    delete neutral;
    delete gameMap;

}

int main() {

    testOrderList();
    return 0;

}
