#include "PlayerTerrContainer.h"
#include <iostream>
#include <algorithm>

using namespace std;

//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

PlayerTerrContainer::PlayerTerrContainer() {

    this -> territories = {};
    this -> territoryIndex = {};
    
}

PlayerTerrContainer::PlayerTerrContainer(vector<WarzoneMap::Territory*> territories, unordered_map<string, WarzoneMap::Territory*> territoryIndex) {
    
    this -> territories = territories;
    this -> territoryIndex = territoryIndex;

}

PlayerTerrContainer::~PlayerTerrContainer() {

    territories.clear();
    territoryIndex.clear();

}

PlayerTerrContainer::PlayerTerrContainer(const PlayerTerrContainer& other) {

    territories = other.territories;
    territoryIndex = other.territoryIndex;

}

PlayerTerrContainer& PlayerTerrContainer::operator=(const PlayerTerrContainer& other) {

    if (this != &other) {

        territories = other.territories;
        territoryIndex = other.territoryIndex;
    
    }

    return *this;

}

ostream& operator<<(ostream& os, const PlayerTerrContainer& pc) {

    os << "Owned Territories (" << pc.territories.size() << "): [";
    
    for (size_t i = 0; i < pc.territories.size(); i++) {
        
        os << pc.territories[i]->getID();
        
        if (i < pc.territories.size() - 1) os << ", ";
    
    }

    return os << "]\n";

}

//-- Accessor and Mutators --//

const vector<WarzoneMap::Territory*>& PlayerTerrContainer::getTerritories() const {

    return territories;

}

void PlayerTerrContainer::setTerritories(const vector<WarzoneMap::Territory*>& newTerritories) {

    territories = newTerritories; //Overwrite old territory vector 

    // Sync hashmap to match
    territoryIndex.clear(); //Clear old hashmap

    for(WarzoneMap::Territory* t : territories) { //Link hashmap to IDs

        if(t != nullptr) {

            territoryIndex[t->getID()] = t;
        
        }
    
    }

}

const unordered_map<string, WarzoneMap::Territory*>& PlayerTerrContainer::getTerritoryIndex() const {

    return territoryIndex;

}

void PlayerTerrContainer::setTerritoryIndex(const unordered_map<string, WarzoneMap::Territory*>& newIndex) {

    territoryIndex = newIndex; //Overwrite old territory ID vector 
    
    // Sync vector to match
    territories.clear();
    
    for(const pair<const string, WarzoneMap::Territory*>& kv : territoryIndex) { //Loop like this since this is the hashmap structure

        territories.push_back(kv.second);
    }

}


//-- Class Methods --//

void PlayerTerrContainer::addTerritory(WarzoneMap::Territory* t) {

    //Check if inputted territory is a null pointer, or if it already exists in the container
    if(this -> owns(t)){ return; }

    //Insert into container
    territories.push_back(t); //vector
    territoryIndex[t -> getID()] = t; //hashmap

}

void PlayerTerrContainer::removeTerritory(WarzoneMap::Territory* t) {
    
    //Check if inputted territory is a null pointer, or if it does NOT exist in the container
    if (!(this -> owns(t))){ return; }

    //Remove from hashmap
    territoryIndex.erase(territoryIndex.find(t -> getID()));

    //Remove from vector
    territories.erase(

        remove(territories.begin(), territories.end(), t), //Remove undesired territory
        territories.end() //Removes extra undesired memory from the vector

    );

}

bool PlayerTerrContainer::owns(WarzoneMap::Territory* t) const {

    //Check if inputted territory is a null pointer
    if(t == nullptr){ return false; }

    return territoryIndex.count(t -> getID()) > 0; //Check if the given territory's ID exists in the hashmap

}

void PlayerTerrContainer::clear() {

    //Territories are ONLY deleted when the map is deleted; map should be the ONLY territory owner.
    territories.clear();
    territoryIndex.clear();

}

size_t PlayerTerrContainer::size() const {

    return territories.size();

}