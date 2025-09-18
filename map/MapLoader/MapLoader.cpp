#include "MapLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

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

    for (const auto& [name, value] : mapLoader.continents) {

        os << "  " << name << ": " << value << "\n";
    
    }

    os << "Territories:\n";

    for (const auto& territory : mapLoader.territories) {

        os << "  ";

        for (const auto& field : territory) {

            os << field << " ";

        }

        os << "\n";
    
    }

    return os;

}

//-- Accessors and Mutators --//

const string& MapLoader::getAuthor() const { return author; }
void MapLoader::setAuthor(string& author) { this -> author = author; }

const string& MapLoader::getImage() const { return image; }
void MapLoader::setImage(string& image) { this -> image = image; }

const string& MapLoader::getWrap() const { return wrap; }
void MapLoader::setWrap(string& wrap) { this -> wrap = wrap; }

const string& MapLoader::getScrollType() const { return scrollType; }
void MapLoader::setScrollType(string& scrollType) { this -> scrollType = scrollType; }

const string& MapLoader::getWarn() const { return warn; }
void MapLoader::setWarn(string& warn) { this -> warn = warn; }

const map<string, int>& MapLoader::getContinents() const { return continents; }
void MapLoader::setContinents(map<string, int> continents) { this -> continents = continents; }

const vector<vector<string>>& MapLoader::getTerritories() const {return territories;}
void MapLoader::setTerritories(vector<vector<string>> territories) { this -> territories = territories; }


//-- Class Methods --//

string MapLoader::trim(const string& s) {

    size_t start = s.find_first_not_of(" \t\r\n"); // Find first non-space character
    size_t end   = s.find_last_not_of(" \t\r\n"); // Find last non-space character
    return (start == string::npos) ? "" : s.substr(start, end - start + 1); // Return trimmed string, or no string if all spaces

}

int MapLoader::importMapInfo(const string& filePath) {
    
    ifstream file(filePath); // Open file buffer on filePath

    if(!file.is_open()) { // If the file directory is invalid

        cerr << "Error: Could not open file " << filePath << endl;
        return MAP_FILE_NOT_FOUND;

    }

    string line;     // To hold each line of the file
    string section;  // To track which section of the map file is being parsed

    while(getline(file, line)) { // Read file line by line

        line = trim(line);       // Remove spaces and \r
        if(line.empty()) continue; // Skip empty lines

        // Detect section headers
        if (line[0] == '[') {
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

            if (pos == string::npos) { // If there is no '=' character, the line is invalid
                cerr << "Invalid continent line: " << line << endl;
                return MAP_PARSE_ERROR;
            }

            string continentName = trim(line.substr(0, pos)); // Extract continent name
            int bonusValue = 0;

            try { // Try to convert the bonus value to an integer

                bonusValue = stoi(trim(line.substr(pos + 1))); // Attempt to extract and convert bonus value
                continents[continentName] = bonusValue; 

            } catch (const invalid_argument& e) { // String wasn't an integer

                cerr << "Parse error: Continent bonus is not a valid number in line: " << line << endl;
                return MAP_PARSE_ERROR;

            } catch (const out_of_range& e) { // String was an integer, but out of range

                cerr << "Parse error: Continent bonus out of range in line: " << line << endl;
                return MAP_PARSE_ERROR;

            }

        } else if (section == "[Territories]") { // Parse [Territories] section

            vector<string> fields; // To hold the fields of the territory line
            string token;          // Holds each field temporarily for a territory line
            stringstream ss(line); // Use stringstream for easy CSV parsing
            
            while (getline(ss, token, ',')) { // Split line by commas

                string trimmed = trim(token); // Trim leading/trailing spaces
                if (!trimmed.empty()) fields.push_back(trimmed);

            }

            if (fields.size() < 4) { // Must have at least ID, X, Y, Continent

                cerr << "Invalid territory line: " << line << endl;
                return MAP_PARSE_ERROR;
                
            }

            territories.push_back(fields); // Add the parsed territory to the territories vector
        }

        // If section is invalid
        else {
            cerr << "Unknown section: " << section << endl;
            return MAP_INVALID_SECTION;
        }
    }

    file.close();

    //Print out summary of loaded map
    cout << "Map loaded successfully!" << endl;
    cout << "  Continents loaded: " << continents.size() << endl;
    cout << "  Territories loaded: " << territories.size() << endl;

    return MAP_OK;
}

int MapLoader::loadMap(Map* inputMapPtr) {

    if (inputMapPtr == nullptr) {

        cerr << "Error: Input map pointer is null." << endl;
        return INVALID_MAP_PTR;

    }

    // Create a temporary copy of the input map to avoid modifying the original in case of failure
    Map* tempMapPtr = new Map(*inputMapPtr);

    //Load metadata into map object

    if(author.empty()) { //Check author metadata

        cerr << "Error: Author metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return INVALID_AUTHOR;

    } else { tempMapPtr -> setAuthor(author); }

    if(image.empty()) { //Check image metadata

        cerr << "Error: Image metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return INVALID_IMAGE;

    } else { tempMapPtr -> setImage(image); }

    if(wrap.empty()) { //Check wrap metadata

        cerr << "Error: Wrap metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return INVALID_WRAP;

    } else { tempMapPtr -> setWrap(wrap); }

    if(scrollType.empty()) { //Check scrollType metadata

        cerr << "Error: ScrollType metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return INVALID_SCROLL;

    } else { tempMapPtr -> setScrollType(scrollType); }

    if(warn.empty()) { //Check warn metadata

        cerr << "Error: Warn metadata is missing or invalid." << endl;
        delete tempMapPtr;
        return INVALID_WARN;

    } else { tempMapPtr -> setWarn(warn); }

    // Load continents into map object
    if(continents.empty()) {

        cerr << "Error: Continent data is missing or invalid." << endl;
        delete tempMapPtr;
        return INVALID_CONTINENT;

    } else {

        vector<Continent*> continentPtrs; // To hold pointers to created continents

        for (const auto& [name, value] : continents) {

            if (name.empty() || value <= 0) { // Continent name must be non-empty and bonus value positive

                cerr << "Error: Invalid continent data for continent '" << name << "' with bonus value " << value << "." << endl;
                
                for (Continent* c : continentPtrs){ delete c; } // Free previously allocated continent objects
                delete tempMapPtr;

                return INVALID_CONTINENT;

            }

            Continent* newContinent = new Continent(name, value); // Create new continent
            continentPtrs.push_back(newContinent);  // Store pointer to new continent

        }

        tempMapPtr -> setContinents(continentPtrs); // Set continents in the map

    // Load territories into map object
    if(territories.empty()) {

        cerr << "Error: Territory data is missing or invalid." << endl;

        for (Continent* c : continentPtrs){ delete c; }  // Free previously allocated continent objects
        delete tempMapPtr;
        return INVALID_TERRITORY;

    } else {

        for(int i = 0; i < territories.size(); i++) {

            Territory* tempTerritory = new Territory(); //Intialize new territory

            tempTerritory -> setID(territories[i][0]); //Set territory ID
            tempTerritory -> setXCoord(stoi(territories[i][1])); //Set xCoord. This will ALWAYS be a valid integer due to prior validation in importMapInfo
            tempTerritory -> setYCoord(stoi(territories[i][2])); //Set yCoord. This will ALWAYS be a valid integer due to prior validation in importMapInfo
            

            for(int j = 4; j < territories[i].size(); j++){

            }

        }

        return MAP_OK;

    }

}
