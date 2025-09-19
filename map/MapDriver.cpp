#include <iostream>
#include <fstream>
#include <sstream>
#include "MapLoader/MapLoader.h"
#include "MapImplementation/Map.h"

using namespace std;

    int main () {

        MapLoader mapLoader;

        int result = mapLoader.importMapInfo("Maps/Brazil/Brazil.map");
        
        if (result != 0) {
            cerr << "Failed to load map file with error code: " << result << endl;
            return result;
        }

        Map* mapPtr = mapLoader.loadMap().second;

        cout << *mapPtr << endl;

        return 0;

    };
