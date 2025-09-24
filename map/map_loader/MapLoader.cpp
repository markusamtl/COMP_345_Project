#include "MapLoader.h"

#define MAP_OK 0
#define MAP_FILE_NOT_FOUND 1
#define MAP_INVALID_SECTION 2
#define MAP_PARSE_ERROR 3

#define INVALID_MAP_PTR 1
#define INVALID_AUTHOR 2
#define INVALID_IMAGE 3
#define INVALID_WRAP 4
#define INVALID_SCROLL 5
#define INVALID_WARN 6
#define INVALID_CONTINENT 7
#define INVALID_TERRITORY 8
#define INVALID_MAP_STRUCTURE 9

using namespace std;
//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

MapLoader::MapLoader() {

    this -> author = "";
    this -> image = "";
    this -> wrap = "";
    this -> scrollType = "";
    this -> warn = "";
    this -> continents = {};
    this -> territories = {};
 
}

MapLoader::MapLoader(string author, string image, string wrap, string scrollType, string warn, map<string, int> continents, vector<vector<string>> territories){
    
    this -> author = author;
    this -> image = image;
    this -> wrap = wrap;
    this -> scrollType = scrollType;
    this -> warn = warn;
    this -> continents = continents;
    this -> territories = territories;

}

MapLoader::~MapLoader() {

    // No dynamic memory to free

}

 MapLoader::MapLoader(const MapLoader& other) {

    this -> author = other.author;
    this -> image = other.image;
    this -> wrap = other.wrap;
    this -> scrollType = other.scrollType;
    this -> warn = other.warn;
    this -> continents = other.continents;
    this -> territories = other.territories;
 
}

MapLoader& MapLoader::operator=(const MapLoader& other) {
    
    if (this != &other) { // self-assignment check
        
        this -> author = other.author;
        this -> image = other.image;
        this -> wrap = other.wrap;
        this -> scrollType = other.scrollType;
        this -> warn = other.warn;
        this -> continents = other.continents;
        this -> territories = other.territories;

    }

    return *this;

}

ostream& operator<<(ostream& os, const MapLoader& mapLoader) {

    os << "Author: " << mapLoader.author << "\n";
    os << "Image: " << mapLoader.image << "\n";
    os << "Wrap: " << mapLoader.wrap << "\n";
    os << "Scroll Type: " << mapLoader.scrollType << "\n";
    os << "Warn: " << mapLoader.warn << "\n";

    os << "Continents:\n";

    for(const auto& [name, value] : mapLoader.continents) {

        os << "  " << name << ": " << value << "\n";
    
    }

    os << "Territories:\n";

    for(const auto& territory : mapLoader.territories) {

        os << "  ";

        for(const auto& field : territory) {

            os << field << " ";

        }

        os << "\n";
    
    }

    return os;

}

//-- Accessors and Mutators --//

const string& MapLoader::getAuthor() const { return author; }
void MapLoader::setAuthor(const string& author) { this -> author = author; }

const string& MapLoader::getImage() const { return image; }
void MapLoader::setImage(const string& image) { this -> image = image; }

const string& MapLoader::getWrap() const { return wrap; }
void MapLoader::setWrap(const string& wrap) { this -> wrap = wrap; }

const string& MapLoader::getScrollType() const { return scrollType; }
void MapLoader::setScrollType(const string& scrollType) { this -> scrollType = scrollType; }

const string& MapLoader::getWarn() const { return warn; }
void MapLoader::setWarn(const string& warn) { this -> warn = warn; }

const map<string, int>& MapLoader::getContinents() const { return continents; }
void MapLoader::setContinents(const map<string, int> continents) { this -> continents = continents; }

const vector<vector<string>>& MapLoader::getTerritories() const {return territories;}
void MapLoader::setTerritories(const vector<vector<string>> territories) { this -> territories = territories; }


//-- Class Methods --//

