#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "../Map/Map.h"
#include "../Player/Player.h"
#include "../Order/Order.h"
#include "../Card/Card.h"

namespace WarzoneEngine {

    using std::string;
    using std::vector;
    using std::unordered_map;

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

    // Convenience to print state names
    std::ostream& operator<<(std::ostream& os, const EngineState& s);

    // ----------------------- Game Engine -------------------------
    class GameEngine {
    private:
        EngineState state;

        Map* gameMap;                 // owned
        Deck* deck;                   // owned
        vector<Player*> players;      // owned

        // Reinforcements for current turn
        unordered_map<Player*, int> reinforcementPool;

        // helpers
        bool requireState(EngineState expected, const string& cmdName);

        void clearGame(); // deletes map, deck, players

        // internal phases
        void assignCountries();  // distribute territories to players
        int  computeReinforcementFor(Player* p) const; // base+continents
        void autoSeedOrdersFromReinforcements();       // simple demo issuing

    public:
        GameEngine();
        ~GameEngine();

        // core API called by driver/console
        const EngineState& getState() const { return state; }

        // commands (return a short status string as per your previous style)
        string loadmap(const string& path);
        string validatemap();
        string addplayer(const string& name);
        string gamestart();

        string assignreinforcement(); // extended integration
        string issueorder();
        string endissueorders();
        string execorder();
        string endexecorders();
        string win();
        string end();

        // console dispatcher (used by driver)
        // accepts raw command (first token) and an optional argument
        string processCommand(const string& cmd, const string& arg = "");
    };

} // namespace WarzoneEngine
