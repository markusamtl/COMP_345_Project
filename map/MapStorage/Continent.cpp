#include "Territory.h"
#include "Continent.h"
#include <iostream>

using namespace std;

//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

Continent::Continent() {

    this -> ID = "";
    this -> bonusValue = 0;
    this -> territories = {};

}

Continent::Continent(const string& ID, int bonusValue) {

    this -> ID = ID;
    this -> bonusValue = bonusValue;
    this -> territories = {};

}

Continent::Continent(const string& ID, int bonusValue, const vector<Territory*>& territories) {

    this -> ID = ID;
    this -> bonusValue = bonusValue;
    this -> territories = territories;

}

Continent::~Continent() {

    territories.clear();

}

Continent::Continent(const Continent& other) {

    this -> ID = other.ID;
    this -> bonusValue = other.bonusValue;
    this -> territories = other.territories;

}

Continent& Continent::operator=(const Continent& other) {
    
    if (this != &other) { // self-assignment check
        
        this -> ID = other.ID;
        this -> bonusValue = other.bonusValue;
        this -> territories = other.territories;

    }

    return *this;

}

ostream& operator<<(ostream& os, const Continent& continent) {

    os << "Continent(ID: " << continent.ID 
       << ", Bonus Value: " << continent.bonusValue 
       << ", Territories: [";

    for (size_t i = 0; i < continent.territories.size(); ++i) {
        os << continent.territories[i]->getID();
        if (i < continent.territories.size() - 1) os << ", ";
    }

    os << "])";

    return os;

}

//-- Accessors and Mutators --//

const string& Continent::getID() const { return this -> ID; }
void Continent::setID(const string& ID) { this -> ID = ID; }

int Continent::getBonusValue() const { return this -> bonusValue; }
void Continent::setBonusValue(int bonusValue) { this -> bonusValue = bonusValue; }

const vector<Territory*>& Continent::getTerritories() const { return this -> territories; }
void Continent::setTerritories(const vector<Territory*>& territories) { this -> territories = territories; }

// -- Class Methods -- //

void Continent::addTerritory(Territory* territory) {

    if (territory == nullptr){ return; } // Ignore null pointers
    this->territories.push_back(territory);

}