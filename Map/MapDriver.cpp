#include "MapDriver.h"

using namespace std;
using namespace WarzoneMap;
namespace fs = std::filesystem;

void testLoadMaps() {

    const string directory = "Map/test_maps"; //Fixed root directory
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

}

int main() {

    testLoadMaps();
    return 0;

}
