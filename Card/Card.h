#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Forward declarations to avoid circular dependencies
namespace WarzonePlayer { class Player; }  
namespace WarzoneOrder { class Order; class OrderList; class TimeUtil; }

namespace WarzoneCard {

    using WarzonePlayer::Player;
    using WarzoneOrder::Order;
    using WarzoneOrder::TimeUtil;

    // ================= CardType Enum ================= //
    enum class CardType {

        Unknown,
        Bomb,
        Blockade,
        Airlift,
        Diplomacy

    };

    // ================= Card ================= //
        /**
     * @class Card
     * @brief Represents a single Warzone card of a specific type.
     * 
     * A Card can be drawn from a Deck, held in a Player's Hand, and played to create an Order.
     * Once played, the Card is removed from the Hand and returned to the Deck.
     */
    class Card {

        private:

            /**
             * @brief The type of this card (Bomb, Reinforcement, Blockade, Airlift, Diplomacy).
             */
            CardType type;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Default constructor.
             * 
             * Initializes the Card with a default type (e.g., Bomb).
             */
            Card();

            /**
             * @brief Parameterized constructor.
             * 
             * Creates a Card with a specified type.
             * @param t The CardType to assign to this card.
             */
            Card(CardType t);

            /**
             * @brief Destructor.
             */
            ~Card();

            /**
             * @brief Copy constructor.
             * Creates a deep copy of another Card.
             * @param other The Card to copy from.
             */
            Card(const Card& other);

            /**
             * @brief Assignment operator.
             * Assigns the values of one Card to another.
             * @param other The Card to assign from.
             * @return Reference to this Card.
             */
            Card& operator=(const Card& other);

            /**
             * @brief Stream insertion operator.
             * Outputs a textual representation of the Card to an output stream.
             * @param os The output stream.
             * @param card The Card to output.
             * @return Reference to the output stream.
             */
            friend std::ostream& operator<<(std::ostream& os, const Card& card);

            //-- Accessors and Mutators --//

            /**
             * @brief Getter for the card type.
             * @return The CardType of this card.
             */
            CardType getType() const;

            /**
             * @brief Setter for the card type.
             * @param t The new CardType to assign.
             */
            void setType(CardType t);

            //-- Class Methods --//

            /**
             * @brief Play this card.
             * 
             * Creates an Order corresponding to the card type,
             * adds it to the specified Player's OrderList, and then
             * returns the card back to the Deck.
             * @param player Pointer to the Player who is playing the card.
             * @param deck Deck which the played card will be returned to.
             */
            void play(Player* player); 

    };


    // ================= Deck ================= //
        /**
     * @class Deck
     * @brief Represents the global collection of Warzone cards available for players to draw.
     * 
     * The Deck starts with a finite set of cards of all types.
     * Players may draw cards randomly from the Deck into their Hand.
     * When a Card is played, it is returned back into the Deck.
     */
    class Deck {

        private:

            /**
             * @brief Container of all cards in the deck.
             */
            vector<Card*> cards;
            int numOfPlayers;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Default constructor.
             * Initializes the Deck
             */
            Deck();

            /**
             * @brief Parameterized constructor.
             * Initializes the Deck with a predefined set of Warzone cards, based on the number of players.
             * @param numberOfPlayers Number of players in the game (used to scale deck size).
             */
            Deck(int numOfPlayers);

            /**
             * @brief Destructor.
             */
            ~Deck();

            /**
             * @brief Copy constructor.
             * 
             * Creates a deep copy of another Deck, duplicating all cards.
             * @param other The Deck to copy from.
             */
            Deck(const Deck& other);

            /**
             * @brief Assignment operator.
             * Assigns another Deck to this one by duplicating all cards.
             * @param other The Deck to assign from.
             * @return Reference to this Deck.
             */
            Deck& operator=(const Deck& other);

            /**
             * @brief Stream insertion operator.
             * Outputs all cards currently in the Deck to an output stream.
             * @param os The output stream.
             * @param deck The Deck to output.
             * @return Reference to the output stream.
             */
            friend std::ostream& operator<<(std::ostream& os, const Deck& deck);

