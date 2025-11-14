#pragma once
#include <string>
#include <unordered_map>
#include "../Player/Player.h"
#include "../Card/Card.h"
#include "../Map/Map.h"

namespace WarzonePlayerStrategy {

    using namespace std;
    using namespace WarzonePlayer;
    using namespace WarzoneCard;
    using namespace WarzoneMap;

    enum class PlayerStrategyType {
        HUMAN,
        AGGRESSIVE,
        BENEVOLENT,
        NEUTRAL,
        CHEATER
    };

    class PlayerStrategy{
        protected:
            PlayerStrategyType strategyType;
        public:
            virtual string issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) = 0;
            virtual unordered_map<Territory*, Territory*> toAttack(Player* player) = 0;
            virtual unordered_map<Territory*, Territory*> toDefend(Player* player) = 0;
            virtual string toAttackString(Player* player) = 0; 
            virtual string toDefendString(Player* player) = 0; 
            virtual PlayerStrategy* clone() const = 0;
            PlayerStrategyType getStrategyType() const;
    };

    class HumanPlayerStrategy : public PlayerStrategy {
        public:
            HumanPlayerStrategy();
            string issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) override;
            unordered_map<Territory*, Territory*> toAttack(Player* player) override;
            unordered_map<Territory*, Territory*> toDefend(Player* player) override;
            string toAttackString(Player* player) override;
            string toDefendString(Player* player) override;
            PlayerStrategy* clone() const override;
    };

    class AggressivePlayerStrategy : public PlayerStrategy {
        public:
            AggressivePlayerStrategy();
            string issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) override;
            unordered_map<Territory*, Territory*> toAttack(Player* player) override;
            unordered_map<Territory*, Territory*> toDefend(Player* player) override;
            string toAttackString(Player* player) override;
            string toDefendString(Player* player) override;
            PlayerStrategy* clone() const override;
    };

    class BenevolentPlayerStrategy : public PlayerStrategy {
        public:
            BenevolentPlayerStrategy();
            string issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) override;
            unordered_map<Territory*, Territory*> toAttack(Player* player) override;
            unordered_map<Territory*, Territory*> toDefend(Player* player) override;
            string toAttackString(Player* player) override;
            string toDefendString(Player* player) override;
            PlayerStrategy* clone() const override;
    };

    class NeutralPlayerStrategy : public PlayerStrategy {
        public:
            NeutralPlayerStrategy();
            string issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) override;
            unordered_map<Territory*, Territory*> toAttack(Player* player) override;
            unordered_map<Territory*, Territory*> toDefend(Player* player) override;
            string toAttackString(Player* player) override;
            string toDefendString(Player* player) override;
            PlayerStrategy* clone() const override;
    };

    class CheaterPlayerStrategy : public PlayerStrategy {
        public:
            CheaterPlayerStrategy();
            string issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) override;
            unordered_map<Territory*, Territory*> toAttack(Player* player) override;
            unordered_map<Territory*, Territory*> toDefend(Player* player) override;
            string toAttackString(Player* player) override;
            string toDefendString(Player* player) override;
            PlayerStrategy* clone() const override;
    };
}