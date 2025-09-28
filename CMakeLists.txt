cmake_minimum_required(VERSION 3.10)
project(COMP_345_Project LANGUAGES CXX)

# Set C++ standard and warnings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# Include root directory so #include "Map/Map.h" etc. works
include_directories(${CMAKE_SOURCE_DIR})

# ================== MAP DRIVER ==================
add_executable(map_driver
    Map/Map.cpp 
    Player/Player.cpp 
    Order/Order.cpp 
    Card/Card.cpp 
    Map/MapDriver.cpp
)

# ================== PLAYER DRIVER ==================
add_executable(player_driver
    Player/Player.cpp
    Order/Order.cpp 
    Map/Map.cpp 
    Card/Card.cpp 
    Player/PlayerDriver.cpp
)

# ================== ORDER DRIVER ==================
add_executable(order_driver
    Order/Order.cpp 
    Player/Player.cpp 
    Card/Card.cpp 
    Map/Map.cpp 
    Order/OrderDriver.cpp
)

# ================== CARD DRIVER ==================
add_executable(card_driver
    Card/Card.cpp 
    Order/Order.cpp 
    Player/Player.cpp 
    Map/Map.cpp 
    Card/CardDriver.cpp
)

# ================== CUSTOM TARGETS FOR VALGRIND ==================
find_program(VALGRIND_EXECUTABLE valgrind)

if (VALGRIND_EXECUTABLE)
    add_custom_target(run_map_valgrind
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full -s --track-origins=yes ./map_driver
        DEPENDS map_driver
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(run_player_valgrind
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full -s --track-origins=yes ./player_driver
        DEPENDS player_driver
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(run_order_valgrind
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full -s --track-origins=yes ./order_driver
        DEPENDS order_driver
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(run_card_valgrind
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full -s --track-origins=yes ./card_driver
        DEPENDS card_driver
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