            //-- Accessors and Mutators --//

            /**
             * @brief Accessor for the cards in the deck.
             * @return Const reference to the vector of Card pointers.
             */
            const std::vector<Card*>& getCards() const;

            //Do NOT have setter of cards, as cards should only be created at the start of the game. 

            //Do NOT have getter for nuber of players, as only the game engine should provide that information.

            /**
             * @brief Sets the number of cards in the deck (used only internally or for testing).
             * @param numberOfPlayers New player count.
             */
            void setNumOfPlayers(int numberOfPlayers);

            //-- Class Methods --//

            /**
             * @brief Draw a random card from the deck.
             * 
             * Removes a randomly selected card from the Deck and returns it
             * so that it can be placed into a Player's Hand.
             * @return Pointer to the drawn Card.
             */
            Card* draw();

            /**
             * @brief Return a card back into the deck.
             * 
             * Called when a Player has played a card; the card is removed from the Hand
             * and reinserted back into the Deck.
             * @param card Pointer to the Card being returned.
             */
            void returnToDeck(Card* card);

    };

        /**
     * @class Hand
     * @brief Represents the collection of cards currently held by a Player.
     * 
     * Cards in a Hand are drawn from the Deck. 
     * When a card is played, it is removed from the Hand and returned to the Deck.
     */
    class Hand {

        private:

            /**
             * @brief Container of cards in the player's hand.
             */
            vector<Card*> handCards;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Default constructor.
             * 
             * Initializes an empty Hand.
             */
            Hand();

            /**
             * @brief Parameterized constructor.
             * Creates a hand with cards
             * @param handOfCards Hand with cards
             */
            Hand(vector<Card*> handOfCards);

            /**
             * @brief Destructor.
             * 
             * Cleans up all dynamically allocated Card objects in the hand.
             */
            ~Hand();

            /**
             * @brief Copy constructor.
             * 
             * Creates a deep copy of another Hand, duplicating all cards.
             * @param other The Hand to copy from.
             */
            Hand(const Hand& other);

            /**
             * @brief Assignment operator.
             * 
             * Assigns another Hand to this one by duplicating all cards.
             * @param other The Hand to assign from.
             * @return Reference to this Hand.
             */
            Hand& operator=(const Hand& other);

            /**
             * @brief Stream insertion operator.
             * 
             * Outputs all cards currently in the Hand to an output stream.
             * @param os The output stream.
             * @param hand The Hand to output.
             * @return Reference to the output stream.
             */
            friend std::ostream& operator<<(std::ostream& os, const Hand& hand);

            //-- Accessors and Mutators --//

            /**
             * @brief Accessor for the cards in the hand.
             * @return Const reference to the vector of Card pointers.
             */
            const std::vector<Card*>& getHandCards() const;

            /**
             * @brief Mutator for the cards in the hand.
             * @param newHandCards The new vector of Card pointers to replace the current hand.
             */
            void setHandCards(const std::vector<Card*>& newHandCards);

            //-- Class Methods --//

            /**
             * @brief Add a card to the hand.
             * @param card Pointer to the Card being added.
             */
            void addCardToHand(Card* card);

            /**
             * @brief Play a card from the hand.
             * 
             * Removes the specified Card from the Hand, plays it
             * (generating an Order for the Player), and then returns it
             * back into the Deck.
             * 
             * @param card Pointer to the Card to be played.
             * @param player Pointer to the Player who is playing the card.
             * @param deck Pointer to the Deck where the card will be returned.
             */
            void playCard(Card* card, Player* player, Deck* deck);


            /**
             * @brief Remove a card from the hand.
             * @param card Pointer to the Card being removed.
             */
            void removeCardFromHand(Card* card);

            /** 
             * @brief Check if a card of a specific type is in the hand.
             * @param cardType The CardType to check for.
             * @return True if a card of the specified type is found, false otherwise.
             */
            bool hasCardOfType(WarzoneCard::CardType cardType) const;

    };

}