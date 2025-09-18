#pragma once

#include <string>
#include <map>
#include <vector>

#include "../MapImplementation/Map.h"

using namespace std;

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
        * @param continents
        * @param territories
        */
        MapLoader(string author, string image, string wrap, string scrollType, string warn, map<string, int> continents, vector<vector<string>> territories);

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
        void setAuthor(string& author);
        

        /**
        * @brief Accessor and Mutator for image
        * @return image string
        */
        const string& getImage() const;

        /**
        * @brief Mutator for image
        * @param image string to set 
        */
        void setImage(string& image);
        
        /**
        * @brief Accessor for wrap
        * @return wrap string 
        */
        const string& getWrap() const;

        /**
        * @brief Mutator for wrap
        * @param wrap string to set 
        */
        void setWrap(string& wrap);

        /**
        * @brief Accessor for scrollType
        * @return scrollType string 
        */
        const string& getScrollType() const;

        /**
        * @brief Mutator for scrollType
        * @param scrollType string to set 
        */
        void setScrollType(string& scrollType);

        /**
        * @brief Accessor for warn
        * @return warn string 
        */
        const string& getWarn() const;

        /**
        * @brief Mutator for warn 
        * @param warn string to set
        */
        void setWarn(string& warn);

        /**
        * @brief Accessor and Mutator for continents
        * @return map of continent names to their values 
        */
        const map<string, int>& getContinents() const;

        /**
        * @brief Mutator for continents
        * @param continents map to set 
        */
        void setContinents(map<string, int> continents);

        /**
        * @brief Accessor for territories
        * @return vector of territories, each represented as a vector of strings 
        */
        const vector<vector<string>>& getTerritories() const;

        /**
        * @brief Mutator for territories
        * @param territories vector of territories to set
        */
        void setTerritories(vector<vector<string>> territories);
        
        //-- Class Methods --//

        /**
        * @brief Helper function to trim whitespace from both ends of a string
        * @param s input string
        * @return trimmed string 
        */
        string trim(const string& s);

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
        int MapLoader::loadMap(Map* inputMapPtr);

};