#pragma once

#include <string>
#include <map>
#include <vector>
#include "Territory.h"

using namespace std;

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