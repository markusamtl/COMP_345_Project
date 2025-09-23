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

    territories.clear();

    //Free dynamically allocated continents
    for (Continent* c : continents) { delete c; }

    continents.clear();

}

Map& Map::operator=(const Map& other) {

    if (this != &other) { // self-assignment guard

        // --- 1. Clean up current memory ---
        for (Territory* t : territories) delete t;
        territories.clear();

        for (Continent* c : continents) delete c;
        continents.clear();

        // --- 2. Copy metadata ---
        author = other.author;
        image = other.image;
        wrap = other.wrap;
        scrollType = other.scrollType;
        warn = other.warn;

        // --- 3. Deep copy continents ---
        unordered_map<Continent*, Continent*> continentMap;

        for (Continent* currCont : other.continents) {

            Continent* newCont = new Continent(*currCont); // shallow copy of ID/bonus
            continents.push_back(newCont);
            continentMap[currCont] = newCont;

        }

        // --- 4. Deep copy territories ---
        unordered_map<Territory*, Territory*> terrMap;

        for (Territory* t : other.territories) {

            Territory* newTerr = new Territory(*t); // shallow copy (neighbors/continent fixed later)
            territories.push_back(newTerr);
            terrMap[t] = newTerr;

        }

        // --- 5. Fix neighbors and continent pointers ---
        for (Territory* oldTerr : other.territories) {

            Territory* newTerr = terrMap[oldTerr];

            // Fix neighbors
            vector<Territory*> newNeighbors;

            for (Territory* oldNeigh : oldTerr->getNeighbors()) {

                newNeighbors.push_back(terrMap[oldNeigh]);

            }

            newTerr->setNeighbors(newNeighbors);

            // Fix continent pointer
            Continent* oldCon = oldTerr->getContinent();

            if (oldCon && continentMap.count(oldCon)) {

                Continent* newCon = continentMap[oldCon];
                newTerr->setContinent(newCon);
                newCon->addTerritory(newTerr);

            }

        }

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

    for(const auto& continent : map.continents) {

        os << "  " << *continent << "\n";
    
    }

    os << "Territories:\n";

    for(const auto& territory : map.territories) {

        os << "  " << *territory << "\n";
    
    }

    return os;

}

//-- Accessors and Mutators --//

const string& Map::getAuthor() const { return author; }
void Map::setAuthor(const string& author) { this -> author = author; }

const string& Map::getImage() const { return image; }
void Map::setImage(const string& image) { this -> image = image; }

const string& Map::getWrap() const { return wrap; }
void Map::setWrap(const string& wrap) { this -> wrap = wrap; }

const string& Map::getScrollType() const { return scrollType; }
void Map::setScrollType(const string& scrollType) { this -> scrollType = scrollType; }

const string& Map::getWarn() const { return warn; }
void Map::setWarn(const string& warn) { this -> warn = warn; }

const vector<Continent*>& Map::getContinents() const { return continents; }
void Map::setContinents(const vector<Continent*>& continents) { this -> continents = continents; }

const vector<Territory*>& Map::getTerritories() const { return territories; }
void Map::setTerritories(const vector<Territory*>& territories) { this -> territories = territories; }

//-- Class Methods -- //

void Map::addContinent(Continent* continent) { 

    if(continent == nullptr) return; // Ignore null pointers

    for(Continent* c : this -> continents){ 

        if(c -> getID() == continent -> getID()) return; // Continent already exists

    }

    this -> continents.push_back(continent);
    
}

void Map::addTerritory(Territory* territory) { 

    if(territory == nullptr) return; // Ignore null pointers

    for(Territory* t : this -> territories) { 

        if(t -> getID() == territory -> getID()) return; //Territory already exists

    }

    this -> territories.push_back(territory);
    
}

Territory* Map::getTerritoryByID(const string& ID) {

    for(Territory* t : this -> territories){

        if(t -> getID() == ID){ return t; }

    }

    return nullptr; // Not found


}

Continent* Map::getContinentByID(const string& ID) {

    for(Continent* c : this -> continents){

        if (c -> getID() == ID){ return c;}

    }

    return nullptr; // Not found

}


bool Map::isMapConnectedDFS(const vector<Territory*>& terrList, Continent* restrictCont) const {
    
    if(terrList.empty()){ return false; } //If territories are empty, stop

    int totalTerritories = static_cast<int>(terrList.size());
    unordered_map<Territory*, int> terrIndex; //Set up hashmap, Territory* -> index of territory in terrList

    for (int i = 0; i < totalTerritories; i++) { terrIndex[terrList[i]] = i;} //Map each territory to its index in the hashmap

    vector<bool> visited(totalTerritories, false); //Create a visited array to track visited territories
    stack<Territory*> toVisit; //Create a stack for DFS traversal. Currently unvisited territories will be pushed onto this stack, popped off once visited

    Territory* start = terrList[0]; //Start from the first territory in the list
    toVisit.push(start); //Push it onto the stack
    visited[terrIndex[start]] = true; //Mark it as visited

    while(!toVisit.empty()) { //Iterate through the stack until it's empty

        Territory* curTerr = toVisit.top();
        toVisit.pop();

        for(Territory* neighbor : curTerr -> getNeighbors()) { //For each neighbor of the current territory

            if(neighbor == nullptr) { continue; } //Skip for empty neighbours 

            //If the method is restricted to a continent, skip neighbors outside it
            if(restrictCont && neighbor->getContinent() != restrictCont) { continue; }

            int neighIndex = terrIndex[neighbor];

            if(!visited[neighIndex]) { //If the neighbor hasn't been visited yet

                visited[neighIndex] = true; //Mark it as visited
                toVisit.push(neighbor);

            }

        }
        
    }

    int totalVisited = 0;
    for (bool v : visited) { if (v) totalVisited++; } // Count visited territories

    return (totalVisited == totalTerritories); //If all territories were visited, the graph is connected

}

bool Map::validate() const {
    
    // -- 1. Non-empty checks --
    if(territories.empty()) { //If no territories exist in the map

        cerr << "Validation failed: Map has no territories." << endl;
        return false;
    
    }

    if(continents.empty()) { //If no continents exist in the map
      
        cerr << "Validation failed: Map has no continents." << endl;
        return false;
    
    }

    //-- 2. Each territory must belong to a continent --
    for(Territory* t : territories) { //For each territory in the map

        if(t -> getContinent() == nullptr) { //If the territory does not belong to any continent
            
            cerr << "Validation failed: Territory '" << t->getID() << "' does not belong to any continent." << endl;
            return false;

        }

    }

    //-- 3. Check overall map connectivity --
    if (!isMapConnectedDFS(territories, nullptr)) { // No continent restriction, perform DFS on the WHOLE map

        cerr << "Validation failed: Map is not fully connected." << endl;
        return false;

    }

    //-- 4. Check continent connectivity --
    for(Continent* cont : continents) { // For each continent
        
        const vector<Territory*>& terrList = cont->getTerritories(); //Get current continent's territories

        if(terrList.empty()) { //If no territories are associated with the continent
            
            cerr << "Validation failed: Continent '" << cont->getID() << "' has no territories." << endl;
            return false;
        
        }

        if(!isMapConnectedDFS(terrList, cont)) { //Perform DFS for the continent's territories only
            
            cerr << "Validation failed: Continent '" << cont->getID() << "' is not a connected subgraph." << endl;
            return false;

        }

    }

    cout << "Validation successful: Map is valid." << endl;
    return true;

}