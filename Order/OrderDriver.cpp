#include "OrderDriver.h"

void testOrderExecution() {

    cout << "=============================================\n";
    cout << "       TEST: ORDER EXECUTION (BRAZIL.MAP)    \n";
    cout << "=============================================\n\n";

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

    cout << "[Setup] Map loaded successfully: " << mapPath << std::endl;

    //Get empty hashmap for player
    unordered_map<Continent*, long long> playerContHashmap = gameMap -> buildEmptyContinentHashmap();

    //Setup Players
    Player* alice = new Player("Alice", playerContHashmap);
    Player* bob = new Player("Bob", playerContHashmap);
    Player* neutral = new Player("Neutral", playerContHashmap);

    //Create Deck
    Deck* gameDeck = new Deck(2);
    cout << "[Setup] Deck initialized with " << gameDeck -> getCards().size() << " cards.\n\n";

    //Territories
    vector<Territory*> allTerritories = gameMap -> getTerritories();

    if(allTerritories.size() < 8) {

        cerr << "Map does not have enough territories for testing!" << std::endl;
        return;
    
    }

    Territory* terrA = allTerritories[0];
    Territory* terrB = allTerritories[1];
    Territory* terrC = allTerritories[2];
    Territory* terrD = allTerritories[3];
    Territory* terrE = allTerritories[4];
    Territory* terrF = allTerritories[5];
    Territory* terrG = allTerritories[6];
    Territory* terrH = allTerritories[7];

    //Assign Ownership
    alice -> addOwnedTerritories(terrA);
    alice -> addOwnedTerritories(terrB);
    bob -> addOwnedTerritories(terrC);
    bob -> addOwnedTerritories(terrD);
    bob -> addOwnedTerritories(terrE);
    alice -> addOwnedTerritories(terrF);
    alice -> addOwnedTerritories(terrG);
    alice -> addOwnedTerritories(terrH);

    terrA -> setNumArmies(10);
    terrB -> setNumArmies(500);
    terrC -> setNumArmies(8);
    terrD -> setNumArmies(12);
    terrE -> setNumArmies(3);
    terrF -> setNumArmies(300);
    terrG -> setNumArmies(3000);
    terrH -> setNumArmies(100);

    //Have each type of card to demonstrate Alice's deck prior to conquering territory
    Card* bombCard     = gameDeck -> draw(WarzoneCard::CardType::Bomb);
    Card* blockadeCard = gameDeck -> draw(WarzoneCard::CardType::Blockade);
    Card* airliftCard  = gameDeck -> draw(WarzoneCard::CardType::Airlift);
    Card* diploCard    = gameDeck -> draw(WarzoneCard::CardType::Diplomacy);

    //Add each card to Alice’s hand before issuing
    alice -> getHand() -> addCardToHand(bombCard);
    alice -> getHand() -> addCardToHand(blockadeCard);
    alice -> getHand() -> addCardToHand(airliftCard);
    alice -> getHand() -> addCardToHand(diploCard);

    //Print Setup Info

    cout << endl << "=== Viewing owned territories prior to execution ===" << endl;
    cout << "Alice's territories: "<< alice -> getOwnedTerritories();
    cout << "Bob's territories: "<< bob -> getOwnedTerritories();
    cout << "Neutral player's territories: "<< neutral -> getOwnedTerritories() << endl;

    cout << "Check how many armies Bob has on Territory 4. He has " 
         << terrD -> getNumArmies() << " armies on 4" << endl << endl;

    cout << "=== Viewing what Alice and Bob can attack (before) ===" << endl;
    cout << alice -> toAttackString();
    cout << bob -> toAttackString() << endl;

    cout << endl << "=== Viewing Player info prior to execution ===" << endl;
    cout << *alice << endl << endl;
    cout << *bob << endl << endl;

    // ========================= ORDERS ========================= //
    cout << endl << "=== Adding orders for Alice (Valid Orders) ===" << endl;

    // Add valid orders
    alice -> getPlayerOrders() -> addOrder(new Deploy(alice, terrA, 5));
    alice -> getPlayerOrders() -> addOrder(new Advance(alice, terrA, terrB, 7));
    alice -> getPlayerOrders() -> addOrder(new Advance(alice, terrB, terrC, 500));
    alice -> getPlayerOrders() -> addOrder(new Advance(alice, terrC, terrD, 200));
    alice -> getPlayerOrders() -> addOrder(new Bomb(alice, terrE));
    alice -> getPlayerOrders() -> addOrder(new Blockade(alice, terrB, neutral));
    alice -> getPlayerOrders() -> addOrder(new Airlift(alice, terrF, terrG, 100));
    alice -> getPlayerOrders() -> addOrder(new Negotiate(alice, bob));
    alice -> getPlayerOrders() -> addOrder(new Advance(alice, terrD, terrE, 50));
    

    // Display added orders
    for(size_t i = 0; i < alice -> getPlayerOrders() -> size(); i++){
        cout << alice -> getPlayerOrders() -> getOrders()[i] -> getEffect() << endl;
    }

    cout << endl << "Viewing Alice's hand PRIOR to execution: " << *(alice -> getHand()) << endl;

    // ========================= EXECUTION PHASE ========================= //
    cout << endl << "=== EXECUTION PHASE ===" << endl;
    while(!alice -> getPlayerOrders()-> getOrders().empty()) {

        Order* tempOrder = alice -> getPlayerOrders() -> getOrders()[0];
        cout << (tempOrder -> validate()).second << endl;
        tempOrder -> execute(); //Executes
        cout << tempOrder -> getEffect() << endl;
        alice -> getPlayerOrders() -> removeOrder(0);
    }

    // Reward card if territory conquered
    if(alice -> getGenerateCardThisTurn()) {
        Card* rewardCard = gameDeck -> draw();
        alice -> getHand() -> addCardToHand(rewardCard);
        cout << "\n[Reward] Alice conquered a territory and drew a card!\n";
    }

    cout << endl << "Viewing Alice's hand AFTER execution: " << *(alice -> getHand()) << endl;

    // ========================= POST EXECUTION ========================= //
    cout << endl << "=== Viewing Player info AFTER execution ===" << endl;
    cout << *alice << endl << endl;
    cout << *bob << endl << endl;

    cout << "\n=== Viewing owned territories AFTER execution ===" << endl;
    cout << "Alice's territories: "<< alice -> getOwnedTerritories();
    cout << "Bob's territories: "<< bob -> getOwnedTerritories();
    cout << "Neutral player's territories: "<< neutral -> getOwnedTerritories() << endl;

    cout << "=== Viewing what Alice and Bob can attack AFTER execution ===" << endl;
    cout << alice -> toAttackString();
    cout << bob -> toAttackString() << endl;

    cout << endl << "=== Final Order List for Alice (Should be empty) ===" << endl;
    cout << *(alice -> getPlayerOrders()) << endl;

    // ========================= CLEANUP ========================= //
    delete alice;
    delete bob;
    delete neutral;
    delete gameDeck;
    delete gameMap;

    cout << "\n=============================================\n";
    cout << "         END OF ORDER EXECUTION TEST         \n";
    cout << "=============================================\n";

}
