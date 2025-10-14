#pragma once
#include <climits>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <sstream>
#include <string>

namespace WarzonePlayer { class Player; } //Forward declaration
namespace WarzoneOrder { class TimeUtil; } //Forward declaration
using namespace std;


namespace WarzoneMap {

    // Map loader and validation status codes
    const int MAP_OK = 0;
    const int MAP_FILE_NOT_FOUND = 1;
    const int MAP_INVALID_SECTION = 2;
    const int MAP_PARSE_ERROR = 3;

    const int INVALID_MAP_PTR = 1;
    const int INVALID_AUTHOR = 2;
    const int INVALID_IMAGE = 3;
    const int INVALID_WRAP = 4;
    const int INVALID_SCROLL = 5;
    const int INVALID_WARN = 6;
    const int INVALID_CONTINENT = 7;
    const int INVALID_TERRITORY = 8;
    const int INVALID_MAP_STRUCTURE = 9;

    using WarzonePlayer::Player;
    using WarzoneOrder::TimeUtil;

    // ================= StringHandling =================
    /**
    * @brief A few methods to help reduce the amount of lines used in the WarzoneMap 
    */
    class StringHandling{

        public:

            /**
            * @brief Helper function to trim whitespace from both ends of a string
            * @param inputStr input string
            * @return trimmed string 
            */
            static string trim(const string& inputStr);

            /**
            * @brief Helper function to see if a string is an integer?
            * @param inputStr input string
            * @return Is input string an integer? 
            */
            static pair<bool, int> isStrInt(const string& inputStr);

    };

    // Forward declarations
    class Continent;
    class Territory;

    // ================= Continent =================
    /**
     * @brief The Continent class represents a continent in the map.
     * It contains properties such as ID, bonus value, and the territories that belong to it.
     */

    class Continent {

        private:
            
            //-- Continent Properties --//
            string ID;
            int bonusValue;
            vector<Territory*> territories;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
            * @brief Default constructor
            */
            Continent();

            /**
            * @brief Parameterized constructor
            * @param ID
            * @param bonusValue
            */
            Continent(const string& ID, int bonusValue);

            /**
            * @brief Parameterized constructor
            * @param ID
            * @param bonusValue
            * @param territories
            */
            Continent(const string& ID, int bonusValue, const vector<Territory*>& territories);

            /**
            * @brief Destructor
            */
            ~Continent();

            /**
            * @brief Copy constructor
            * @param other
            */
            Continent(const Continent& other);

            /**
            * @brief Assignment operator
            * @param other
            * @return reference to this Continent
            */
            Continent& operator=(const Continent& other);

            /**
            * @brief Stream insertion operator
            * @param os output stream
            * @param continent continent to output
            * @return reference to output stream
            */
            friend ostream& operator<<(ostream& os, const Continent& continent);

            //-- Accessors and Mutators --//

            /**
            * @brief Accessor for ID
            * @return ID string 
            */
            const string& getID() const;

            /**
            * @brief Mutator for ID
            * @param ID string to set 
            */
            void setID(const string& ID);

            /**
            * @brief Accessor for bonusValue
            * @return bonus value of the continent 
            */
            int getBonusValue() const;

            /**
            * @brief Mutator for bonusValue
            * @param bonusValue value to set 
            */
            void setBonusValue(int bonusValue);

            /**
            * @brief Accessor for territories
            * @return vector of pointers to territories in the continent 
            */
            const vector<Territory*>& getTerritories() const;

            /**
            * @brief Mutator for territories
            * @param territories vector of pointers to set as territories in the continent
            */
            void setTerritories(const vector<Territory*>& territories);

            //-- Class Methods --//

            /**
             * @brief Adds a territory to the continent
             * @param territory pointer to the territory to add to territories list
             */
            void addTerritory(Territory* territory);

    };

    // ================= Territory =================
    /**
     * @brief The Territory class represents a territory in the map.
     * It contains properties such as ID, neighboring territories, the continent it belongs to,
     * ownership information, and the number of armies stationed there.
     */
    class Territory {
        
        private:
                
            //-- Territory Properties --//    

            string ID;
            int xCoord;
            int yCoord;
            vector<Territory*> neighbors;
            Continent* continent;

            //-- Ownership & armies --// 
            Player* owner;
            int numArmies;
            long long numericTerrID;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator  --//

            /**
            * @brief Default constructor
            */
            Territory();

