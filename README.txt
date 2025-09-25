ENSURE THAT YOU ARE IN THE COMP_345_PROJECT LEVEL

MAP:

COMPILE MAP WITH: g++ Map/Map.cpp Map/MapDriver.cpp -o map_driver
RUN MAP WITH VALGRIND WITH: valgrind --leak-check=full -s --track-origins=yes ./map_driver
RUN MAP NORMALLY: ./map_driver
