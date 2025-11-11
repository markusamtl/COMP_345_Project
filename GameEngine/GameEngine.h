#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <filesystem>

#include "../Map/Map.h"
#include "../Player/Player.h"
#include "../Order/Order.h"
#include "../Card/Card.h"
#include "../LoggingObserver/LoggingObserver.h"

namespace WarzoneEngine {

    using std::string;
    using std::vector;
    using std::unordered_map;
    using std::queue;
    namespace fs = std::filesystem;

    using namespace WarzoneMap;

    using WarzonePlayer::PlayerTerrContainer;
    using WarzonePlayer::Player;

    using WarzoneOrder::OrderList;
    using WarzoneOrder::Order;
    using WarzoneOrder::OrderType;
    using WarzoneOrder::Deploy;
    using WarzoneOrder::Advance;
    using WarzoneOrder::Bomb;
    using WarzoneOrder::Blockade;
    using WarzoneOrder::Airlift;
    using WarzoneOrder::Negotiate;
    using WarzoneOrder::TimeUtil;

    using WarzoneCard::Deck;
    using WarzoneCard::Hand;
    using WarzoneCard::Card;
    using WarzoneCard::CardType;

    using WarzoneLog::Subject;
    using WarzoneLog::ILoggable;


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
     * - Win: A player has achieved victory by being the last one standing, or by controlling all continents
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
    class GameEngine : public WarzoneLog::Subject, public WarzoneLog::ILoggable {

        private:

            //------- Game Attributes -------//

            EngineState state;
            Map* gameMap; 
            Deck* deck; 
            vector<Player*> players;
            int turn;
            int maxTurns;

            // === Queue-based turn system ===
            queue<Player*> playerQueue;
            Player* currentPlayer = nullptr;

             /**
             * @brief Latest descriptive log message about engine activity.
             * 
             * This field holds human-readable context such as phase transitions
             * or command results. It is updated whenever a significant event occurs,
             * and returned via stringToLog() for observer-based logging.
             */
            std::string engineLogMessage;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

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
            friend ostream& operator<<(std::ostream& os, const GameEngine& engine);


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
             * @brief Accessor for number of turns elapsed
             * @return Current turn of the game
             */
            int getTurn() const;

            /**
             * @brief Mutator for number of turns elapse
             * @param turn Turn to set it to
             */
            void setTurn(int turn);

            /**
             * @brief Accessor for maximum number of turns to be elapsed
             * @return Maximum number of turns
             */
            int getMaxTurns() const;

            /**
             * @brief Mutator for maximum number of turns to be elapsed
             * @param maxTurns New maximum amount of turns
             */
            void setMaxTurns(int maxTurns);

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
            const EngineState& getState() const;

            /**
             * @brief Get the current state as a string
             * @return Current state, as a string 
             */
            const string getStateAsString() const;

            /**
             * @brief Set the current engine state.
             * @param gameState reference to the desired EngineState.
             */
            void setState(const EngineState& gameState);

            //-------- Helper and Internal Methods ---------//

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
             * @brief Randomly shuffle the players vector for a game
             * @param surpressOutput Boolean value to prevent string generation
             * @return String output of the method
             */
            string shufflePlayers(bool supressOutput);

            /**
             * @brief Assigns territories from the map to players at the start of the game.
             * @param surpressOutput Determine whether to generate string output of method
             * @return String output of the method
             */
            string assignTerritories(bool surpressOutput);

            /**
             * @brief Handles the creation and validation of a Bomb order for a player.
             * 
             * Identifies the optimal target territory for bombing, prioritizing non-neutral
             * enemies, and replaces the placeholder order with a valid Bomb order if possible.
             * 
             * @param p The player issuing the order.
             * @param neutralPlayer Pointer to the neutral player (used to filter targets).
             * @param issuedOrder Pointer to the placeholder order to be replaced.
             * @param surpressOutput Whether console output should be suppressed.
             * @param output Output stream for logging or printing.
             */
            void handleBombOrder(Player* p, Player* neutralPlayer, Order* issuedOrder, bool surpressOutput, ostringstream& output);