            /**
            * @brief Parameterized constructor
            * @param ID Unique identifier of the territory
            */
            Territory(const string &ID);

            /**
            * @brief Parameterized constructor with coordinates and continent
            * @param ID Unique identifier of the territory
            * @param xCoord X coordinate
            * @param yCoord Y coordinate
            * @param continent Pointer to the continent this territory belongs to
            */
            Territory(const string &ID, int xCoord, int yCoord, Continent* continent);

            /**
            * @brief Parameterized constructor with all properties
            * @param ID Unique identifier of the territory
            * @param xCoord X coordinate
            * @param yCoord Y coordinate
            * @param neighbors Vector of pointers to neighboring territories
            * @param continent Pointer to the continent this territory belongs to
            * @param owner Pointer to the player who owns the territory
            * @param numArmies Number of armies stationed in the territory
            */
            Territory(const string& ID, int xCoord, int yCoord, const vector<Territory*>& neighbors,Continent* continent, WarzonePlayer::Player* owner, int numArmies);
            /**
            * @brief Destructor
            */
            ~Territory();

            /**
            * @brief Copy constructor
            * @param other Territory object to copy from
            */
            Territory(const Territory& other);

            /**
            * @brief Assignment operator
            * @param other Territory object to assign from
            * @return Reference to this Territory
            */
            Territory& operator=(const Territory& other);

            /**
            * @brief Stream insertion operator
            * @param os Output stream
            * @param territory Territory to output
            * @return Reference to output stream
            */
            friend ostream& operator<<(ostream& os, const Territory& territory);

            //-- Accessors and Mutators --//
            
            /**
            * @brief Accessor for ID
            * @return Unique identifier of the territory
            */
            const string& getID() const;

            /**
            * @brief Mutator for ID
            * @param ID Unique identifier to set
            */
            void setID(const string &ID);

            /**
            * @brief Accessor for X coordinate
            * @return X coordinate of the territory
            */
            int getXCoord() const;

            /**
            * @brief Mutator for X coordinate
            * @param xCoord X coordinate to set
            */
            void setXCoord(int xCoord);

            /**
            * @brief Accessor for Y coordinate
            * @return Y coordinate of the territory
            */
            int getYCoord() const;

            /**
            * @brief Mutator for Y coordinate
            * @param yCoord Y coordinate to set
            */
            void setYCoord(int yCoord);

            /**
            * @brief Accessor for neighbors
            * @return Vector of pointers to neighboring territories
            */
            const vector<Territory*>& getNeighbors() const;

            /**
            * @brief Mutator for neighbors
            * @param neighbors Vector of pointers to set as neighbors
            */
            void setNeighbors(const vector<Territory*>& neighbors);

            /**
            * @brief Accessor for continent
            * @return Pointer to the continent this territory belongs to
            */
            Continent* getContinent() const;

            /**
            * @brief Mutator for continent
            * @param continent Pointer to set as this territory's continent
            */
            void setContinent(Continent* continent);

            /**
            * @brief Accessor for owner
            * @return Player pointer for who owns the territory
            */
            Player* getOwner() const;

            /**
            * @brief Mutator for owner
            * @param owner Player pointer for who to set as owner
            */
            void setOwner(Player* owner);

            /**
            * @brief Accessor for number of armies
            * @return Number of armies stationed in the territory
            */
            int getNumArmies() const;

            /**
            * @brief Mutator for number of armies
            * @param numArmies Number of armies to set
            */
            void setNumArmies(int numArmies);

            /**
            * @brief Accessor for integer value of Territory ID
            * @return integer value of Territory ID
            */
            long long getNumericTerrID();

            //Note: NO mutator should exist. This is ONLY based on the string ID.

            //-- Class Methods --//

            /**
            * @brief Adds a neighbor to the territory's neighbor list
            * @param neighbor Pointer to the neighboring territory to add
            */
            void addNeighbor(Territory* neighbor);

            /**
             * @brief Comperator operation between two territories, helps to sort alphabetically
             * @param a First territory
             * @param b Second territory
             */
            static bool territoryIDCompare(Territory* a, Territory* b);

            /**
             * @brief Comperator operation between two territories, helps to sort based on number of armies in descending order
             * @param a First territory
             * @param b Second territory
             */
            static bool territoryNumArmiesCompareDescend(Territory* a, Territory* b);

            /**
             * @brief Comperator operation between two territories, helps to sort based on number of armies in ascending order
             * @param a First territory
             * @param b Second territory
             */
            static bool territoryNumArmiesCompareAscend(Territory* a, Territory* b);

