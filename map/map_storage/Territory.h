#pragma once

#include <string>
#include <map>
#include <vector>
#include "Continent.h"

using namespace std;

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

        string owner;
        int numArmies;

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
        * @param owner Name of the player who owns the territory
        * @param numArmies Number of armies stationed in the territory
        */
        Territory(const string& ID, int xCoord, int yCoord, const vector<Territory*>& neighbors, Continent* continent, const string &owner, int numArmies);

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
        * @return Name of the player who owns the territory
        */
        const string& getOwner() const;

        /**
        * @brief Mutator for owner
        * @param owner Name of the player to set as owner
        */
        void setOwner(const string &owner);

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

        //-- Class Methods --//

        /**
        * @brief Adds a neighbor to the territory's neighbor list
        * @param neighbor Pointer to the neighboring territory to add
        */
        void addNeighbor(Territory* neighbor);
        
};
