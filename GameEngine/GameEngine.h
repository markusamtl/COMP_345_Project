#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>

#include "../Map/Map.h"
#include "../Player/Player.h"
#include "../Order/Order.h"
#include "../Card/Card.h"

namespace WarzoneEngine {

    using std::string;
    using std::vector;
    using std::unordered_map;
    using std::queue;

    using WarzoneMap::Map;
    using WarzoneMap::MapLoader;
    using WarzoneMap::Territory;
    using WarzoneMap::Continent;

    using WarzonePlayer::Player;

    using WarzoneOrder::OrderList;
    using WarzoneOrder::Deploy;
    using WarzoneOrder::Advance;
    using WarzoneOrder::TimeUtil;

    using WarzoneCard::Deck;
    using WarzoneCard::Hand;
    using WarzoneCard::Card;
    using WarzoneCard::CardType;


    /*------------------------------------------ENGINE STATE ENUM--------------------------------------------------*/

    /**
     * @enum EngineState
     * @brief Represents the different phases of the Warzone game engine state machine.
     *
     * Each state defines a specific phase of gameplay flow:
     * - Start: Initial state before any map is loaded.
     * - MapLoaded: A map has been successfully loaded.
     * - MapValidated: The loaded map has passed validation checks.
     * - PlayersAdded: All players have been added to the game.
     * - AssignReinforcement: Players receive their reinforcements based on controlled territories.
     * - IssueOrders: Players create orders for the current turn.
     * - ExecuteOrders: Orders are executed interleaved between players.
     * - Win: A player has achieved victory by controlling all continents.
     * - End: The game has concluded.
     */
    enum class EngineState {
        Start,
        MapLoaded,
        MapValidated,
        PlayersAdded,
        AssignReinforcement,
        IssueOrders,
        ExecuteOrders,
        Win,
        End
    };

    /**
     * @brief Stream insertion operator for EngineState.
     * @param os The output stream.
     * @param s The EngineState to output.
     * @return Reference to the output stream.
     */
    std::ostream& operator<<(std::ostream& os, const EngineState& s);


    /*------------------------------------------GAME ENGINE CLASS--------------------------------------------------*/

    /**
     * @class GameEngine
     * @brief Central controller class managing the Warzone game’s state machine and flow.
     *
     * The GameEngine class coordinates map loading, validation, player management,
     * reinforcement assignment, order issuing, and order execution in a turn-based loop.
     *
     * It also manages the transitions between different states, ensuring the proper
     * sequence of commands according to the rules of Warzone.
     *
     * @see WarzoneMap::Map
     * @see WarzonePlayer::Player
     * @see WarzoneCard::Deck
     * @see WarzoneOrder::OrderList
     */
    class GameEngine {
    private:

        //-- Game Attributes --//

        EngineState state;          ///< Current state of the game engine
        Map* gameMap;               ///< Pointer to the current map 
        Deck* deck;                 ///< Pointer to the game’s deck of cards 
        vector<Player*> players;    ///< Vector of active players

        // === Queue-based turn system ===
        std::queue<Player*> playerQueue;  ///< Queue managing player turn order
        Player* currentPlayer = nullptr;  ///< Pointer to the currently active player

        //-- Helper and Internal Methods --//

        /**
         * @brief Validates if the current state matches an expected one before executing a command.
         * @param expected The expected EngineState for the command.
         * @param cmdName The name of the command being executed.
         * @return True if the current state matches the expected state, false otherwise.
         */
        bool isCurrentStateCorrect(EngineState expected, const string& cmdName);

        /**
         * @brief Clears all game-related data, including players, map, and deck.
         * 
         * Frees dynamically allocated memory for map, deck, and player objects,
         * and resets the engine to a clean state.
         */
        void clearGame();

        /**
         * @brief Assigns territories from the map to players at the start of the game.
         */
        void assignCountries();

        /**
         * @brief Computes reinforcement count for a given player based on owned territories and continents.
         * @param p Pointer to the player.
         * @return The total reinforcement armies to allocate.
         */
        int computeReinforcementFor(Player* p) const;

        /**
         * @brief Executes all orders for players in an interleaved manner.
         * @param players Vector of player pointers representing turn order.
         */
        void executeOrders(vector<WarzonePlayer::Player*>& players);

    public:

        /*------------------------------------Constructors & Operators-------------------------------------------*/

        /**
         * @brief Default constructor.
         *
         * Initializes a new GameEngine instance in the Start state,
         * with no map, deck, or players loaded.
         */
        GameEngine();

        /**
         * @brief Destructor.
         *
         * Cleans up all owned memory, including players, map, and deck.
         */
        ~GameEngine();

        /**
         * @brief Copy constructor.
         *
         * Creates a deep copy of another GameEngine, duplicating its map, deck, and player data.
         * @param other The GameEngine object to copy from.
         */
        GameEngine(const GameEngine& other);

