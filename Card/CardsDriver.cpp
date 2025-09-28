#include "CardsDriver.h"
#include "Card.h"
#include <iostream>

using namespace WarzoneCard;

void testCards() {
    std::cout << "--- testCards ---" << std::endl;

    // Create a deck with one card of each type
    Deck deck;
    deck.addCard(new Card(CardType::Bomb));
    deck.addCard(new Card(CardType::Reinforcement));
    deck.addCard(new Card(CardType::Blockade));
    deck.addCard(new Card(CardType::Airlift));
    deck.addCard(new Card(CardType::Diplomacy));

    std::cout << deck << std::endl;

    // Draw a few cards into hand
    Hand hand;
    for (int i = 0; i < 3; ++i) {
        Card* c = deck.draw();
        if (c) {
            hand.addCard(c);
            std::cout << "Drew " << *c << " into hand." << std::endl;
        }
    }

    std::cout << hand << std::endl;

    // Play all cards in hand
    for (Card* c : hand.getCards()) {
        if (c) {
            c->play();
        }
    }

    std::cout << "Deck after drawing: " << deck << std::endl;
}
