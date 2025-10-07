#include "MapDriver.h"

using namespace std;
using namespace WarzoneMap;
namespace fs = std::filesystem;

void testLoadMaps() {

    const string directory = "../Map/test_maps"; //Fixed root directory
    vector<string> mapFiles;

    // Collect ALL .map files recursively
    try {

        for(const auto& entry : fs::recursive_directory_iterator(directory)) { //Iterate over all folders in the test_map folder

            if (entry.is_regular_file() && entry.path().extension() == ".map") { //Check if a file in a folder is a .map file AND is a plain file

                mapFiles.push_back(entry.path().string());

            }

        }

    } catch (const fs::filesystem_error& e) {

        cerr << "Filesystem error while scanning '" << directory << "': " << e.what() << endl;
        return;

    }

    if (mapFiles.empty()) {

        cerr << "No .map files found under: " << directory << endl;
        return;
        
    }

    cout << "=== testLoadMaps() ===" << endl;
    cout << "Found " << mapFiles.size() << " .map files under: " << directory << endl;

    int total = 0, passed = 0, failed = 0;
    vector<pair<string,int>> failures; //String for file path, int for type of failure

    for(const string& filePath : mapFiles) { //Iterate over all valid file directories

        total++;
        cout << "\n--- [" << total << "/" << mapFiles.size() << "] Loading: " << filePath << " ---" << endl;

        MapLoader loader; //Intialize MapLoader object

        // Stage 1: parse file
        int importResult = loader.importMapInfo(filePath);

        if(importResult != 0) {

            cerr << "  Import failed (code=" << importResult << ")" << endl;
            failures.emplace_back(filePath, importResult); //emplace for faster performance, instead of push_back
            failed++;
            continue;

        }

        // Stage 2: build Map object
        pair<int, Map*> loadMapResults = loader.loadMap();

        int loadMapStatus = loadMapResults.first;
        Map* tempMapPtr = loadMapResults.second;

        if(loadMapStatus != 0 || tempMapPtr == nullptr) {

            cerr << "  Build failed (code=" << loadMapStatus << ")" << endl;
            failures.emplace_back(filePath, loadMapStatus);
            failed++;
            continue;

        }

        // Stage 3: validate structure
        bool validMap = tempMapPtr -> validate();
        if (!validMap) {

            cerr << "  Validation failed" << endl;
            failures.emplace_back(filePath, -1);
            failed++;
            delete tempMapPtr;
            continue;

        }

        cout << "  OK" << endl;
        passed++;
        delete tempMapPtr;

    }

    // Summary
    cout << "\n=== Testing Summary ===" << endl;
    cout << "Total maps: " << total << endl;
    cout << "Passed:     " << passed << endl;
    cout << "Failed:     " << failed << endl;

    if(!failures.empty()) {

        cout << "\nFailed files (" << failures.size() << "):" << endl;
        
        for(const auto& f : failures) { //Iterate over ALL failed filepaths 

            cout << "  - " << f.first << " (code=" << f.second << ")" << endl;

        }
        
    }

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
    vector<Territory*> allTerritories = gameMap -> getTerritories();

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

    //Print out map data
    cout << *gameMap;

    //Print out win cons
    cout << "\n--- Continent Hashmap (Brazil) ---" << endl;

    for(pair<Continent*, int>entry : gameMap -> getContinentLookupTable()) { //Iterate over hashmap

        Continent* c = entry.first;
        int sum = entry.second;

        cout << "Continent: " << (c ? c->getID() : "NULL") << " | Sum of Territory IDs (ASCII): " << sum << endl;
        
    }

    //Let Alice own ALL territories:
    for(Territory* t : allTerritories){

        alice -> addOwnedTerritories(t);

    }

    //Look at Alice's and Bob's territories, Bob should have NONE.
    cout << endl << endl << *alice << endl << endl;
    cout << *bob << endl << endl;


    //See if she owns Amazonas (she should)
    if(alice -> controlsContinent(gameMap -> getContinentLookupTable(), gameMap -> getContinentByID("Amazonas"))){

        cout << "Alice controls the entirety of Amazonas!\n"; 

    }

    //See if Alice should win the game (she should)

    if(alice -> hasWon(gameMap -> getContinentLookupTable())) {

        cout << "Alice has won the game!\n"; 

    } 

    //ALWAYS delete players first.
    delete alice;
    delete bob;
    delete gameMap;

}
