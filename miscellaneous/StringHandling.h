#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

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