            /**
             * @brief Comparator operation between two enemy territories based on their owner's overall strength, then by defending armies.
             * @param a First territory
             * @param b Second territory
             */
            static bool territoryAttackPriorityCompare(Territory* a, Territory* b);

             /**
             * @brief Comparator operation between two territories bordering enemies to see who's most at threat, descending by risk level
             * @param a First territory
             * @param b Second territory
             */
            static bool territoryThreatCompareDescend(Territory* a, Territory* b);


            /**
             * @brief Computes a deterministic 64-bit numeric identifier for this Territory using the FNV-1a hash algorithm.
             *
             * This method converts the alphanumeric `ID` string of a Territory into a unique, uniform numeric value
             * (`numericTerrID`) through the Fowler–Noll–Vo (FNV-1a) hashing process. The resulting value is independent of
             * the string’s length or composition, ensuring consistent magnitude and distribution across all territory names.
             *
             * ### Algorithm Summary
             * - Hash variant: **FNV-1a (64-bit)**
             * - Offset basis: `1469598103934665603`
             * - Prime multiplier: `1099511628211`
             * - Final value is reduced modulo a large 64-bit prime (`22801763489`) for bounded magnitude.
             *
             * ### Benefits
             * - Deterministic, Uniform, nearly 100% guarenteed to be Collision-proof, lightweight
             *
             * @note The resulting hash enables consistent continent and player hash-sum comparisons,
             *       making it suitable for both map validation and AI threat assessment.
             *
             * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
             */

            void computeNumericTerrID();
            
        };

    // ================= Map =================
    /**
     * @class Map
     * @brief Represents a complete game map consisting of Territories and Continents.
     *
     * Ownership Model:
     * - Map owns all dynamically allocated Continent* and Territory* objects.
     * - Map is responsible for cleaning them up in its destructor.
     * - Continent and Territory do NOT own each other (non-owning raw pointers only).
     * - Continent and Territory can NOT exist by themselves.
     * 
     * Game Logic:
     * - Maintains a lookup table of continent sizes (number of territories).
     *   This supports O(1) checking of continent control by players.
     */
    class Map{

        private:

            //-- Metadata --//
            string author;
            string image;
            string wrap;
            string scrollType;
            string warn;
            string mapName;

            //-- Map Storage Properties --//
            vector<Continent*> continents;
            vector<Territory*> territories;

            //-- Game Logic Info --/
            unordered_map<Continent*, long long> continentLookupTable;


        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator  --//

            /**
             * @brief Default constructor
             */
            Map();

            /**
             * @brief Parameterized constructor
             * @param author
             * @param image
             * @param wrap
             * @param scrollType
             * @param warn
             * @param mapName
             * @param territories
             * @param continents
             * @param continentLookupTable;
             */
            Map(string author, string image, string wrap, string scrollType, string warn, string mapName,
                vector<Territory*> territories, vector<Continent*> continents, unordered_map<Continent*, long long> continentLookupTable);

            /**
             * @brief Destructor
             */
            ~Map();

            /**
             * @brief Copy constructor
             * @param other
             */
            Map(const Map& other);

            /**
             * @brief Assignment operator
             * @param other
             * @return reference to this Map
             */
            Map& operator=(const Map& other);

            /**
             * @brief Stream insertion operator
             * @param os output stream
             * @param map map to output
             * @return reference to output stream
             */
            friend ostream& operator<<(ostream& os, const Map& map);

            //-- Accessors and Mutators --//

            /**
            * @brief Accessor for author
            * @return author string
            */
            const string& getAuthor() const;

            /**
            * @brief Mutator for author
            * @param author string to set
            */
            void setAuthor(const string& author);
            

            /**
            * @brief Accessor and Mutator for image
            * @return image string
            */
            const string& getImage() const;

            /**
            * @brief Mutator for image
            * @param image string to set 
            */
            void setImage(const string& image);
            
            /**
            * @brief Accessor for wrap
            * @return wrap string 
            */
            const string& getWrap() const;

            /**
            * @brief Mutator for wrap
            * @param wrap string to set 
            */
            void setWrap(const string& wrap);

            /**
            * @brief Accessor for scrollType
            * @return scrollType string 
            */
            const string& getScrollType() const;

            /**
            * @brief Mutator for scrollType
            * @param scrollType string to set 
            */
            void setScrollType(const string& scrollType);

