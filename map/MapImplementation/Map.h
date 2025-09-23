#pragma once

#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include "../MapStorage/Territory.h"
#include "../MapStorage/Continent.h"

using namespace std;


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
 */
class Map{

    private:

        //-- Metadata --//
        string author;
        string image;
        string wrap;
        string scrollType;
        string warn;

        //-- Map Storage Properties --//
        vector<Continent*> continents;
        vector<Territory*> territories;


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
         * @param territories
         * @param continents
         */
        Map(string author, string image, string wrap, string scrollType, string warn, vector<Territory*> territories, vector<Continent*> continents);

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

};