            /**
             * @brief Handles the creation and validation of a Blockade order for a player.
             * 
             * Selects the most threatened owned territory bordering an enemy and issues
             * a Blockade order to fortify and neutralize it.
             * 
             * @param p The player issuing the order.
             * @param neutralPlayer Pointer to the neutral player who will take ownership after blockade
             * @param neutral Vector of player references that the current player has a truce with / can not attack
             * @param issuedOrder Pointer to the placeholder order to be replaced.
             * @param surpressOutput Whether console output should be suppressed.
             * @param output Output stream for logging or printing.
             */
            void handleBlockadeOrder(Player* p, Player* neutralPlayer, Order* issuedOrder, bool surpressOutput, ostringstream& output);

            /**
             * @brief Handles the creation and validation of an Airlift order for a player.
             * 
             * Transfers armies from internal (safe) territories to threatened borders,
             * potentially replacing redundant Advance orders. Ensures the player can
             * defend high-risk borders effectively.
             * 
             * @param p The player issuing the order.
             * @param neutralPlayer Pointer to the neutral player (used in temporary comparisons).
             * @param neutrals List of pointers to players the current player should not attack
             * @param issuedOrder Pointer to the placeholder order to be replaced.
             * @param surpressOutput Whether console output should be suppressed.
             * @param output Output stream for logging or printing.
             */
            void handleAirliftOrder(Player* p, Player* neutralPlayer, Order* issuedOrder, bool surpressOutput, ostringstream& output);

            /**
             * @brief Handles the creation and validation of a Negotiate order for a player.
             * 
             * Analyzes enemy proximity and continent threat levels to determine which
             * opponent poses the highest risk, and issues a Negotiate (Diplomacy) order
             * to form a temporary truce.
             * 
             * @param p The player issuing the order.
             * @param neutralPlayer Pointer to the neutral player (ignored for diplomacy logic).
             * @param neutrals List of pointers to players the current player should not attack 
             * @param issuedOrder Pointer to the placeholder order to be replaced.
             * @param surpressOutput Whether console output should be suppressed.
             * @param output Output stream for logging or printing.
             */
            void handleNegotiateOrder(Player* p, Player* neutralPlayer, vector<Player*> neutrals, Order* issuedOrder, bool surpressOutput, ostringstream& output);

            /**
             * @brief Executes the Issue Orders phase of the game.
             * Calls issueOrder() from player
             * Finalizes any card-based orders (Bomb, Blockade, Airlift, Negotiate)
             * by assigning appropriate targets and replacing placeholder orders in the player's OrderList.
             *
            * @param surpressOutput If true, disables console output; otherwise logs actions.
            * @param output String stream reference to collect results of method output
            */
            void issueOrdersPhase(bool surpressOutput, ostringstream& output);

            /**
            * @brief Executes the Execute Orders phase: runs all Deploy orders first (round-robin),
            * then all other orders round-robin; handles truces, card rewards, eliminations,
            * win checks, and increments the turn counter.
            *
            * @param surpressOutput If true, suppresses console/log output from this phase.
            * @param output Aggregated output stream to append human-readable logs.
            */
            void executeOrdersPhase(bool surpressOutput, ostringstream& output);


            /**
             * @brief Computes reinforcement count for a given player based on owned territories and continents.
             * @param p Pointer to the player.
             * @param surpressOutput Whether to capture method string output or not
             * @return The total reinforcement armies to allocate, as well as method execution output.
             */
            pair<int, string> computeReinforcementFor(Player* p, bool surpressOutput) const;

            /**
             * @brief Finds a player in the current player list by name.
             * @param name The name of the player to find.
             * @return Pointer to the matching Player, or nullptr if not found.
             */
            Player* findPlayerByName(const string& name) const;

            /**
             * @brief Resets the game to the initial state, clearing all data.
             * @return True if user decides to restart, false otherwise.
             */
            bool startAgain();

            /**
             * @brief Sets the current engine log message and notifies observers.
             * 
             * This helper ensures that all state transitions or important actions
             * trigger logging output through the Observer pattern.
             * 
             * @param message The message describing the change or event.
             */
            void logAndNotify(const std::string& message);

