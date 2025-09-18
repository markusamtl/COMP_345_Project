#include <iostream>
#include "../MapStorage/Territory.h"
#include "../MapStorage/Continent.h"
#include "Map.h"

using namespace std;

//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

Map::Map() {

    this -> author = "";
    this -> image = "";
    this -> wrap = "";
    this -> scrollType = "";
    this -> warn = "";
    this -> territories = {};
    this -> continents = {};
 
}

Map::Map(string author, string image, string wrap, string scrollType, string warn, vector<Territory*> territories, vector<Continent*> continents){
    
    this -> author = author;
    this -> image = image;
    this -> wrap = wrap;
    this -> scrollType = scrollType;
    this -> warn = warn;
    this -> territories = territories;
    this -> continents = continents;

}

Map::~Map() {

    //Free dynamically allocated territories
    for (Territory* t : territories) { delete t; }

    //Free dynamically allocated continents
    for (Continent* c : continents) { delete c; }

}

Map::Map(const Map& other) {
    
    this -> author = other.author;
    this -> image = other.image;
    this -> wrap = other.wrap;
    this -> scrollType = other.scrollType;
    this -> warn = other.warn;
    this -> territories = other.territories;
    this -> continents = other.continents;
    
}

Map& Map::operator=(const Map& other) {
    
    if (this != &other) { // self-assignment check
        
        this -> author = other.author;
        this -> image = other.image;
        this -> wrap = other.wrap;
        this -> scrollType = other.scrollType;
        this -> warn = other.warn;
        this -> territories = other.territories;
        this -> continents = other.continents;

    }

    return *this;
    
}

ostream& operator<<(ostream& os, const Map& map) {

    os << "Author: " << map.author << "\n";
    os << "Image: " << map.image << "\n";
    os << "Wrap: " << map.wrap << "\n";
    os << "Scroll Type: " << map.scrollType << "\n";
    os << "Warn: " << map.warn << "\n";

    os << "Continents:\n";

    for (const auto& continent : map.continents) {

        os << "  " << *continent << "\n";
    
    }

    os << "Territories:\n";

    for (const auto& territory : map.territories) {

        os << "  " << *territory << "\n";
    
    }

    return os;

}

//-- Accessors and Mutators --//

const string& Map::getAuthor() const { return author; }
void Map::setAuthor(string& author) { this -> author = author; }

const string& Map::getImage() const { return image; }
void Map::setImage(string& image) { this -> image = image; }

const string& Map::getWrap() const { return wrap; }
void Map::setWrap(string& wrap) { this -> wrap = wrap; }

const string& Map::getScrollType() const { return scrollType; }
void Map::setScrollType(string& scrollType) { this -> scrollType = scrollType; }

const string& Map::getWarn() const { return warn; }
void Map::setWarn(string& warn) { this -> warn = warn; }

const vector<Continent*>& Map::getContinents() const { return continents; }
void Map::setContinents(const vector<Continent*>& continents) { this -> continents = continents; }

const vector<Territory*>& Map::getTerritories() const { return territories; }
void Map::setTerritories(const vector<Territory*>& territories) { this -> territories = territories; }

//-- Class Methods -- //

void Map::addContinent(Continent* continent) { 

    if (continent == nullptr) return; // Ignore null pointers

    for (Continent* c : this -> continents){ 

        if (c -> getID() == continent -> getID()) return; // Continent already exists

    }

    this -> continents.push_back(continent);
    
}

void Map::addTerritory(Territory* territory) { 

    if (territory == nullptr) return; // Ignore null pointers

    for (Territory* t : this -> territories){ 

        if (t -> getID() == territory -> getID()) return; //Territory already exists

    }

    this -> territories.push_back(territory);
    
}

Territory* Map::getTerritoryByID(const string& ID) {

    for (Territory* t : this -> territories){

        if (t -> getID() == ID){ return t; }

    }

    return nullptr; // Not found


}

Continent* Map::getContinentByID(const string& ID) {

    for (Continent* c : this -> continents){

        if (c -> getID() == ID){ return c;}

    }

    return nullptr; // Not found

}