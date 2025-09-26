#include "Card.h"

namespace WarzoneCard {

    // ================= Card =================
    Card::Card() : type("") {}
    Card::Card(const string& type) : type(type) {}
    Card::~Card() {}
    Card::Card(const Card& other) : type(other.type) {}
    Card& Card::operator=(const Card& other) {
        if (this != &other) {
            this->type = other.type;
        }
        return *this;
    }
    ostream& operator<<(ostream& os, const Card& card) {
        os << "Card(Type: " << card.type << ")";
        return os;
    }
    const string& Card::getType() const { return this->type; }
    void Card::setType(const string& type) { this->type = type; }
    void Card::play() {
        // TODO: Implement play logic
    }

    // ================= Deck =================
    Deck::Deck() : cards() {}
    Deck::~Deck() {
        for (Card* c : cards) delete c;
        cards.clear();
    }
    Deck::Deck(const Deck& other) {
        for (Card* c : other.cards) {
            cards.push_back(new Card(*c));
        }
    }
    Deck& Deck::operator=(const Deck& other) {
        if (this != &other) {
            for (Card* c : cards) delete c;
            cards.clear();
            for (Card* c : other.cards) {
                cards.push_back(new Card(*c));
            }
        }
        return *this;
    }
    ostream& operator<<(ostream& os, const Deck& deck) {
        os << "Deck(Size: " << deck.cards.size() << ")";
        return os;
    }
    Card* Deck::draw() {
        // TODO: Implement draw logic
        return nullptr;
    }

    // ================= Hand =================
    Hand::Hand() : cardsInHand() {}
    Hand::~Hand() {
        for (Card* c : cardsInHand) delete c;
        cardsInHand.clear();
    }
    Hand::Hand(const Hand& other) {
        for (Card* c : other.cardsInHand) {
            cardsInHand.push_back(new Card(*c));
        }
    }
    Hand& Hand::operator=(const Hand& other) {
        if (this != &other) {
            for (Card* c : cardsInHand) delete c;
            cardsInHand.clear();
            for (Card* c : other.cardsInHand) {
                cardsInHand.push_back(new Card(*c));
            }
        }
        return *this;
    }
    ostream& operator<<(ostream& os, const Hand& hand) {
        os << "Hand(Size: " << hand.cardsInHand.size() << ")";
        return os;
    }
    void Hand::addCard(Card* card) {
        this->cardsInHand.push_back(card);
    }
    void Hand::removeCard(Card* card) {
        // TODO: Implement removal logic (search and erase pointer)
    }

} // namespace WarzoneCard