int MapLoader::importMapInfo(const string& filePath) {
    
    ifstream file(filePath); // Open file buffer on filePath

    if(!file.is_open()) { // If the file directory is invalid

        cerr << "Error: Could not open file " << filePath << endl;
        return MAP_FILE_NOT_FOUND;

    }

    string line;     // To hold each line of the file
    string section;  // To track which section of the map file is being parsed

    while(getline(file, line)) { // Read file line by line

        line = StringHandling::trim(line);       // Remove spaces and \r
        if(line.empty()) continue; // Skip empty lines

        // Detect section headers
        if(line[0] == '[') {
            section = line; // Store section name, e.g., "[Map]"
            continue;
        }

        // Parse [Map] section
        if(section == "[Map]") {

            if(line.find("author=") == 0) author = line.substr(7); // Get everything after "author="
            else if(line.find("image=") == 0) image = line.substr(6); // Get everything after "image="
            else if(line.find("wrap=") == 0) wrap = line.substr(5); // Get everything after "wrap="
            else if(line.find("scroll=") == 0) scrollType = line.substr(7); // Get everything after "scroll="
            else if(line.find("warn=") == 0) warn = line.substr(5); // Get everything after "warn="
            
            else { // The above are the only valid lines in the [Map] section
                cerr << "Invalid line in [Map]: " << line << endl;
                return MAP_PARSE_ERROR;
            }

        } else if (section == "[Continents]") { // Parse [Continents] section

            size_t pos = line.find('='); // Find position of '=' to split continent name and bonus value

            if(pos == string::npos) { // If there is no '=' character, the line is invalid

                cerr << "Invalid continent line: " << line << endl;
                return MAP_PARSE_ERROR;
            }

            string continentName = StringHandling::trim(line.substr(0, pos)); // Extract continent name
            string continentBonusStr = StringHandling::trim(line.substr(pos + 1)); // Extract continent name
            int bonusValue = 0;
            
            pair<bool, int> bonusValueStatus = StringHandling::isStrInt(continentBonusStr); //Get bonus valus

            if(bonusValueStatus.first == true) { continents[continentName] = bonusValueStatus.second; } 
            else { return MAP_PARSE_ERROR; }


        } else if (section == "[Territories]") { // Parse [Territories] section

            vector<string> fields; //To hold the fields of the territory line
            string token; //Holds each field temporarily for a territory line
            stringstream ss(line); //Use stringstream for easy CSV parsing
            
            while(getline(ss, token, ',')) { //Split line by commas

                string trimmed = StringHandling::trim(token); //Trim leading/trailing spaces
                if(!trimmed.empty()){ fields.push_back(trimmed); } //Make sure the territory info isn't empty 

            }

            if(fields.size() < 4) { // Must have at least ID, XCoord, YCoord, Continent

                cerr << "Invalid territory line: " << line << endl;
                return MAP_PARSE_ERROR;
                
            }

            territories.push_back(fields); // Add the parsed territory to the territories vector

        } else { // If section is invalid

            cerr << "Unknown section: " << section << endl;
            return MAP_INVALID_SECTION;

        }

    }

    file.close();

    cout << "Map loaded successfully!" << endl << "  Continents loaded: " << continents.size() << endl << "  Territories loaded: " << territories.size() << endl;

    return MAP_OK;
}

