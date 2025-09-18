#include "Territory.h"
#include <iostream>

using namespace std;

//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

Territory::Territory() {

    this -> ID = "";
    this -> xCoord = 0;
    this -> yCoord = 0;
    this -> neighbors = {};
    this -> continent = nullptr; 
    this -> owner = "";
    this -> numArmies = 0;

}

Territory::Territory(const string &ID) {

    this -> ID = ID;
    this -> xCoord = 0;
    this -> yCoord = 0;
    this -> neighbors = {};
    this -> continent = nullptr; 
    this -> owner = "";
    this -> numArmies = 0;

}

Territory::Territory(const string &ID, int xCoord, int yCoord, const Continent& continent) {

    this -> ID = ID;
    this -> xCoord = xCoord;
    this -> yCoord = yCoord;
    this -> neighbors = {};
    this -> continent = new Continent(continent); // Deep copy of continent
    this -> owner = "";
    this -> numArmies = 0;

}

Territory::Territory(const string& ID, int xCoord, int yCoord, const vector<Territory*>& neighbors, const Continent& continent, const string &owner, int numArmies) {

    this -> ID = ID;
    this -> xCoord = xCoord;
    this -> yCoord = yCoord;
    this -> neighbors = neighbors;
    this -> continent = new Continent(continent); // Deep copy of continent
    this -> owner = owner;
    this -> numArmies = numArmies;

}

Territory::~Territory() {

    neighbors.clear();
    continent = nullptr;

}

//-- Accessors and Mutators --//

const string& Territory::getID() const { return this -> ID;}
void Territory::setID(const string &ID) { this -> ID = ID; }

int Territory::getXCoord() const { return this -> xCoord; }
void Territory::setXCoord(int xCoord) { this -> xCoord = xCoord; }

int Territory::getYCoord() const { return this -> yCoord; }
void Territory::setYCoord(int yCoord) { this -> yCoord = yCoord; }

const vector<Territory*>& Territory::getNeighbors() const { return this -> neighbors; }
void Territory::setNeighbors(const vector<Territory*>& neighbors) { this -> neighbors = neighbors; }

Continent* Territory::getContinent() const {return this -> continent;}
void Territory::setContinent(Continent* continent) { 

    delete this -> continent; // Free existing memory
    this -> continent = new Continent(*continent); // Deep copy of continent

}   

const string& Territory::getOwner() const { return this -> owner; }
void Territory::setOwner(const string &owner) { this -> owner = owner; }

int Territory::getNumArmies() const { return this -> numArmies; }
void Territory::setNumArmies(int numArmies) { this -> numArmies = numArmies; }

//-- Class Methods --//

void Territory::addNeighbor(Territory* neighbor) { 

    if (neighbor == nullptr) return; // Ignore null pointers

    //For all territories n in the neighbors vector, if n's ID is the same as neighbor's ID, return
    for (Territory* n : this -> neighbors){ if (n -> ID == neighbor -> ID) return;}

    this -> neighbors.push_back(neighbor);
    
}