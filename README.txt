ENSURE THAT YOU ARE IN THE COMP_345_PROJECT LEVEL

MAP:

COMPILE MAP WITH: g++ Map/Map.cpp Map/MapDriver.cpp -o driver
RUN MAP WITH VALGRIND WITH: valgrind --leak-check=full -s --track-origins=yes ./driver
RUN MAP NORMALLY: ./driver