pair<int, Map*> MapLoader::loadMap() {

    if(this == nullptr) { //Check for null pointer

        cerr << "Error: Input map pointer is null." << endl;
        return {INVALID_MAP_PTR, nullptr};

    }
    

    //Create return map pointer, to be returned IF loading is successful
    Map* tempMapPtr = new Map();

    //-- Check if all metadata is valid --

    if(author.empty()) { //Check author metadata

        cerr << "Error: Author metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return {INVALID_AUTHOR, nullptr};

    } else { tempMapPtr -> setAuthor(author); }

    if(image.empty()) { //Check image metadata

        cerr << "Error: Image metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return {INVALID_IMAGE, nullptr};

    } else { tempMapPtr -> setImage(image); }

    if(wrap.empty()) { //Check wrap metadata

        cerr << "Error: Wrap metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return {INVALID_WRAP, nullptr};

    } else { tempMapPtr -> setWrap(wrap); }

    if(scrollType.empty()) { //Check scrollType metadata

        cerr << "Error: ScrollType metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return {INVALID_SCROLL, nullptr};

    } else { tempMapPtr -> setScrollType(scrollType); }

    if(warn.empty()) { //Check warn metadata

        cerr << "Error: Warn metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return {INVALID_WARN, nullptr};

    } else { tempMapPtr -> setWarn(warn); }

    //Load continents into map object
    if(continents.empty()) {

        cerr << "Error: Continent data is missing or invalid." << endl;
        delete tempMapPtr;
        return {INVALID_CONTINENT, nullptr};

    } else {

        vector<Continent*> continentPtrs; //To hold pointers for created continents

        for (const auto& [name, value] : continents) { //Auto was really convenient here compared to pair as an iterator
            
            //Validate BEFORE allocating
            if (name.empty() || value <= 0) {

                cerr << "Error: Invalid continent data for continent '" << name << "' with bonus value " << value << "." << endl;
                for (Continent* c : continentPtrs) {delete c;}
                delete tempMapPtr;
                return {INVALID_CONTINENT, nullptr};

            }

            //Only allocate if validation passed
            Continent* newContinent = new Continent(name, value);
            continentPtrs.push_back(newContinent);

        }

        tempMapPtr -> setContinents(continentPtrs); //Set continents in the map

        //-- Load territories into map object (PASS 1: Instantiating the Map) --

        unordered_map<string, Territory*> lookup; //Fast lookup map of territory names to pointers
        vector<Territory*> territoryPtrs; //To hold pointers to created territories

        if(territories.empty()) {

            cerr << "Error: Territory data is missing or invalid." << endl;
            delete tempMapPtr;
            return {INVALID_TERRITORY, nullptr};

        } else {

            for(int i = 0; i < territories.size(); i++) { //For ALL territories

                //Extract mandatory fields from territory entry
                string territoryID   = territories[i][0];
                string xCoordStr     = territories[i][1];
                string yCoordStr     = territories[i][2];
                string continentName = territories[i][3];

                //-- Validate that all mandatory fields are valid --

                //Check Territory ID
                if(territoryID.empty()) {

                    cerr << "Error: Territory ID is missing or invalid in territory entry " << i+1 << "." << endl;
                    delete tempMapPtr;
                    return {INVALID_TERRITORY, nullptr};

                }

                //Check X Coordinate
                if(xCoordStr.empty()) {

                    cerr << "Error: X coordinate is missing or invalid in territory entry " << i+1 << "." << endl;
                    delete tempMapPtr;
                    return {INVALID_TERRITORY, nullptr};

                }
                
                pair<bool,int> xCoordStatus = StringHandling::isStrInt(xCoordStr);
                if(!xCoordStatus.first) {

                    cerr << "Error: X coordinate is not a valid number in territory entry " << i+1 << "." << endl;
                    delete tempMapPtr;
                    return {INVALID_TERRITORY, nullptr};

                }

                //Check Y Coordinate
                if(yCoordStr.empty()) {

                    cerr << "Error: Y coordinate is missing or invalid in territory entry " << i+1 << "." << endl;
                    delete tempMapPtr;
                    return {INVALID_TERRITORY, nullptr};

                }

                pair<bool,int> yCoordStatus = StringHandling::isStrInt(yCoordStr);
                if(!yCoordStatus.first) {

                    cerr << "Error: Y coordinate is not a valid number in territory entry " << i+1 << "." << endl;
                    delete tempMapPtr;
                    return {INVALID_TERRITORY, nullptr};

                }

                //Check Continent Name
                if(continentName.empty()) {

                    cerr << "Error: Continent ID is missing or invalid in territory entry " << i+1 << "." << endl;
                    delete tempMapPtr;
                    return {INVALID_TERRITORY, nullptr};

                }

                Continent* tempContinentPtr = tempMapPtr -> getContinentByID(continentName); //O(n) time
                if(tempContinentPtr == nullptr) {

                    cerr << "Error: Continent ID is missing or invalid in territory entry " << i+1 << "." << endl;
                    delete tempMapPtr;
                    return {INVALID_TERRITORY, nullptr};

                }

                //All validations passed, now allocate territory
                Territory* tempTerritory = new Territory();
                tempTerritory->setID(territoryID);
                tempTerritory->setXCoord(xCoordStatus.second);
                tempTerritory->setYCoord(yCoordStatus.second);
                tempTerritory->setContinent(tempContinentPtr);

                //Attach to continent + map
                tempContinentPtr->addTerritory(tempTerritory);
                tempMapPtr->addTerritory(tempTerritory);
                territoryPtrs.push_back(tempTerritory);
                lookup[territoryID] = tempTerritory;

            }

            //-- Load neighbouring territories into map object (PASS 2: Populating Neighbours) --
            for(size_t i = 0; i < territories.size(); i++) { //For ALL territories

                string territoryID = territories[i][0]; //Get territory ID
                Territory* current = lookup[territoryID]; //Get pointer to current territory

                for(size_t j = 4; j < territories[i].size(); j++) { //Cover the neighbouring territories IDs

                    string neighborID = territories[i][j]; //Get neighbor ID

                    if(lookup.count(neighborID)) { //MUST be bidirectional, since graph is undirected
                        
                        Territory* neighbor = lookup[neighborID];

                        current -> addNeighbor(neighbor);
                        neighbor -> addNeighbor(current);

                    } else {
                        
                        cerr << "Error: Neighbor '" << neighborID << "' not found for territory '" << territoryID << "'." << endl;
                        delete tempMapPtr;
                        return {INVALID_TERRITORY, nullptr};

                    }

                }

            }

        }
        
    }

    // Validate map structure
    if(!(tempMapPtr -> validate())) {

        cerr << "Error: The map structure is invalid." << endl;
        delete tempMapPtr;
        return {INVALID_MAP_STRUCTURE, nullptr};

    }

    return {MAP_OK, tempMapPtr};

}
