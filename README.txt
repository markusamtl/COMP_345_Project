# COMP_345_Project

This project is developed and tested in an **Ubuntu environment**.  
It uses **CMake** to manage builds and supports running under **Valgrind** for memory debugging.  

---

## Requirements

Make sure the following are installed on your Ubuntu system:

- **g++** (C++17 compiler)
- **cmake** (>= 3.10)
- **make**
- **valgrind**

Install them (if needed) with:

sudo apt update
sudo apt install g++ cmake make valgrind

**BUILD INTRUCTIONS FOR CMKAE**

1) Navigate to root directory: cd COMP_345_PROJECT
2) Create and enter build directory: mkdir build && cd build
3) Run cmake, and build the drivers: cmake .. && make

This **SHOULD** be successful.

To run the drivers, run the following commands:

MAP:

Without Valgrind: ./map_driver
With Valgrind: make run_map_valgrind

PLAYER:

Without Valgrind: ./player_driver
With Valgrind: make run_player_valgrind

ORDER:

Without Valgrind: ./order_driver
With Valgrind: make run_order_valgrind  

CARD:

Without Valgrind: ./card_driver
With Valgrind: make run_card_valgrind

GAMEENGINE:
Without Valgrind: ./game_engine_driver