            /**
            * @brief Accessor for warn
            * @return warn string 
            */
            const string& getWarn() const;

            /**
            * @brief Mutator for warn 
            * @param warn string to set
            */
            void setWarn(const string& warn);

            /**
             * @brief Accessor for mapName
             * @return mapName string
             */
            const string& getMapName() const;

            /**
             * @brief Mutator for mapName
             * @param mapName String to set
             */
            void setMapName(const string mapName);

            /**
             * @brief Accessor for continents
             * @return vector of pointers to continents in the map 
             */
            const vector<Continent*>& getContinents() const;

            /**
             * @brief Mutator for continents
             * @param continents vector of pointers to set as continents in the map
             */
            void setContinents(const vector<Continent*>& continents);

            /**
             * @brief Accessor for territories
             * @return vector of pointers to territories in the map 
             */
            const vector<Territory*>& getTerritories() const;

            /**
             * @brief Mutator for territories
             * @param territories vector of pointers to set as territories in the map
             */
            void setTerritories(const vector<Territory*>& territories);

            /**
             * @brief Accessor for territories
             * @return Lookup table
             */
            const unordered_map<Continent*, long long>& getContinentLookupTable() const;

            /**
             * @brief Mutator for continentLookupTable
             * @param continentLookupTable Replacement hashmap
             */
            void setContinentLookupTable(const unordered_map<Continent*, long long> continentLookupTable);

            //-- Class Methods --//

            /**
            * @brief Adds a continent to the map
            * @param continent pointer to the continent to add to continents list
            */
            void addContinent(Continent* continent);

            /**
            * @brief Adds a territory to the map
            * @param territory pointer to the territory to add to territories list
            */
            void addTerritory(Territory* territory);

            /**
             * @brief Retrieves a territory by its ID
             * @param ID string ID of the territory to find
             * @return pointer to the Territory if found, nullptr otherwise
             */
            Territory* getTerritoryByID(const string& ID);

            /**
             * @brief Retrieves a territory by its ID
             * @param ID numeric ID of the territory to find
             * @return pointer to the Territory if found, nullptr otherwise
             */
            Territory* getTerritoryByNumID(long long ID);

            /**
             * @brief Retrieves a continent by its ID
             * @param ID string ID of the continent to find
             * @return pointer to the Continent if found, nullptr otherwise
             */
            Continent* getContinentByID(const string& ID);

            /**
            * @brief Runs DFS on a set of territories and checks if they form a connected graph.
            * @param terrList List of territories to check
            * @param restrictCont Optional continent to restrict traversal (nullptr = no restriction)
            * @return true if all territories in terrList are connected, false otherwise
            */
            bool isMapConnectedDFS(const vector<Territory*>& terrList, Continent* restrictCont) const;

            /**
            * @brief Validate the integrity of the map.
            * Ensures:
            * 1. The map is non-empty (has at least one territory and one continent).
            * 2. Each territory belongs to exactly one continent.
            * 3. The entire map is a connected graph (any territory can reach any other territory). Done with Depth First Search.
            * 4. Each continent is a connected subgraph (any territory in the continent can reach any other territory in the same continent). Done with Depth First Search.
            * @return true if the map is valid, false otherwise.
            */
            bool validate() const;

            /**
             * @brief Build the continent lookup table.
             * Call this once after the map is fully loaded.
             */
            void buildContinentHashmap();

            /**
             * @brief Build an empty continent hashmap (all continents mapped to 0).
             * @return A hashmap with every continent in the map as a key, and 0 as the value.
             */
            unordered_map<Continent*, long long> buildEmptyContinentHashmap() const;

            /**
             * @brief Finds the shortest path between two territories using Dijkstra's algorithm.
             * 
             * This method computes the minimal path (fewest adjacency steps) between a start
             * and goal territory by traversing the map graph. All edges have equivalent work.
             * 
             * @param start Pointer to the starting territory.
             * @param goal Pointer to the target territory.
             * @return A vector of Territory* representing the shortest path from start to goal (inclusive).
             * Returns an empty vector if no path exists or either input is null.
             */
            vector<Territory*> shortestPathBetweenTerritories(Territory* start, Territory* goal);


    };

    // ================= MapLoader =================
    /**
     * @brief The MapLoader class is responsible for loading and parsing map files.
     * It reads map metadata, continents, and territories from a specified file.
     * The class provides a method to access the loaded data and validate the map structure.
     */
    class MapLoader {

        private:             

