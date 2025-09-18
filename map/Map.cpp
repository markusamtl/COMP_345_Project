#include "Map.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <queue>

namespace WarzoneMap{

    // ------------------ Territory ------------------

    Territory::Territory(const std::string &name_) :
        name(name_), 
        continent(nullptr), 
        owner(""), 
        numArmies(0) {}

    const std::string & Territory::getName() const { return name; }

    void Territory::addNeighbor(Territory* neighbor) {
        if (!neighbor) return;

        // Avoid duplicates
        for (Territory* n : neighbors) {
            if (n == neighbor) return;
        }
        neighbors.push_back(neighbor);
    }

    const std::vector<Territory*> & Territory::getNeighbors() const {
        return neighbors;
    }

    Continent* Territory::getContinent() const {
        return continent;
    }
    void Territory::setContinent(Continent* c) {
        continent = c;
    }

    const std::string & Territory::getOwner() const { return owner; }
    void Territory::setOwner(const std::string &o) { owner = o; }

    int Territory::getNumArmies() const { return numArmies; }
    void Territory::setNumArmies(int n) { numArmies = n; }

    // ------------------ Continent ------------------

    Continent::Continent(const std::string &name_, int controlValue_) :
        name(name_), controlValue(controlValue_) {}

    const std::string & Continent::getName() const { return name; }
    int Continent::getControlValue() const { return controlValue; }

    void Continent::addTerritory(Territory* t) {
        if (!t) return;
        territories.push_back(t);
    }

    const std::vector<Territory*> & Continent::getTerritories() const {
        return territories;
    }

    // ------------------ Map ------------------

    Map::Map() {}

    Territory* Map::getTerritory(const std::string &territoryName) const {
        auto it = territories.find(territoryName);
        if (it == territories.end()) return nullptr;
        return it->second.get();
    }

    Continent* Map::getContinent(const std::string &continentName) const {
        auto it = continents.find(continentName);
        if (it == continents.end()) return nullptr;
        return it->second.get();
    }

    void Map::addContinent(const std::string &continentName, std::unique_ptr<Continent> continentPtr){
        continents[continentName] = std::move(continentPtr);
    }

    void Map::addTerritory(const std::string &territoryName, std::unique_ptr<Territory> territoryPtr){
        territories[territoryName] = std::move(territoryPtr);
    }

    const std::map<std::string, std::unique_ptr<Territory>> & Map::getTerritories() const {
        return territories;
    }

    const std::map<std::string, std::unique_ptr<Continent>> & Map::getContinents() const {
        return continents;
    }

    void Map::addNeighborRelation(const std::string &name1, const std::string &name2) {
        Territory* t1 = getTerritory(name1);
        Territory* t2 = getTerritory(name2);
        if (!t1 || !t2) return;
        t1->addNeighbor(t2);
        t2->addNeighbor(t1);
    }

    bool Map::validate(std::string &errorMessage) const {
        // 1) Check non‐empty
        if (territories.empty()) {
            errorMessage = "Map has no territories.";
            return false;
        }
        if (continents.empty()) {
            errorMessage = "Map has no continents.";
            return false;
        }

        for (auto const & tp : territories) {
            Territory* t = tp.second.get();
            if (t->getContinent() == nullptr) {
                errorMessage = "Territory '" + t->getName() + "' does not belong to any continent.";
                return false;
            }
        }

        std::set<std::string> visited;
        std::queue<Territory*> q;
        auto it0 = territories.begin();
        q.push(it0->second.get());
        visited.insert(it0->first);
        while (!q.empty()) {
            Territory* cur = q.front(); q.pop();
            for (Territory* nb : cur->getNeighbors()) {
                if (!nb) continue;
                const std::string & nname = nb->getName();
                if (visited.count(nname) == 0) {
                    visited.insert(nname);
                    q.push(nb);
                }
            }
        }
        if (visited.size() != territories.size()) {
            errorMessage = "Map is not fully connected (some territories unreachable).";
            return false;
        }

        for (auto const & cp : continents) {
            Continent* cont = cp.second.get();
            auto const & terrList = cont->getTerritories();
            if (terrList.empty()) {
                errorMessage = "Continent '" + cont->getName() + "' has no territories.";
                return false;
            }
            std::set<std::string> contVisited;
            std::queue<Territory*> cq;
            Territory* start = terrList.front();
            cq.push(start);
            contVisited.insert(start->getName());
            while (!cq.empty()) {
                Territory* cur = cq.front(); cq.pop();
                for (Territory* nb : cur->getNeighbors()) {
                    if (!nb) continue;
                    if (nb->getContinent() == cont) {
                        const std::string & nbName = nb->getName();
                        if (contVisited.count(nbName) == 0) {
                            contVisited.insert(nbName);
                            cq.push(nb);
                        }
                    }
                }
            }
            if (contVisited.size() != terrList.size()) {
                errorMessage = "Continent '" + cont->getName() + "' is not a connected subgraph.";
                return false;
            }
        }

        errorMessage.clear();
        return true;
    }