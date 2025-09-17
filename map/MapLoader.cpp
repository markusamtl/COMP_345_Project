#include "MapLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define MAP_OK 0
#define MAP_FILE_NOT_FOUND 1
#define MAP_INVALID_SECTION 2
#define MAP_PARSE_ERROR 3

using namespace std;

//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

MapLoader::MapLoader() {

    this -> author = "";
    this -> image = "";
    this -> wrap = "";
    this -> scrollType = "";
    this -> warn = "";
    this -> continent = {};
    this -> territories = {};
 
}

MapLoader::MapLoader(string author, string image, string wrap, string scrollType, string warn, map<string, int> continent, vector<vector<string>> territories){
    
    this -> author = author;
    this -> image = image;
    this -> wrap = wrap;
    this -> scrollType = scrollType;
    this -> warn = warn;
    this -> continent = continent;
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
    this -> continent = other.continent;
    this -> territories = other.territories;
 
}

MapLoader& MapLoader::operator=(const MapLoader& other) {
    
    if (this != &other) { // self-assignment check
        
        this -> author = other.author;
        this -> image = other.image;
        this -> wrap = other.wrap;
        this -> scrollType = other.scrollType;
        this -> warn = other.warn;
        this -> continent = other.continent;
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

    for (const auto& [name, value] : mapLoader.continent) {

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

const map<string, int>& MapLoader::getContinent() const { return continent; }
void MapLoader::setContinent(map<string, int> continent) { this -> continent = continent; }

const vector<vector<string>>& MapLoader::getTerritories() const {return territories;}
void MapLoader::setTerritories(vector<vector<string>> territories) { this -> territories = territories; }


//-- Class Methods --//

/**
 * Trims leading and trailing whitespace and carriage returns from a string.
 * @param s The input string to trim.
 * @return The trimmed string.
 */
string MapLoader::trim(const string& s) {

    size_t start = s.find_first_not_of(" \t\r\n"); // Find first non-space character
    size_t end   = s.find_last_not_of(" \t\r\n"); // Find last non-space character
    return (start == string::npos) ? "" : s.substr(start, end - start + 1); // Return trimmed string, or no string if all spaces

}

/**
 * Imports map information from a .map file.
 * @param filePath The path to the .map file.
 * @return MAP_OK (0) on success, or an error code on failure.
 * Error Codes:
 * MAP_FILE_NOT_FOUND (1): File could not be opened.
 * MAP_INVALID_SECTION (2): Encountered an unknown section in the file.
 * MAP_PARSE_ERROR (3): Error parsing a line in the file.
 * 
 */
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
                continent[continentName] = bonusValue; 

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
    cout << "  Continents loaded: " << continent.size() << endl;
    cout << "  Territories loaded: " << territories.size() << endl;

    return MAP_OK;
}
