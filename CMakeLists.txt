cmake_minimum_required(VERSION 3.10)
project(COMP_345_Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common warnings/symbols for both builds
add_compile_options(-Wall -Wextra -g)

# Allow includes like #include "Map/Map.h"
include_directories(${CMAKE_SOURCE_DIR})

# ---- Shared sources (NO files with their own main()) ----
set(SOURCES_COMMON
    MainDriver.cpp

    # Module Implementations
    Map/Map.cpp
    Player/Player.cpp
    Order/Order.cpp
    Card/Card.cpp
    GameEngine/GameEngine.cpp
    CommandProcessing/CommandProcessing.cpp

    # Test functions (ensure these files do NOT define main())
    Map/MapDriver.cpp
    Player/PlayerDriver.cpp
    Order/OrderDriver.cpp
    Card/CardDriver.cpp
    GameEngine/GameEngineDriver.cpp
    CommandProcessing/CommandProcessingDriver.cpp

)

# ---- Normal run target: -O2 ----
add_executable(main_driver ${SOURCES_COMMON})
target_compile_options(main_driver PRIVATE -O2)

# ---- Valgrind target binary: -O3 ----
add_executable(main_driver_vg ${SOURCES_COMMON})
target_compile_options(main_driver_vg PRIVATE -O3)

# ---- Convenience run targets ----
add_custom_target(run
    COMMAND ./main_driver
    DEPENDS main_driver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running main_driver (built with -O2)..."
)

find_program(VALGRIND_EXECUTABLE valgrind)
if (VALGRIND_EXECUTABLE)
    add_custom_target(run_main_valgrind
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full -s --track-origins=yes ./main_driver_vg
        DEPENDS main_driver_vg
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running main_driver_vg (built with -O3) under Valgrind..."
    )
endif()