            /*-------------------------------------------Game Commands-----------------------------------------------*/

            /**
             * @brief Loads a map file into memory.
             * @param path The file path to the map.
             * @param surpressOutput Bool val to generate output or not
             * @return Status message describing success or failure.
             */
            string engineLoadMap(const string& path, bool surpressOutput);

            /**
             * @brief Validates the currently loaded map for correctness.
             * @param surpressOutput Bool val to generate output or not
             * @return Status message indicating validation result.
             */
            string engineValidateMap(bool surpressOutput);

            /**
             * @brief Adds a player to the game.
             * @param name Name of the new player.
             * @param surpressOutput Bool val to generate output or not
             * @return Status message indicating result.
             */
            string engineAddPlayer(const string& name, bool surpressOutput);

            /**
             * @brief Starts the game after all players and map are ready.
             * @param surpressOutput Boolean value to surpress outputs
             * @return Status message indicating transition to reinforcement phase.
             */
            string engineGameStart(bool surpressOutput);

            /**
             * @brief Assigns reinforcements to all players.
             * @param surpressOutput Boolean value to surpress outputs
             * @return Status message indicating transition to IssueOrders phase.
             */
            string reinforcementPhase(bool surpressOutput);

            /**
             * @brief Prompts players to issue their orders.
             * @param surpressOutput Boolean value to surpress outputs
             * @return Status message.
             */
            string engineIssueOrder(bool surpressOutput);

            /**
             * @brief Ends the IssueOrders phase.
             * @param surpressOutput Boolean value to surpress outputs
             * @return Status message indicating transition to ExecuteOrders phase.
             */
            string engineEndIssueOrder(bool surpressOutput);

            /**
             * @brief Executes all player orders interleaved.
             * @param surpressOutput Boolean value to surpress outputs
             * @return Status message indicating next phase or win condition.
             */
            string engineExecuteOrder(bool surpressOutput);

            /**
             * @brief Ends the ExecuteOrders phase and loops back to reinforcement.
             * @param surpressOutput Boolean value to surpress outputs
             * @return Status message indicating transition.
             */
            string engineEndExecuteOrder(bool surpressOutput);

            /**
             * @brief Handles win condition state.
             * @return Status message confirming win.
             */
            string engineWin();

            /**
             * @brief Ends the game and transitions to the End state.
             * @return Status message confirming end of game.
             */
            string engineEnd();

            /**
             * @brief Processes a text-based command and executes the corresponding method.
             * @param cmd The command name (case-insensitive).
             * @param arg Optional argument for the command.
             * @param surpressOutput Boolean value to prevent printing if desired
             * @return Status message from the executed command.
             */
            string processCommand(const string& cmd, const string& arg, bool surpressOutput);


            /*-----------------------------------------Player Queue Management---------------------------------------*/

            /**
             * @brief Adds a new player to the queue system.
             * @param playerName The name of the new player.
             */
            void addPlayerToQueue(const string& playerName);

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

            /*-----------------------------------------Gameplay Execution---------------------------------------*/

            /**
             * @brief UI mechanism that allows a player to start the game
             * @param supressOutput Boolean value that prevents string generation for the method
             * return String result for the method
             */
            string startupPhase(bool surpressOutput);

            /**
             * @brief Runs the complete Warzone main game loop (reinforcement -> issue orders -> execute orders).
             * @param surpressOutput Suppresses console output when true.
             */
            void gameplayPhase(bool surpressOutput);

            /**
             * @brief Handles the game ending
             * @return True if the game is restarted, False if the game is terminated 
             */
            bool endPhase();

            /**
             * @brief Returns the current engine log message.
             * 
             * Used by the Observer pattern to record significant events
             * such as state transitions, phase starts, or player actions.
             * 
             * @return Formatted string representing the most recent engine event.
             */
            std::string stringToLog() override;


            /**
             * @brief Runs the game.
             * - Prompt the user to silence file output or not
             * - Runs startPhase
             * - Runs gameplayPhase
             * - Runs endPhase. If user wants to continue, return to startPhase. If not, break. 
             */
            void mainGameLoop();
            
        };

}
