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

    // ----------------------- State Machine -----------------------
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

    std::ostream& operator<<(std::ostream& os, const EngineState& s);

    // ----------------------- Game Engine -------------------------
    class GameEngine {
    private:
        EngineState state;

        Map* gameMap;                 // owned
        Deck* deck;                   // owned
        vector<Player*> players;      // owned

        // === Queue-based turn system ===
        std::queue<Player*> playerQueue;
        Player* currentPlayer = nullptr;

        // === Helpers ===
        bool isCurrentStateCorrect(EngineState expected, const string& cmdName);
        void clearGame();

        // === Internal phases ===
        void assignCountries();
        int  computeReinforcementFor(Player* p) const;
        void executeOrders(vector<WarzonePlayer::Player*>& players);

    public:
        GameEngine();
        ~GameEngine();

        const EngineState& getState() const { return state; }

        // === Commands ===
        string loadmap(const string& path);
        string validatemap();
        string addplayer(const string& name);
        string gamestart();

        string assignreinforcement();
        string issueorder();
        string endissueorders();
        string executeOrder();
        string endExecuteOrder();
        string win();
        string end();

        string processCommand(const string& cmd, const string& arg = "");

        // === Queue methods ===
        void addPlayer(const std::string& playerName);
        void addPlayers(int count = 3);
        WarzonePlayer::Player* getNextPlayer();
        void nextTurn();
        bool hasPlayers() const;
    };

} // namespace WarzoneEngine
