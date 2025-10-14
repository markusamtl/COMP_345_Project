#include "Card.h"
#include "../Order/Order.h"

#include <algorithm>

using namespace std;

namespace WarzoneCard {

    // ================= Card ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Card::Card(){ this -> type = CardType::Unknown; }

    Card::Card(CardType t){ this->type = t; }

    Card::~Card(){} // No dynamic memory to free
    

    Card::Card(const Card& other) { this->type = other.type; }

    Card& Card::operator=(const Card& other) {
        
        if (this != &other) {

            this->type = other.type;

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Card& card) {
        
        switch (card.type) {

            case (CardType::Bomb): os << "Card(Bomb)"; break;
            case (CardType::Blockade): os << "Card(Blockade)"; break;
            case CardType::Airlift: os << "Card(Airlift)"; break;
            case CardType::Diplomacy: os << "Card(Diplomacy)"; break;
            case (CardType::Unknown): os << "Card: Unknown"; break;
            default: os << "Error";

        }

        return os;

    }

    //-- Accessors and Mutators --//

    CardType Card::getType() const { return this -> type; }

    string Card::getTypeString() const {

        switch(this -> type) {

            case CardType::Bomb:        return "Bomb";
            case CardType::Blockade:    return "Blockade";
            case CardType::Airlift:     return "Airlift";
            case CardType::Diplomacy:   return "Diplomacy";
            case CardType::Unknown:     return "Unknown";
            default:                    return "Error";

        }

    }

    void Card::setType(CardType t) { this->type = t; }

    //-- Class Methods --//

    void Card::play(Player* player){

        Order* order = nullptr;

        switch (this -> type) {

            case(CardType::Bomb):

                order = new WarzoneOrder::Bomb(player, nullptr); 
                break;

            case(CardType::Blockade):

                order = new WarzoneOrder::Blockade(player, nullptr, nullptr);
                break;

            case(CardType::Airlift):

                order = new WarzoneOrder::Airlift(player, nullptr, nullptr, 0);
                break;

            case(CardType::Diplomacy):

                order = new WarzoneOrder::Negotiate(player, nullptr);
                break;

            case(CardType::Unknown):

                order = nullptr;

        }

        //Check if the order is valid, and if the player has a valid orderList, add the order to the list
        if(order != nullptr && player -> getPlayerOrders() != nullptr) { player -> getPlayerOrders() -> addOrder(order); }

    }


    // ================= Deck ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Deck::Deck() {
        
        //Create 20 cards (5 of each type). Not too sure how many cards they'll want from us by default
        for(int i = 0; i < 5; i++) {

            cards.push_back(new Card(CardType::Bomb));
            cards.push_back(new Card(CardType::Blockade));
            cards.push_back(new Card(CardType::Airlift));
            cards.push_back(new Card(CardType::Diplomacy));

        }

        //Shuffle Deck
        unsigned seed = static_cast<unsigned>(TimeUtil::getSystemTimeNano());
        std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));

        this -> numOfPlayers = 1; // Default to 1 player if not set

    }

    Deck::Deck(int numOfPlayers){ 

        for(int i = 0; i < numOfPlayers; i++) {

            for(int i = 0; i < 5; i++) {

                cards.push_back(new Card(CardType::Bomb));
                cards.push_back(new Card(CardType::Blockade));
                cards.push_back(new Card(CardType::Airlift));
                cards.push_back(new Card(CardType::Diplomacy));

            }

        }

        //Shuffle Deck
        unsigned seed = static_cast<unsigned>(TimeUtil::getSystemTimeNano());
        std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));

        this -> numOfPlayers = numOfPlayers;

    }

    Deck::~Deck() {

        for(Card* c : cards) { delete c; }

        cards.clear();
    }

    Deck::Deck(const Deck& other) {
        
        for(Card* c : other.cards) { cards.push_back(new Card(*c)); }
        numOfPlayers = other.numOfPlayers;
    
    }

    Deck& Deck::operator=(const Deck& other) {

        if (this != &other) {

            //Free old memory
            for (Card* c : cards) { delete c; }
            cards.clear();

            //Add cards to the new object
            for (Card* c : other.cards) { cards.push_back(new Card(*c)); }

            numOfPlayers = other.numOfPlayers;

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Deck& deck) {

        os << "Deck contains " << deck.cards.size() << " cards: [";

        for (size_t i = 0; i < deck.cards.size(); i++) {

            os << *deck.cards[i]; //Print out card details
            if (i < deck.cards.size() - 1) os << ", "; //Manage commas

        }

        os << "]";
        return os;

    }

    //-- Accessors and Mutators --//

    const vector<Card*>& Deck::getCards() const { return cards; }

    void Deck::setNumOfPlayers(int numberOfPlayers){
     
        if(numOfPlayers >= 2) { //Valid number of players
        
            this -> numOfPlayers = numberOfPlayers;
        
        } else { return; } //Do nothing if the number of players is invalid

        for(Card* c : cards) { delete c; } // Free old memory
        cards.clear();

        for(int i = 0; i < numberOfPlayers; i++) { //Regenerate deck based on new player count

            for(int i = 0; i < 5; i++) {

                cards.push_back(new Card(CardType::Bomb));
                cards.push_back(new Card(CardType::Blockade));
                cards.push_back(new Card(CardType::Airlift));
                cards.push_back(new Card(CardType::Diplomacy));

            }

        }

    }

    //-- Class Methods --//

    Card* Deck::draw() {

        if(cards.empty()) return nullptr;

        //Use system time % deck size to get random index
        int index = static_cast<int>(TimeUtil::getSystemTimeNano() % cards.size());

        //Get drawn card from deck, and remove it from the deck
        Card* drawn = cards[index];
        cards.erase(cards.begin() + index);

        return drawn;

    }

    Card* Deck::draw(CardType desiredType) {

        if(cards.empty()) { return nullptr; }

        //Look for the first matching card. Use iterator to handle removing mid for loop 
        for(auto it = cards.begin(); it != cards.end(); it++) {

            Card* c = *it;

            if(c != nullptr && c->getType() == desiredType) {

                cards.erase(it); // remove from deck
                return c; // return pointer to caller
            
            }

        }

        //If not found, just return nullptr
        return nullptr;
    }


    void Deck::returnToDeck(Card* card) {
    
        if(card != nullptr){
            
            cards.push_back(card); 
        } 
    
    }


    // ================= Hand ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Hand::Hand() { handCards = {}; }

    Hand::Hand(vector<Card*> handOfCards){ this -> handCards = handOfCards; }

    Hand::~Hand() {

        for (Card* c : handCards) delete c;
        handCards.clear();

    }

    Hand::Hand(const Hand& other) { this->handCards = other.handCards; }

    Hand& Hand::operator=(const Hand& other) {

        if (this != &other) {

            this->handCards = other.handCards;

        }

        return *this;
    
    }

    ostream& operator<<(ostream& os, const Hand& hand) {

        os << "Hand contains " << hand.handCards.size() << " cards: [";

        for(size_t i = 0; i < hand.handCards.size(); i++) {

            os << *hand.handCards[i];
            if (i < hand.handCards.size() - 1) os << ", ";
        
        }
        
        os << "]";
        return os;
    
    }

    //-- Accessors and Mutators --//

    const vector<Card*>& Hand::getHandCards() const { return handCards; }

    void Hand::setHandCards(const vector<Card*>& newHandCards) { this -> handCards = newHandCards; }

    //-- Class Methods --//

    void Hand::addCardToHand(Card* card) {

        if(card != nullptr) {

            handCards.push_back(card);
        
        }
    
    }

    void Hand::playCard(Card* card, Player* player, Deck* deck) {

        //Make sure the card, player, and deck are instantiated
        if (card == nullptr || player == nullptr || deck == nullptr) return;

        // Step 1: play the card (create and add Order)
        card -> play(player);

        // Step 2: remove from hand
        removeCardFromHand(card);

        // Step 3: return to deck
        deck -> returnToDeck(card);

    }

    void Hand::removeCardFromHand(Card* card) {

        if(card == nullptr || handCards.empty()) { return; } //Safety check

        //Get index for which the given card in the hand
        auto it = find(handCards.begin(), handCards.end(), card);

        //Check if the card exists in the Hand
        if(it != handCards.end()) { handCards.erase(it); }
    
    }

    bool Hand::hasCardOfType(WarzoneCard::CardType cardType) const { 
        
        for (Card* c : handCards) { 

            if (c != nullptr && c -> getType() == cardType) { return true; } 

        }

        return false;

    }

    Card* Hand::getCardOfType(CardType type) const {

        //Safety check: no cards in hand
        if(handCards.empty()) { return nullptr; }

        //Iterate over all cards to find the first match
        for(Card* c : handCards) {

            //Check if card exists, and if it's the same type as inputted in method
            if(c != nullptr && c -> getType() == type) { return c; }

        }

        //Return nullptr if no matching card was found
        return nullptr;

    }


}
