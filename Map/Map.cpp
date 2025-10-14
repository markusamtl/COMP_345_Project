#include "Map.h"
#include "../Player/Player.h"

using namespace std;


namespace WarzoneMap {

    // ================= StringHandling =================

    string StringHandling::trim(const string& inputStr) {

        size_t start = inputStr.find_first_not_of(" \t\r\n"); // Find first non-space character
        size_t end   = inputStr.find_last_not_of(" \t\r\n"); // Find last non-space character
        return (start == string::npos) ? "" : inputStr.substr(start, end - start + 1); // Return trimmed string, or no string if all spaces

    }


    pair<bool, int> StringHandling::isStrInt(const string& inputStr) {

        int retVal = 0;

        try {

            retVal = stoi(inputStr);  // attempt to convert string to int

        } catch (const invalid_argument& e) {

            cerr << "Parse error: string is not a valid integer: " << inputStr << endl;
            return {false, -1};

        } catch (const out_of_range& e) {

            cerr << "Parse error: integer value out of range: " << inputStr << endl;
            return {false, -1};


        }

        return {true, retVal};

    }

    // ================= Continent =================

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
        this -> territories.clear(); //Map fills in the territories

    }

    Continent& Continent::operator=(const Continent& other) {
        
        if (this != &other) { // self-assignment check
            
            this -> ID = other.ID;
            this -> bonusValue = other.bonusValue;
            this -> territories.clear();

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Continent& continent) {

        os << "Continent(ID: " << continent.ID 
        << ", Bonus Value: " << continent.bonusValue 
        << ", Territories: [";

        for(size_t i = 0; i < continent.territories.size(); ++i) {
            os << continent.territories[i] -> getID();
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

        if(territory == nullptr){ return; } // Ignore null pointers
        this->territories.push_back(territory);

    }

    // ================= Territory =================

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Territory::Territory() {

        this -> ID = "";
        this -> xCoord = 0;
        this -> yCoord = 0;
        this -> neighbors = {};
        this -> continent = nullptr; 
        this -> owner = nullptr;
        this -> numArmies = 1;
        this -> numericTerrID = 0;

    }

    Territory::Territory(const string &ID) {

        this -> ID = ID;
        this -> xCoord = 0;
        this -> yCoord = 0;
        this -> neighbors = {};
        this -> continent = nullptr; 
        this -> owner = nullptr;
        this -> numArmies = 1;
        computeNumericTerrID();

    }

    Territory::Territory(const string &ID, int xCoord, int yCoord, Continent* continent) {

        this -> ID = ID;
        this -> xCoord = xCoord;
        this -> yCoord = yCoord;
        this -> neighbors = {};
        this -> continent = continent; 
        this -> owner = nullptr;
        this -> numArmies = 1;
        computeNumericTerrID();

    }

    Territory::Territory(const string& ID, int xCoord, int yCoord, const vector<Territory*>& neighbors, Continent* continent, Player* owner, int numArmies) {

        this -> ID = ID;
        this -> xCoord = xCoord;
        this -> yCoord = yCoord;
        this -> neighbors = neighbors;
        this -> continent = continent; 
        this -> owner = owner;
        this -> numArmies = numArmies;
        computeNumericTerrID();

    }

    Territory::~Territory() {

        neighbors.clear();
        continent = nullptr;

    }

    Territory::Territory(const Territory& other) {

        this -> ID = other.ID;
        this -> xCoord = other.xCoord;
        this -> yCoord = other.yCoord;
        this -> neighbors = other.neighbors;
        this -> continent = other.continent; 
        this -> owner = other.owner;
        this -> numArmies = other.numArmies;
        this -> numericTerrID = other.numericTerrID;

    }

    Territory& Territory::operator=(const Territory& other) {
        
        if (this != &other) { // self-assignment check
            
            this -> ID = other.ID;
            this -> xCoord = other.xCoord;
            this -> yCoord = other.yCoord;
            this -> neighbors = other.neighbors;
            this -> continent = other.continent; 
            this -> owner = other.owner;
            this -> numArmies = other.numArmies;
            this -> numericTerrID = other.numericTerrID;

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Territory& territory) {

        os << "Territory(ID: " << territory.ID 
        << ", Coordinates: (" << territory.xCoord << ", " << territory.yCoord << ")"
        << ", Neighbors: [";

        for (size_t i = 0; i < territory.neighbors.size(); ++i) {
            os << territory.neighbors[i]->ID;
            if (i < territory.neighbors.size() - 1) {
                os << ", ";
            }
        }

        os << "], Continent: " << (territory.continent ? territory.continent -> getID() : "None") 
        << ", Owner: "  << (territory.getOwner() ? territory.getOwner() -> getPlayerName() : "None")
        << ", Num Armies: " << territory.numArmies
        << ", NumericID: " << territory.numericTerrID
        << ")";

        return os;

    }

    //-- Accessors and Mutators --//

    const string& Territory::getID() const { return this -> ID;}
    void Territory::setID(const string &ID){ 
        
        this -> ID = ID; 
        computeNumericTerrID();

    }

    int Territory::getXCoord() const { return this -> xCoord; }
    void Territory::setXCoord(int xCoord) { this -> xCoord = xCoord; }

    int Territory::getYCoord() const { return this -> yCoord; }
    void Territory::setYCoord(int yCoord) { this -> yCoord = yCoord; }

    const vector<Territory*>& Territory::getNeighbors() const { return this -> neighbors; }
    void Territory::setNeighbors(const vector<Territory*>& neighbors) { this -> neighbors = neighbors; }

    Continent* Territory::getContinent() const {return this -> continent;}
    void Territory::setContinent(Continent* continent) { this -> continent = continent; }   

    Player* Territory::getOwner() const { return this -> owner; }
    void Territory::setOwner(Player* owner) { this -> owner = owner; }

    int Territory::getNumArmies() const { return this -> numArmies; }
    void Territory::setNumArmies(int numArmies) { this -> numArmies = numArmies; }

    long long WarzoneMap::Territory::getNumericTerrID() { return this -> numericTerrID; }

    //-- Class Methods --//

    void Territory::addNeighbor(Territory* neighbor) { 

        if(neighbor == nullptr) return; // Ignore null pointers

        //For all territories n in the neighbors vector, if n's ID is the same as neighbor's ID, return
        for(Territory* n : this -> neighbors){ 
            
            if(n -> getNumericTerrID() == neighbor -> getNumericTerrID()) { return; }

        }

        this -> neighbors.push_back(neighbor);
        
    }

    bool Territory::territoryIDCompare(Territory* a, Territory* b) {

        string idA = a -> getID();
        string idB = b -> getID();

        //This method thinks ahead for cases like AA10 vs A10 (A10 should come first)

        size_t i = 0, j = 0; //Keep track of indexes in ID string
        
        while(i < idA.size() && j < idB.size()) {

            if(std::isdigit(idA[i]) && std::isdigit(idB[j])) { //Extract full number substrings

                size_t iStart = i, jStart = j;

                while(i < idA.size() && isdigit(idA[i])){ i++; } //Iterate over the numerical values of the first ID 
                while(j < idB.size() && isdigit(idB[j])){ j++; } //Iterate over the numerical values of the first ID 

                int numA = stoi(idA.substr(iStart, i - iStart)); //Cast the number
                int numB = stoi(idB.substr(jStart, j - jStart)); //Cast the number

                if(numA != numB) { return numA < numB; }

            } else { //Compare as characters

                if(idA[i] != idB[j]){ return idA[i] < idB[j]; }
                i++; 
                j++;

            }

        }

        //If one string is a prefix of the other, shorter comes first
        return idA.size() < idB.size();
        
    }

    bool Territory::territoryNumArmiesCompareDescend(Territory* a, Territory* b) {

        if(a == nullptr || b == nullptr){ return false; }
        return a -> getNumArmies() > b -> getNumArmies(); //More armies = higher priority

    }

    bool Territory::territoryNumArmiesCompareAscend(Territory* a, Territory* b) {

        if(a == nullptr || b == nullptr){ return false; }
        return a -> getNumArmies() < b -> getNumArmies(); //Lower armies = higher priority

    }

    bool Territory::territoryAttackPriorityCompare(Territory* a, Territory* b) {

        if(a == nullptr || b == nullptr){ return false; } //Safety check

        Player* ownerA = a -> getOwner();
        Player* ownerB = b -> getOwner();

        //Fallback to large value if owner is null (shouldn’t happen, just in case)
        int territoriesA = (ownerA ? static_cast<int>(ownerA -> getOwnedTerritories().size()) : INT_MAX);
        int territoriesB = (ownerB ? static_cast<int>(ownerB -> getOwnedTerritories().size()) : INT_MAX);

        //1. Strategic priority — focus on knocking out the weaker opponent first
        if(territoriesA != territoriesB) { return territoriesA < territoriesB; }

        //2. Strategic reward — prefer attacking territories in high-bonus continents
        Continent* contA = a -> getContinent();
        Continent* contB = b -> getContinent();

        if(contA != nullptr && contB != nullptr) {

            int bonusA = contA -> getBonusValue();
            int bonusB = contB -> getBonusValue();

            if(bonusA != bonusB) { return bonusA > bonusB; } //Higher continent bonus = higher attack priority

        }

        //3. Tactical weakness — fewer defenders come last
        return a -> getNumArmies() < b -> getNumArmies();

    }

    bool Territory::territoryThreatCompareDescend(Territory* a, Territory* b) {

        if(a == nullptr || b == nullptr){ return false; }

        //Compute threat ratio for a
        double enemySumA = 0.0;
        vector<Territory*> neighborsA = a -> getNeighbors();
        Player* ownerA = a -> getOwner();

        if(ownerA == nullptr){ return false; }

        vector<Player*> trucesA = ownerA -> getNeutralEnemies();

        for(Territory* neigh : neighborsA){

            if(neigh == nullptr){ continue; }

            Player* neighOwner = neigh -> getOwner();
            if(neighOwner == nullptr){ continue; }

            //Ignore self-owned, neutral, or truce neighbours
            if(neighOwner == ownerA){ continue; }
            if(find(trucesA.begin(), trucesA.end(), neighOwner) != trucesA.end()){ continue; }

            enemySumA += neigh -> getNumArmies();

        }

        double threatA = (enemySumA + 1.0) / (static_cast<double>(a -> getNumArmies()) + 1.0);

        //Compute threat ratio for Territory B
        double enemySumB = 0.0;
        vector<Territory*> neighborsB = b -> getNeighbors();
        Player* ownerB = b -> getOwner();

        if(ownerB == nullptr){ return false; }

        vector<Player*> trucesB = ownerB -> getNeutralEnemies();

        for(Territory* neigh : neighborsB){

            if(neigh == nullptr){ continue; }

            Player* neighOwner = neigh -> getOwner();
            if(neighOwner == nullptr){ continue; }

            //Ignore self-owned, neutral, or truce neighbours
            if(neighOwner == ownerB){ continue; }
            if(find(trucesB.begin(), trucesB.end(), neighOwner) != trucesB.end()){ continue; }

            enemySumB += neigh -> getNumArmies();

        }

        double threatB = (enemySumB + 1.0) / (static_cast<double>(b -> getNumArmies()) + 1.0);

        //Compare both Territories. Primary key: higher threat ratio first (more threatened)

        if(abs(threatA - threatB) > 1e-6){ return threatA > threatB; }

        //Secondary key: Whoever has fewer armies first
        int armiesA = a -> getNumArmies();
        int armiesB = b -> getNumArmies();

        if(armiesA != armiesB){ return armiesA < armiesB; }

        //Just return false, this should not change anything in the grand scheme of things
        else{ return false; }

    }

    void Territory::computeNumericTerrID() {

        const uint64_t FNV_OFFSET = 1469598103934665603ULL;
        const uint64_t FNV_PRIME  = 1099511628211ULL;

        uint64_t hash = FNV_OFFSET;

        // Apply XOR and multiply for each character in the ID
        for(unsigned char c : ID) {

            hash ^= static_cast<uint64_t>(c);
            hash *= FNV_PRIME;
        
        }

        //Reduce to a fixed 64-bit prime modulus to keep magnitudes reasonable
        const uint64_t MOD_LIMIT = 29996224275833ULL; //1 trillionith prime
        hash = hash % MOD_LIMIT;

        // Store as signed long long
        this -> numericTerrID = static_cast<long long>(hash);

    }

    // ================= Map =================

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

     Map::Map() {

        this -> author = "";
        this -> image = "";
        this -> wrap = "";
        this -> scrollType = "";
        this -> warn = "";
        this -> mapName = "";
        this -> territories = {};
        this -> continents = {};
        this -> continentLookupTable = {};

    }

    Map::Map(string author, string image, string wrap, string scrollType, string warn, string mapName,
             vector<Territory*> territories, vector<Continent*> continents,
             unordered_map<Continent*, long long> continentLookupTable) {

        this -> author = author;
        this -> image = image;
        this -> wrap = wrap;
        this -> scrollType = scrollType;
        this -> warn = warn;
        this -> mapName = mapName;
        this->territories = territories;
        this->continents = continents;
        this->continentLookupTable = continentLookupTable;

    }

    Map::~Map() {

        // Free dynamically allocated territories
        for(Territory* t : territories){ delete t; }
        territories.clear();

        // Free dynamically allocated continents
        for(Continent* c : continents){ delete c; }
        continents.clear();

        //Clear continent hashmap
        continentLookupTable.clear();

    }

    Map::Map(const Map& other) {

        // Copy metadata
        this -> author = other.author;
        this -> image = other.image;
        this -> wrap = other.wrap;
        this -> scrollType = other.scrollType;
        this -> warn = other.warn;
        this -> mapName = other.mapName;

        // Deep copy continents
        unordered_map<Continent*, Continent*> contMap;

        for(Continent* c : other.continents) {

            Continent* newCont = new Continent(*c);
            this->continents.push_back(newCont);
            contMap[c] = newCont;

        }

        // Deep copy territories
        unordered_map<Territory*, Territory*> terrMap;
        for (Territory* t : other.territories) {

            Territory* newTerr = new Territory(*t);
            this->territories.push_back(newTerr);
            terrMap[t] = newTerr;

        }

        // Fix neighbors + continent pointers
        for(Territory* oldTerr : other.territories) {

            Territory* newTerr = terrMap[oldTerr];

            // Neighbors
            vector<Territory*> newNeighbors;
            for(Territory* oldNeigh : oldTerr -> getNeighbors()) { newNeighbors.push_back(terrMap[oldNeigh]); }

            newTerr->setNeighbors(newNeighbors);

            // Continent
            Continent* oldCont = oldTerr -> getContinent();
            if(oldCont && contMap.count(oldCont)) {

                Continent* newCont = contMap[oldCont];
                newTerr->setContinent(newCont);
                newCont->addTerritory(newTerr);

            }

        }

        // Rebuild continent lookup
        buildContinentHashmap();

    }

    Map& Map::operator=(const Map& other) {

        if (this != &other) {

            // Clean up
            for(Territory* t : territories){ delete t; }
            territories.clear();
            
            for(Continent* c : continents){ delete c; }
            continents.clear();
            
            continentLookupTable.clear();

            // Copy metadata
            author = other.author;
            image = other.image;
            wrap = other.wrap;
            scrollType = other.scrollType;
            warn = other.warn;
            mapName = other.mapName;

            // Deep copy continents
            unordered_map<Continent*, Continent*> contMap;
            for(Continent* c : other.continents) {

                Continent* newCont = new Continent(*c);
                continents.push_back(newCont);
                contMap[c] = newCont;

            }

            // Deep copy territories
            unordered_map<Territory*, Territory*> terrMap;
            for(Territory* t : other.territories) {

                Territory* newTerr = new Territory(*t);
                territories.push_back(newTerr);
                terrMap[t] = newTerr;

            }

            // Fix neighbors + continent pointers
            for(Territory* oldTerr : other.territories) {

                Territory* newTerr = terrMap[oldTerr];

                vector<Territory*> newNeighbors;
                for (Territory* oldNeigh : oldTerr -> getNeighbors()){ newNeighbors.push_back(terrMap[oldNeigh]); }
                newTerr->setNeighbors(newNeighbors);

                Continent* oldCont = oldTerr -> getContinent();
                if (oldCont && contMap.count(oldCont)) {

                    Continent* newCont = contMap[oldCont];
                    newTerr->setContinent(newCont);
                    newCont->addTerritory(newTerr);

                }

            }

            // Rebuild continent lookup
            buildContinentHashmap();

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Map& map) {
        
        os << "Author: " << map.author << "\n";
        os << "Image: " << map.image << "\n";
        os << "Wrap: " << map.wrap << "\n";
        os << "Scroll Type: " << map.scrollType << "\n";
        os << "Warn: " << map.warn << "\n";
        os << "Map Name: " << map.mapName << "\n";

        os << "Continents:\n";
        for(const auto& continent : map.continents) { os << "  " << *continent << "\n"; }

        os << "Territories:\n";
        for (const auto& territory : map.territories) { os << "  " << *territory << "\n"; }

        //NOT necessary to see the hashmap here

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

    const string& Map::getMapName() const { return mapName; }
    void Map::setMapName(const string mapName){ this -> mapName = mapName; }

    const vector<Continent*>& Map::getContinents() const { return continents; }
    void Map::setContinents(const vector<Continent*>& continents) { this -> continents = continents; }

    const vector<Territory*>& Map::getTerritories() const { return territories; }
    void Map::setTerritories(const vector<Territory*>& territories) { this -> territories = territories; }

    const unordered_map<Continent*, long long>& Map::getContinentLookupTable() const{ return continentLookupTable; }
    void Map::setContinentLookupTable(const unordered_map<Continent*, long long> continentLookupTable){this -> continentLookupTable = continentLookupTable; }


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

    Territory* Map::getTerritoryByNumID(long long ID) {

        for(Territory* t : this -> territories){

            if(t -> getNumericTerrID() == ID){ return t; }

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
                
                cerr << "Validation failed: Territory '" << t -> getID() << "' does not belong to any continent." << endl;
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
                
                cerr << "Validation failed: Continent '" << cont -> getID() << "' has no territories." << endl;
                return false;
            
            }

            if(!isMapConnectedDFS(terrList, cont)) { //Perform DFS for the continent's territories only
                
                cerr << "Validation failed: Continent '" << cont -> getID() << "' is not a connected subgraph." << endl;
                return false;

            }

        }

        return true;

    }

    void Map::buildContinentHashmap() {

        continentLookupTable.clear(); //Remove old table

        for(Continent* c : continents) { //Iterate over all continents

            if(c != nullptr) {
                
                long long sum = 0ll;

                for (Territory* t : c -> getTerritories()) { //Iterate over all territories in a continent

                    if(t != nullptr){ sum += t -> getNumericTerrID(); }

                }

                continentLookupTable[c] = sum; //Set up KVP

            }

        }

    }

    unordered_map<Continent*, long long> Map::buildEmptyContinentHashmap() const {

        unordered_map<Continent*, long long> emptyRetMap;

        for (Continent* c : continents) {
            
            if(c != nullptr) { emptyRetMap[c] = 0ll; }
        
        }

        return emptyRetMap;

    }

    vector<Territory*> Map::shortestPathBetweenTerritories(Territory* start, Territory* goal) {

        //Safety Checks
        if(start == nullptr || goal == nullptr) { return {}; }
        if(start == goal) { return { start }; }

        //Instantiate relevant structures
        unordered_map<Territory*, int> distance; //Shortest known distance from start
        unordered_map<Territory*, Territory*> previous; //Predecessor for path reconstruction
        unordered_set<Territory*> visited; //Visited set to prevent reprocessing


        // Comparator structure for priority queue
        struct TerritoryDistanceCompare {

            unordered_map<Territory*, int>* distancePtr;

            //Constructor for struct
            TerritoryDistanceCompare(unordered_map<Territory*, int>* dist) { distancePtr = dist; }

            //Comparator definition
            bool operator()(Territory* left, Territory* right) const { return (*distancePtr)[left] > (*distancePtr)[right]; }

        };

        //Define priority queue
        TerritoryDistanceCompare comparator(&distance);

        //Invert heap access since C++ priority_queues are max-heaps by default; use a custom comparator to make it act as a min-heap        
        priority_queue<Territory*, vector<Territory*>, TerritoryDistanceCompare> frontier(comparator);

        //Intialize default values
        for(Territory* t : this -> getTerritories()) {

            if(t == nullptr) { continue; } //Ignore null pointers

            distance[t] = INT_MAX; //Initialize all distances to "infinity"
            previous[t] = nullptr; //Set previous territory pointer to be null

        }

        //Intialize default values for starting territory
        distance[start] = 0; 
        frontier.push(start);

        //Execute Djkistra's
        while(!frontier.empty()) {

            Territory* current = frontier.top(); //Get current neighbouring territory
            frontier.pop();

            //Skip already visited territories
            if(visited.find(current) != visited.end()) { continue; }
            
            //Mark current territory as visited
            visited.insert(current); 

            //Goal found
            if(current == goal) { break; }

            //Iterate through all neighbors
            for(Territory* neighbor : current -> getNeighbors()) {

                if(neighbor == nullptr) { continue; }

                //The distance to the neighbor from start is the distance from the current node++ (current holds the distance from start)
                int altDistance = distance[current] + 1;

                //If this new path to the neighbor is shorter than the previously known path,
                //update the neighbor's shortest distance and record that it was reached through the current node.
                //Then, add the neighbor to the priority queue for further exploration.
                if(altDistance < distance[neighbor]) {

                    distance[neighbor] = altDistance; //Record new shortest known distance to neighbor
                    previous[neighbor] = current; //Store path predecessor for reconstruction later
                    frontier.push(neighbor); //Schedule neighbor for expansion in the frontier

                }


            }

        }

        //Rebuild path
        vector<Territory*> shortestPath;
        Territory* step = goal;

        //Backtrack from goal to start
        while(step != nullptr) {

            shortestPath.push_back(step);
            step = previous[step];

        }

        //If start wasn't reached, there is no path
        if(shortestPath.back() != start) { return {}; }

        //Reverse shortestPath to be start to goal
        reverse(shortestPath.begin(), shortestPath.end());
        return shortestPath;

    }

    // ================= MapLoader =================

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

    MapLoader::MapLoader() {

        this -> author = "";
        this -> image = "";
        this -> wrap = "";
        this -> scrollType = "";
        this -> warn = "";
        this -> mapName = mapName;
        this -> continents = {};
        this -> territories = {};
    
    }

    MapLoader::MapLoader(string author, string image, string wrap, string scrollType, string warn, string mapName,
                         map<string, int> continents, vector<vector<string>> territories){
        
        this -> author = author;
        this -> image = image;
        this -> wrap = wrap;
        this -> scrollType = scrollType;
        this -> warn = warn;
        this -> mapName = mapName;
        this -> continents = continents;
        this -> territories = territories;

    }

    MapLoader::~MapLoader(){} // No dynamic memory to free

    MapLoader::MapLoader(const MapLoader& other) {

        this -> author = other.author;
        this -> image = other.image;
        this -> wrap = other.wrap;
        this -> scrollType = other.scrollType;
        this -> warn = other.warn;
        this -> mapName = other.mapName;
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
            this -> mapName = other.mapName;
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
        os << "Map Name:" << mapLoader.mapName << "n";

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

    const string& MapLoader::getMapName() const { return mapName; }
    void MapLoader::setMapName(const string& mapName) { this -> mapName = mapName; }

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

        //Extract map name from file path
        size_t lastSlash = filePath.find_last_of("/\\"); //Find last slash (Windows or Unix)
        size_t lastDot = filePath.find_last_of('.'); //Find last dot before extension

        if(lastDot == string::npos || lastDot <= lastSlash) { //Default to full filename if no extension
            
            mapName = filePath.substr(lastSlash + 1);

        } else {

            mapName = filePath.substr(lastSlash + 1, lastDot - lastSlash - 1); //Extract name only

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
        return MAP_OK;
    
    }

    pair<int, Map*> MapLoader::loadMap() {    

        //Create return map pointer, to be returned IF loading is successful
        Map* tempMapPtr = new Map();

        //Set map name
        tempMapPtr -> setMapName(this -> getMapName());

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

                for(size_t i = 0; i < territories.size(); i++) { //For ALL territories

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

        tempMapPtr -> buildContinentHashmap(); //Set up lookup table

        return {MAP_OK, tempMapPtr};

    }

}