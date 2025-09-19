#include <iostream>
#include <string>
#include "StringHandling.h"

using namespace std;


string StringHandling::trim(const string& inputStr) {

    size_t start = inputStr.find_first_not_of(" \t\r\n"); // Find first non-space character
    size_t end   = inputStr.find_last_not_of(" \t\r\n"); // Find last non-space character
    return (start == string::npos) ? "" : inputStr.substr(start, end - start + 1); // Return trimmed string, or no string if all spaces

}


pair<bool, int> StringHandling::isStrInt(const string& inputStr) {

    int retVal = 0;

    try {

        retVal = stoi(inputStr);  // attempt to convert string to int

    } catch (const invalid_argument& e) {

        cerr << "Parse error: string is not a valid integer: " << inputStr << endl;
        return {false, -1};

    } catch (const out_of_range& e) {

        cerr << "Parse error: integer value out of range: " << inputStr << endl;
        return {false, -1};


    }

    return {true, retVal};

}