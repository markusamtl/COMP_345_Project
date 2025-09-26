#pragma once
#include <iostream>
#include <vector>
#include <string>

namespace WarzoneCard {

    using namespace std;

    // ================= Card =================
    class Card {
        private:
            string type; // e.g., "bomb", "reinforcement", "blockade", "airlift", "diplomacy"

        public:
            // Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator
            Card();
            Card(const string& type);
            ~Card();
            Card(const Card& other);
            Card& operator=(const Card& other);
            friend ostream& operator<<(ostream& os, const Card& card);

            // Accessor & Mutator
            const string& getType() const;
            void setType(const string& type);

            // Class Methods
            void play(); // skeleton only
    };

    // ================= Deck =================
    class Deck {
        private:
            vector<Card*> cards; // must store pointers because Card is user-defined

        public:
            // Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator
            Deck();
            ~Deck();
            Deck(const Deck& other);
            Deck& operator=(const Deck& other);
            friend ostream& operator<<(ostream& os, const Deck& deck);

            // Class Methods
            Card* draw(); // skeleton only
    };

    // ================= Hand =================
    class Hand {
        private:
            vector<Card*> cardsInHand; // must store pointers because Card is user-defined

        public:
            // Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator
            Hand();
            ~Hand();
            Hand(const Hand& other);
            Hand& operator=(const Hand& other);
            friend ostream& operator<<(ostream& os, const Hand& hand);

            // Class Methods
            void addCard(Card* card);
            void removeCard(Card* card);
    };

} // namespace WarzoneCard
