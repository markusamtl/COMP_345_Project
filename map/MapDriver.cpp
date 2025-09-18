#include <iostream>
#include <fstream>
#include <sstream>
#include "MapLoader/MapLoader.h"
using namespace std;

    int main () {

        MapLoader mapLoader;

        int result = mapLoader.importMapInfo("../Maps/Brazil/Brazil.map");
        
        if (result != 0) {
            cerr << "Failed to load map file with error code: " << result << endl;
            return result;
        }

        cout << mapLoader << endl;
        return 0;

    };
