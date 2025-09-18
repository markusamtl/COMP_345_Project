#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace WarzoneMap{
    class Territory;
    class Continent;

    class Territory
    {
        private:
            std::string name;
            std::vector<Territory*> neighbors;
            Continent* continent;

            std::string owner;
            int numArmies;

        public:
            Territory(const std::string &name);
            
            const std::string & getName() const;

            void addNeighbor(Territory* neighbor);
            const std::vector<Territory*> & getNeighbors() const;

            Continent* getContinent() const;
            void setContinent(Continent* c);

            // Ownership & armies (these may be set later in your game logic)
            const std::string & getOwner() const;
            void setOwner(const std::string &owner);
            int getNumArmies() const;
            void setNumArmies(int n);
        };

    class Continent
    {
        private:
            std::string name;
            int controlValue;
            std::vector<Territory*> territories;
        public:
            Continent(const std::string &name, int controlValue);

            const std::string & getName() const;
            int getControlValue() const;

            void addTerritory(Territory* t);
            const std::vector<Territory*> & getTerritories() const;
    };


    class Map
    {
        private:
            std::map<std::string, std::unique_ptr<Territory>> territories;     // name → Territory
            std::map<std::string, std::unique_ptr<Continent>> continents;       // name → Continent
        public:
            Map();

            // Access by territory name
            Territory* getTerritory(const std::string &territoryName) const;
            Continent* getContinent(const std::string &continentName) const;

            void addContinent(const std::string &continentName, std::unique_ptr<Continent> continentPtr);
            void addTerritory(const std::string &territoryName, std::unique_ptr<Territory> territoryPtr);

            const std::map<std::string, std::unique_ptr<Territory>> & getTerritories() const;
            const std::map<std::string, std::unique_ptr<Continent>> & getContinents() const;

            // Add adjacency
            void addNeighborRelation(const std::string &name1, const std::string &name2);

            // Validate the map according to the required properties.
            // Returns true if valid; if not, sets errorMessage.
            bool validate(std::string &errorMessage) const;
    };
}

#endif