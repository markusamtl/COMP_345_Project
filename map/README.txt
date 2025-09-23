ENSURE THAT THE USER IS ABOVE THE MAP CLASS IN THE DIRECTORY.
COMPILE MAP WITH: g++ map/MapImplementation/*.cpp map/MapLoader/*.cpp map/MapStorage/*.cpp miscellaneous/*.cpp map/MapDriver.cpp -o driver
RUN MAP WITH VALGRIND WITH: valgrind --leak-check=full -s --track-origins=yes ./driver