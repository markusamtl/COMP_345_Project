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
        * @param name
        */
        Territory(const string &ID);

        /**
        * @brief Parameterized constructor
        * @param ID
        * @param xCoord
        * @param yCoord
        * @param continent 
        */
        Territory(const string &ID, int xCoord, int yCoord, const Continent& continent);


        /**
        * @brief Parameterized constructor with all properties
        * @param ID
        * @param xCoord
        * @param yCoord
        * @param neighbors
        * @param continent
        * @param owner
        * @param numArmies
        */
        Territory(const string& ID, int xCoord, int yCoord, const vector<Territory*>& neighbors, const Continent& continent, const string &owner, int numArmies);

        /**
        * @brief Destructor
        */
        ~Territory();

        /**
        * @brief Copy constructor
        * @param other
        */
        Territory(const Territory& other);

        /**
        * @brief Assignment operator
        * @param other
        */
        Territory& operator=(const Territory& other);

        /**
        * @brief Stream insertion operator
        * @param os output stream
        * @param territory territory to output
        * @return reference to output stream
        */
        friend ostream& operator<<(ostream& os, const Territory& territory);

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
        void setID(const string &ID);

        /** 
        * @brief Accessor for xCoord
        * @return x coordinate of the territory
        */
        int getXCoord() const;

        /**
        * @brief Mutator for xCoord
        * @param xCoord x coordinate to set 
        */
        void setXCoord(int xCoord);

        /**
        * @brief Accessor for yCoord
        * @return y coordinate of the territory 
        */
        int getYCoord() const;

        /**
        * @brief Mutator for yCoord
        * @param yCoord y coordinate to set 
        */
        void setYCoord(int yCoord);

        /**
        * @brief Accessor for neighbors
        * @return vector of pointers to neighboring territories 
        */
        const vector<Territory*>& getNeighbors() const;

        /**
        * @brief Mutator for neighbors
        * @param neighbors vector of pointers to set as neighbors
        */
        void setNeighbors(const vector<Territory*>& neighbors);
        
        /**
        * @brief Accesor for continent
        * @return Continent pointer of the territory
        */
        Continent* getContinent() const;

        /**
        * @brief Mutator for continent
        * @param continent pointer to set as continent
        */
        void setContinent(Continent* continent);

        /**
        * @brief Accessor for owner
        * @return owner string
        */
        const string& getOwner() const;

        /**
        * @brief Mutator for owner
        * @param owner string to set
        */
        void setOwner(const string &owner);

        /**
        * @brief Accessor for numArmies
        * @return number of armies on the territory
        */
        int getNumArmies() const;

        /**
        * @brief Mutator for numArmies
        * @param numArmies number of armies to set 
        */
        void setNumArmies(int numArmies);

        //-- Class Methods --//

        /**
        * @brief Adds a neighbor to the territory's neighbor list
        * @param neighbor pointer to the neighboring territory to add 
        */
        void addNeighbor(Territory* neighbor);
        
};