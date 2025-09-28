ENSURE THAT YOU ARE IN THE COMP_345_PROJECT LEVEL

MAP:

COMPILE MAP WITH: g++ -std=c++17 -Wall -I. Map/Map.cpp Map/MapDriver.cpp -o map_driver
RUN MAP WITH VALGRIND WITH: valgrind --leak-check=full -s --track-origins=yes ./map_driver
RUN MAP NORMALLY: ./map_driver

PLAYER:

COMPILE PLAYER WITH: g++ -std=c++17 -Wall -I.
RUN PLAYER WITH VALGRIND WITH: valgrind --leak-check=full -s --track-origins=yes ./player_driver
RUN PLAYER NORMALLY: ./player_driver

ORDER:

COMPILE ORDER WITH: g++ -std=c++17 -Wall -I. Order/Order.cpp Player/Player.cpp Card/Card.cpp Map/Map.cpp Order/OrderDriver.cpp -o order_driver
RUN ORDER WITH VALGRIND WITH: valgrind --leak-check=full -s --track-origins=yes ./order_driver
RUN ORDER NORMALLY: ./order_driver

CARD:

COMPILE ORDER WITH: g++ -std=c++17 -Wall -I.
RUN ORDER WITH VALGRIND WITH: valgrind --leak-check=full -s --track-origins=yes ./card_driver
RUN ORDER NORMALLY: ./card_driver