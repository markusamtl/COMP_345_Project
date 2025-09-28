#pragma once

#include "Card.h"
#include "../Player/Player.h"
#include "../Order/Order.h"

#include <iostream>
using namespace std;
using namespace WarzoneCard;
using namespace WarzonePlayer;
using namespace WarzoneOrder;

/**
 * @brief Free function to test Deck, Hand, and Card functionality.
 * 
 * Creates a deck, draws cards into a hand, plays them, and
 * returns them back to the deck, while also generating Orders
 * for the Player.
 */
void testCards();
