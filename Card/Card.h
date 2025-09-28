#pragma once
#include <iostream>
#include <vector>
#include <string>

namespace WarzoneCard {

    using namespace std;

    // ================= CardType =================
    enum class CardType { Unknown, Bomb, Reinforcement, Blockade, Airlift, Diplomacy };

    // Helper function for printing enum
    string cardTypeToString(CardType type);

    // ================= Card =================
    class Card {
    private:
        CardType type; // enum type instead of string

    public:
        // Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator
        Card();
        Card(CardType type);
        ~Card();
        Card(const Card& other);
        Card& operator=(const Card& other);
        friend ostream& operator<<(ostream& os, const Card& card);

        // Accessor & Mutator
        CardType getType() const;
        void setType(CardType type);

        // Class Methods
        void play(); // skeleton only
    };

    // ================= Deck =================
    class Deck {
    private:
        vector<Card*> cards;

    public:
        Deck();
        ~Deck();
        Deck(const Deck& other);
        Deck& operator=(const Deck& other);
        friend ostream& operator<<(ostream& os, const Deck& deck);

        Card* draw();
        void addCard(Card* card);
    };

    // ================= Hand =================
    class Hand {
    private:
        vector<Card*> cardsInHand;

    public:
        Hand();
        ~Hand();
        Hand(const Hand& other);
        Hand& operator=(const Hand& other);
        friend ostream& operator<<(ostream& os, const Hand& hand);

        void addCard(Card* card);
        void removeCard(Card* card);

        // For driver/testing convenience
        const vector<Card*>& getCards() const { return cardsInHand; }
    };

} // namespace WarzoneCard