        /**
         * @brief Assignment operator.
         *
         * Assigns another GameEngine’s data to this one by performing a deep copy
         * of its internal structures.
         * @param other The GameEngine object to copy from.
         * @return Reference to this GameEngine instance.
         */
        GameEngine& operator=(const GameEngine& other);


        /*------------------------------------Accessors and Mutators--------------------------------------------*/

        /**
         * @brief Get the currently loaded map.
         * @return Pointer to the Map object.
         */
        Map* getGameMap() const;

        /**
         * @brief Set the current game map.
         * @param map Pointer to a Map object.
         */
        void setGameMap(Map* map);

        /**
         * @brief Get the current deck.
         * @return Pointer to the Deck object.
         */
        Deck* getDeck() const;

        /**
         * @brief Set the game deck.
         * @param d Pointer to a Deck object.
         */
        void setDeck(Deck* d);

        /**
         * @brief Get the list of players.
         * @return Const reference to the vector of player pointers.
         */
        const vector<Player*>& getPlayers() const;

        /**
         * @brief Replace the list of players.
         * @param newPlayers New vector of player pointers.
         */
        void setPlayers(const vector<Player*>& newPlayers);

        /**
         * @brief Get the current player queue.
         * @return A copy of the player queue.
         */
        queue<Player*> getPlayerQueue() const;

        /**
         * @brief Set the player queue.
         * @param q The new player queue.
         */
        void setPlayerQueue(const queue<Player*>& q);

        /**
         * @brief Get the currently active player.
         * @return Pointer to the current player.
         */
        Player* getCurrentPlayer() const;

        /**
         * @brief Set the currently active player.
         * @param player Pointer to the new current player.
         */
        void setCurrentPlayer(Player* player);

        /**
         * @brief Get the current engine state.
         * @return Const reference to the EngineState.
         */
        const EngineState& getState() const { return state; }


        /*-------------------------------------------Game Commands-----------------------------------------------*/

        /**
         * @brief Loads a map file into memory.
         * @param path The file path to the map.
         * @return Status message describing success or failure.
         */
        string loadmap(const string& path);

        /**
         * @brief Validates the currently loaded map for correctness.
         * @return Status message indicating validation result.
         */
        string validatemap();

        /**
         * @brief Adds a player to the game.
         * @param name Name of the new player.
         * @return Status message indicating result.
         */
        string addplayer(const string& name);

        /**
         * @brief Starts the game after all players and map are ready.
         * @return Status message indicating transition to reinforcement phase.
         */
        string gamestart();

        /**
         * @brief Assigns reinforcements to all players.
         * @return Status message indicating transition to IssueOrders phase.
         */
        string assignreinforcement();

        /**
         * @brief Prompts players to issue their orders.
         * @return Status message.
         */
        string issueorder();

        /**
         * @brief Ends the IssueOrders phase.
         * @return Status message indicating transition to ExecuteOrders phase.
         */
        string endissueorders();

        /**
         * @brief Executes all player orders interleaved.
         * @return Status message indicating next phase or win condition.
         */
        string executeOrder();

        /**
         * @brief Ends the ExecuteOrders phase and loops back to reinforcement.
         * @return Status message indicating transition.
         */
        string endExecuteOrder();

        /**
         * @brief Handles win condition state.
         * @return Status message confirming win.
         */
        string win();

        /**
         * @brief Ends the game and transitions to the End state.
         * @return Status message confirming end of game.
         */
        string end();

        /**
         * @brief Processes a text-based command and executes the corresponding method.
         * @param cmd The command name (case-insensitive).
         * @param arg Optional argument for the command.
         * @return Status message from the executed command.
         */
        string processCommand(const string& cmd, const string& arg = "");


        /*-----------------------------------------Player Queue Management---------------------------------------*/

        /**
         * @brief Adds a new player to the queue system.
         * @param playerName The name of the new player.
         */
        void addPlayer(const std::string& playerName);

        /**
         * @brief Adds multiple players automatically for testing or demo setup.
         * @param count The number of players to add (default: 3).
         */
        void addPlayers(int count = 3);

        /**
         * @brief Retrieves the next player in the queue without modifying it.
         * @return Pointer to the next player, or nullptr if queue is empty.
         */
        WarzonePlayer::Player* getNextPlayer();

        /**
         * @brief Advances the queue to the next player’s turn.
         */
        void nextTurn();

        /**
         * @brief Checks if there are any players currently in the game.
         * @return True if player queue is not empty.
         */
        bool hasPlayers() const;


        /*--------------------------------------Stream Insertion Operator----------------------------------------*/

        /**
         * @brief Stream insertion operator for GameEngine.
         *
         * Outputs a summary of the current game state, including
         * number of players, whether the map and deck are initialized,
         * and the active state.
         *
         * @param os The output stream.
         * @param engine The GameEngine object to output.
         * @return Reference to the output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine);
    };

} // namespace WarzoneEngine
