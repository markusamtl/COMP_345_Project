#include "Card.h"
#include <algorithm>
#include <random>

namespace WarzoneCard {

    // ===== Helper =====
    string cardTypeToString(CardType type) {
        switch (type) {
            case CardType::Unknown: return "unknown";
            case CardType::Bomb: return "bomb";
            case CardType::Reinforcement: return "reinforcement";
            case CardType::Blockade: return "blockade";
            case CardType::Airlift: return "airlift";
            case CardType::Diplomacy: return "diplomacy";
        }
        return "invalid";
    }

    // ===== Card =====
    Card::Card() : type(CardType::Unknown) {} // default to Unknown
    Card::Card(CardType t) : type(t) {}
    Card::~Card() {}
    Card::Card(const Card& other) : type(other.type) {}
    Card& Card::operator=(const Card& other) {
        if (this != &other) {
            type = other.type;
        }
        return *this;
    }
    ostream& operator<<(ostream& os, const Card& card) {
        os << "Card(" << cardTypeToString(card.type) << ")";
        return os;
    }
    CardType Card::getType() const { return type; }
    void Card::setType(CardType t) { type = t; }
    void Card::play() {
        cout << "Playing card: " << cardTypeToString(type) << endl;
        // skeleton: would create an Order and return to deck
    }

    // ===== Deck =====
    Deck::Deck() {}
    Deck::~Deck() {
        for (auto* c : cards) delete c;
        cards.clear();
    }
    Deck::Deck(const Deck& other) {
        for (auto* c : other.cards) {
            cards.push_back(new Card(*c));
        }
    }
    Deck& Deck::operator=(const Deck& other) {
        if (this != &other) {
            for (auto* c : cards) delete c;
            cards.clear();
            for (auto* c : other.cards) {
                cards.push_back(new Card(*c));
            }
        }
        return *this;
    }
    ostream& operator<<(ostream& os, const Deck& deck) {
        os << "Deck with " << deck.cards.size() << " cards.";
        return os;
    }
    Card* Deck::draw() {
        if (cards.empty()) return nullptr;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, cards.size() - 1);
        int idx = dist(gen);
        Card* c = cards[idx];
        cards.erase(cards.begin() + idx);
        return c;
    }
    void Deck::addCard(Card* card) {
        if (card) cards.push_back(card);
    }

    // ===== Hand =====
    Hand::Hand() {}
    Hand::~Hand() {
        for (auto* c : cardsInHand) delete c;
        cardsInHand.clear();
    }
    Hand::Hand(const Hand& other) {
        for (auto* c : other.cardsInHand) {
            cardsInHand.push_back(new Card(*c));
        }
    }
    Hand& Hand::operator=(const Hand& other) {
        if (this != &other) {
            for (auto* c : cardsInHand) delete c;
            cardsInHand.clear();
            for (auto* c : other.cardsInHand) {
                cardsInHand.push_back(new Card(*c));
            }
        }
        return *this;
    }
    ostream& operator<<(ostream& os, const Hand& hand) {
        os << "Hand with " << hand.cardsInHand.size() << " cards.";
        return os;
    }
    void Hand::addCard(Card* card) {
        if (card) cardsInHand.push_back(card);
    }
    void Hand::removeCard(Card* card) {
        auto it = find(cardsInHand.begin(), cardsInHand.end(), card);
        if (it != cardsInHand.end()) {
            delete *it;
            cardsInHand.erase(it);
        }
    }

} // namespace WarzoneCard
