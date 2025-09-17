#include "MapLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define MAP_OK 0
#define MAP_FILE_NOT_FOUND 1
#define MAP_INVALID_SECTION 2
#define MAP_PARSE_ERROR 3

using namespace std;

//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator  --//

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

int MapLoader::importMapInfo(const string& filePath) {
    
    ifstream file(filePath); //Open file buffer on filePath

    if(!file.is_open()) { //If the file directory is invalid

        cerr << "Error: Could not open file " << filePath << endl;
        return MAP_FILE_NOT_FOUND;

    }

    string line, section; //Keep track of current file line, and map section

    while(getline(file, line)) { //Read file line by line

        if(line.empty()) continue;

        // Detect section headers
        if (line[0] == '[') {
            section = line;
            continue;
        }

        // Parse map metadata
        if (section == "[Map]") {

            if(line.find("author=") == 0) author = line.substr(7);
            else if (line.find("image=") == 0) image = line.substr(6);
            else if (line.find("wrap=") == 0) wrap = line.substr(5);
            else if (line.find("scroll=") == 0) scrollType = line.substr(7);
            else if (line.find("warn=") == 0) warn = line.substr(5);
            else {
                cerr << "Invalid line in [Map]: " << line << endl;
                return MAP_PARSE_ERROR;
            }

        }

        // Parse [Continents]
        else if (section == "[Continents]") {
            size_t pos = line.find('=');
            if (pos == string::npos) {
                cerr << "Invalid continent line: " << line << endl;
                return MAP_PARSE_ERROR;
            }
            string name = line.substr(0, pos);
            int value = stoi(line.substr(pos + 1));
            continent[name] = value;
        }

        // Parse [Territories]
        else if (section == "[Territories]") {
            vector<string> fields;
            string token;
            stringstream ss(line);
            while (getline(ss, token, ',')) {
                // trim whitespace
                size_t start = token.find_first_not_of(" \t");
                size_t end   = token.find_last_not_of(" \t");
                if (start != string::npos)
                    fields.push_back(token.substr(start, end - start + 1));
            }
            if (fields.size() < 4) { // must have ID/Name, X, Y, Continent
                cerr << "Invalid territory line: " << line << endl;
                return MAP_PARSE_ERROR;
            }
            territories.push_back(fields);
        }

        // Unknown section
        else {
            cerr << "Unknown section: " << section << endl;
            return MAP_INVALID_SECTION;
        }
    }

    file.close();
    return MAP_OK;
}