            //-- Metadata --//
            string author;
            string image;
            string wrap;
            string scrollType;
            string warn;
            string mapName;
            
            //-- Continent Info --//
            map<string, int> continents; 
            
            //-- Territory Info --//
            vector<vector<string>> territories;

        public:
            
            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator  --//

            /**
            * @brief Default constructor
            */
            MapLoader();

            /**
            * @brief Parameterized constructor
            * @param author
            * @param image
            * @param wrap
            * @param scrollType
            * @param warn
            * @param mapName
            * @param continents
            * @param territories
            */
            MapLoader(string author, string image, string wrap, string scrollType, string warn, string mapName,
                      map<string, int> continents, vector<vector<string>> territories);

            /**
            * @brief Destructor
            */
            ~MapLoader();

            /**
            * @brief Copy constructor
            * @param other
            */
            MapLoader(const MapLoader& other);
            
            /**
            * @brief Assignment operator
            * @param other
            * @return reference to this MapLoader
            */
            MapLoader& operator=(const MapLoader& other);
            
            /**
            * @brief Stream insertion operator
            * @param os
            * @param mapLoader
            * @return reference to output stream
            */
            friend ostream& operator<<(ostream& os, const MapLoader& mapLoader);

            //-- Accessors and Mutators --//

            /**
            * @brief Accessor for author
            * @return author string
            */
            const string& getAuthor() const;

            /**
            * @brief Mutator for author
            * @param author string to set
            */
            void setAuthor(const string& author);
            

            /**
            * @brief Accessor and Mutator for image
            * @return image string
            */
            const string& getImage() const;

            /**
            * @brief Mutator for image
            * @param image string to set 
            */
            void setImage(const string& image);
            
            /**
            * @brief Accessor for wrap
            * @return wrap string 
            */
            const string& getWrap() const;

            /**
            * @brief Mutator for wrap
            * @param wrap string to set 
            */
            void setWrap(const string& wrap);

            /**
            * @brief Accessor for scrollType
            * @return scrollType string 
            */
            const string& getScrollType() const;

            /**
            * @brief Mutator for scrollType
            * @param scrollType string to set 
            */
            void setScrollType(const string& scrollType);

            /**
            * @brief Accessor for warn
            * @return warn string 
            */
            const string& getWarn() const;

            /**
            * @brief Mutator for warn 
            * @param warn string to set
            */
            void setWarn(const string& warn);

            /**
            * @brief Accessor for mapName
            * @return mapName string 
            */
            const string& getMapName() const;

            /**
            * @brief Mutator for mapName 
            * @param mapName string to set
            */
            void setMapName(const string& mapName);

            /**
            * @brief Accessor and Mutator for continents
            * @return map of continent names to their values 
            */
            const map<string, int>& getContinents() const;

            /**
            * @brief Mutator for continents
            * @param continents map to set 
            */
            void setContinents(const map<string, int> continents);

            /**
            * @brief Accessor for territories
            * @return vector of territories, each represented as a vector of strings 
            */
            const vector<vector<string>>& getTerritories() const;

            /**
            * @brief Mutator for territories
            * @param territories vector of territories to set
            */
            void setTerritories(const vector<vector<string>> territories);
            
            //-- Class Methods --//

            /**
            * @brief Imports and parses map information from a file
            * @param filePath path to the map file
            * @return 0 on success, non-zero error code on failure
            * MAP_OK (0): Success
            * MAP_FILE_NOT_FOUND (1): File could not be opened
            * MAP_INVALID_SECTION (2): Invalid section encountered in file
            * MAP_PARSE_ERROR (3): Error parsing file contents
            */  
            int importMapInfo(const string& filePath);

            /**
             * @brief Loads the parsed map data into a Map object
             * @param inputMapPtr pointer to the Map object to load data into
             * @return 0 on success, non-zero error code on failure
             * INVALID_MAP_PTR (1): Input map pointer is null  
             * INVALID_AUTHOR (2): Author metadata is missing or invalid
             * INVALID_IMAGE (3): Image metadata is missing or invalid
             * INVALID_WRAP (4): Wrap metadata is missing or invalid
             * INVALID_SCROLL (5): ScrollType metadata is missing or invalid
             * INVALID_WARN (6): Warn metadata is missing or invalid
             * INVALID_CONTINENT (7): Continent data is missing or invalid
             * INVALID_TERRITORY (8): Territory data is missing or invalid
             */
            pair<int, Map*> loadMap();

    };

}