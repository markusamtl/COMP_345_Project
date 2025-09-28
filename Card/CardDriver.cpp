#include "CardDriver.h"


void testCards() {

    cout << "=== Testing Cards, Deck, and Hand ===" << endl;

    // --- Create Deck ---
    Deck deck;
    cout << "Initial " << deck << endl;

    // --- Create Player with empty hand and orders---
    Hand* hand = new Hand();
    OrderList* orders = new OrderList();
    Player* alice = new Player("Alice", hand, orders);

    // --- Draw cards into Alice's hand ---
    cout << "\nDrawing 5 cards into Alice's hand..." << endl;
    for(int i = 0; i < 5; i++) {

        Card* drawn = deck.draw();

        if(drawn){

            hand -> addCardToHand(drawn);
            cout << "Drew: " << *drawn << endl;

        }

    }

    cout << "\nAlice's " << *hand << endl;
    cout << deck << endl;

    // --- Play all cards from Alice's hand ---
    cout << "\nPlaying all cards in Alice's hand..." << endl;
    
    
    vector<Card*> cardsToPlay = hand -> getHandCards(); // Need a copy of the hand vector, since it is mutated while iterating
    while (!hand->getHandCards().empty()) {
    
        Card* c = hand -> getHandCards().front(); //Get first card from hand.

        cout << "Playing " << *c << endl;
        hand -> playCard(c, alice, &deck);

    }

    cout << "\nAfter playing cards:" << endl;
    cout << "Alice's " << *hand << endl;
    cout << deck << endl;
    cout << "Alice's Orders: " << *orders << endl;

    // --- Cleanup ---
    delete alice;
}

int main() {

    testCards();
    return 0;

}
