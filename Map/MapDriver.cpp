#include <iostream>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Map.h"
#include "MapDriver.h"

using namespace std;
using namespace WarzoneMap;   // pull in Map, MapLoader, etc.
namespace fs = std::filesystem;

void testLoadMaps(const string& directory, int numOfMapsToLoad) {
    
    vector<string> mapFiles;

    //Collect all map folders and find any .map file inside
    for(const auto& entry : fs::directory_iterator(directory)) {

        if(entry.is_directory()) { //Validate that the folder name is valid

            string folderName = entry.path().filename().string();
            bool foundMap = false;

            //Look inside this folder for a .map file
            for(const auto& inner : fs::directory_iterator(entry.path())) { //Look at all files in the folder

                if(inner.is_regular_file() && inner.path().extension() == ".map") { //If the checked file ends with .map

                    mapFiles.push_back(inner.path().string());
                    foundMap = true;
                    break; //Use the first .map found

                }

            }

            //Should hopefully not be the case, every map has been tested
            if(!foundMap) { cerr << "Skipping folder (no .map file found): " << folderName << endl; }

        }

    }

    if(mapFiles.empty()) { //If NO .map files were found

        cerr << "No valid .map files found in subfolders of: " << directory << endl;
        return;

    }

    // Seed RNG
    srand(static_cast<unsigned>(time(nullptr)));

    //Associate numOfMapsToLoad to available maps
    numOfMapsToLoad = min(numOfMapsToLoad, static_cast<int>(mapFiles.size()));

    cout << "=== testLoadMaps() ===" << endl << "Attempting to load " << numOfMapsToLoad << " random maps from subfolders of: " << directory << endl;

    for(int i = 0; i < numOfMapsToLoad; i++) { //For all maps

        // Pick random index
        int index = rand() % mapFiles.size();
        string filePath = mapFiles[index];

        // Remove it from list so it’s not chosen again
        mapFiles.erase(mapFiles.begin() + index);

        cout << "\n--- Loading Map " << (i+1) << " ---" << "File Name: " << filePath << endl;

        MapLoader loader; //Intialize the MapLoader to help import / load maps
        int importResult = loader.importMapInfo(filePath); //Import file info to temporary data structure

        if(importResult != 0) { //Debugging

            cerr << "Import failed for: " << filePath << " (Error " << importResult << ")" << endl;
            continue;

        }

        //Extract info from MapLoader object and insert into a map object
        pair<int, Map*> loadResult = loader.loadMap();
        int status = loadResult.first;
        Map* mapPtr = loadResult.second;


        //Ensure that everything went smoothly
        if(mapPtr == nullptr || status != 0) { 
           
            cerr << "MapLoader::loadMap failed for: " << filePath << " (error " << status << ")" << endl;
        } else {

            cout << "Validation successful: " << filePath << endl;
            delete mapPtr; // cleanup

        }

    }

}

int main() {

    string directory = "Map/test_maps";

    // Count how many .map files exist
    vector<string> mapFiles;

    for(const auto& entry : fs::directory_iterator(directory)) { //For all folders within the given directory

        if(entry.is_directory()) { //If a folder contains files 

            for(const auto& inner : fs::directory_iterator(entry.path())) { //For all files within the folder

                if(inner.is_regular_file() && inner.path().extension() == ".map") { //If a file ends with .map

                    mapFiles.push_back(inner.path().string());
                    break; // take the first .map found per folder

                }

            }

        }

    }

    int availableMaps = static_cast<int>(mapFiles.size());

    if(availableMaps == 0) { //Duh

        cerr << "No valid .map files found in subfolders of: " << directory << endl;
        return 1;

    }

    int numOfMaps = 0;
    cout << "Enter how many maps you want to test (1–" << availableMaps << "): ";

    // Keep looping until valid input is given
    while(!(cin >> numOfMaps) || numOfMaps <= 0 || numOfMaps > availableMaps) {

        cout << "Invalid input. Please enter a number between 1 and " << availableMaps << ": ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Clear input buffer

    }

    testLoadMaps(directory, numOfMaps);

    return 0;
}
