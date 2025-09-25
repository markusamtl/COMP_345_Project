#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "../../Map/Map.h"

using namespace std;

/**
 * @brief Manages a Player's owned territories using both a vector (for iteration), and an unordered_map (for fast lookups).
 */
class PlayerTerrContainer {

    private:

        vector<WarzoneMap::Territory*> territories;                  // for iteration
        unordered_map<string, WarzoneMap::Territory*> territoryIndex; // for O(1) lookup by ID

    public:

        //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

        PlayerTerrContainer();
        PlayerTerrContainer(vector<WarzoneMap::Territory*> territories, unordered_map<string, WarzoneMap::Territory*> territoryIndex);
        ~PlayerTerrContainer();

        PlayerTerrContainer(const PlayerTerrContainer& other);
        PlayerTerrContainer& operator=(const PlayerTerrContainer& other);

        friend ostream& operator<<(ostream& os, const PlayerTerrContainer& pc);

        //-- Accesors and Mutators --//

        /**
         * @brief Accessor for territories vector
         * @return const reference to vector of Territory* 
         */
        const vector<WarzoneMap::Territory*>& getTerritories() const;

        /**
         * @brief Mutator for territories vector
         * @param newTerritories vector of Territory* to set
         */
        void setTerritories(const vector<WarzoneMap::Territory*>& newTerritories);

        /**
         * @brief Accessor for territoryIndex map
         * @return const reference to unordered_map of Territory ID -> Territory*
         */
        const unordered_map<string, WarzoneMap::Territory*>& getTerritoryIndex() const;

        /**
         * @brief Mutator for territoryIndex map
         * @param newIndex unordered_map of Territory ID -> Territory* to set
         */
        void setTerritoryIndex(const unordered_map<string, WarzoneMap::Territory*>& newIndex);


        //-- Class Methods --//

        /**
         * @brief Adds a territory if not already owned.
         */
        void addTerritory(WarzoneMap::Territory* t);

        /**
         * @brief Removes a territory if owned.
         */
        void removeTerritory(WarzoneMap::Territory* t);

        /**
         * @brief Check if a territory is owned by this player.
         * @return true if owned, false otherwise.
         */
        bool owns(WarzoneMap::Territory* t) const;

        /**
         * @brief Clears all owned territories.
         */
        void clear();

        /**
         * @brief Number of territories owned.
         */
        size_t size() const;
        
};