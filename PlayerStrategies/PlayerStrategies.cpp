#include "PlayerStrategies.h"
#include <vector>
#include <iostream>

using namespace std;

namespace WarzonePlayerStrategy {


    PlayerStrategyType PlayerStrategy::getStrategyType() const {
        return strategyType;
    }

    // ======================================================= //
    // ================= HumanPlayerStrategy ================= //
    // ======================================================= //

    HumanPlayerStrategy::HumanPlayerStrategy() {
        this->strategyType = PlayerStrategyType::HUMAN;
    }

    string HumanPlayerStrategy::issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) {
        if (!player || player->getPlayerOrders() == nullptr) {
            return "[IssueOrder] Error: Player or order list not initialized.\n";
        }

        // If in suppressed/silent mode (e.g., tournament), perform automated behavior
        if (surpressOutput) {
            //PART 1: DEPLOYING ARMIES ACROSS WEAKEST TERRITORIES
            player->deployReinforcements(output, surpressOutput);

            //PART 2: TRYING TO USE A CARD TO ORDER
            player->issueCardOrders(output, surpressOutput, gameDeck);

            //PART 3: ADVANCE (ATTACK LOGIC)
            player->issueAttackOrders(output, surpressOutput, neutralPlayer);

            //PART 4: ADVANCE (DEFENCE LOGIC)
            player->issueDefendOrders(output, surpressOutput);

            return output.str();
        }

        // Interactive human flow (only when not suppressed)
        output << "[Human] " << player->getPlayerName() << " is making decisions interactively.\n";

        while (true) {
            // Show concise status and menu
            cout << "\n=== Human Player: " << player->getPlayerName() << " ===\n";
            cout << "Reinforcements available: " << player->getReinforcementPool() << "\n";
            cout << "Owned territories: " << player->getOwnedTerritories().getTerritories().size() << "\n";
            cout << "Choose an action:\n";
            cout << "  1) Deploy reinforcements\n";
            cout << "  2) Play card(s)\n";
            cout << "  3) Issue attack orders\n";
            cout << "  4) Issue defend/movement orders\n";
            cout << "  5) Show possible attacks\" (brief)\n";
            cout << "  6) Show possible defenses\" (brief)\n";
            cout << "  0) Finish and end ordering phase\n";
            cout << "> ";

            string choice;
            if (!std::getline(cin, choice)) {
                // If input closed, fallback to automatic behavior
                output << "[Human] Input stream closed; falling back to automatic actions.\n";
                player->deployReinforcements(output, true);
                player->issueCardOrders(output, true, gameDeck);
                player->issueAttackOrders(output, true, neutralPlayer);
                player->issueDefendOrders(output, true);
                break;
            }

            if (choice.empty()) continue;

            char c = choice[0];
            if (c == '0') {
                output << "[Human] " << player->getPlayerName() << " finished issuing orders interactively.\n";
                break;
            }

            switch (c) {
                case '1':
                    cout << "-- Deploying reinforcements (uses in-game deploy UI/output).\n";
                    player->deployReinforcements(output, false);
                    break;
                case '2':
                    cout << "-- Playing available card orders (if any).\n";
                    player->issueCardOrders(output, false, gameDeck);
                    break;
                case '3':
                    cout << "-- Issue attack orders (you will be prompted by game logic).\n";
                    player->issueAttackOrders(output, false, neutralPlayer);
                    break;
                case '4':
                    cout << "-- Issue defend/movement orders.\n";
                    player->issueDefendOrders(output, false);
                    break;
                case '5':
                    cout << toAttackString(player) << std::flush;
                    break;
                case '6':
                    cout << toDefendString(player) << std::flush;
                    break;
                default:
                    cout << "Invalid choice. Please enter 0-6.\n";
                    break;
            }
        }

        return output.str();
    }

    unordered_map<Territory*, Territory*> HumanPlayerStrategy::toAttack(Player* player) {
        return player->computeAttackMap();
    }

    string HumanPlayerStrategy::toAttackString(Player* player) {
        stringstream retStr;

        //Get all owned territories
        vector<Territory*> owned = player->getOwnedTerritories().getTerritories();

        //Sort owned territories by ID for consistent output
        sort(owned.begin(), owned.end(), Territory::territoryIDCompare);

        //Iterate through all owned territories
        for(Territory* terr : owned) {

            if(terr == nullptr){ continue; }

            retStr << "From: " << terr -> getID() << ", " << player->getPlayerName() << " can attack: ";

            vector<Territory*> neighbors = terr -> getNeighbors();

            //Sort neighbors using the same attack priority logic
            sort(neighbors.begin(), neighbors.end(), Territory::territoryAttackPriorityCompare);

            bool foundEnemy = false;

            for(size_t i = 0; i < neighbors.size(); i++) {

                Territory* neighbor = neighbors[i];
                if(neighbor == nullptr){ continue; }

                Player* neighOwner = neighbor -> getOwner();

                //Check for neutral enemies
                bool isNeutral = false;

                if (neighOwner != nullptr) {

                    isNeutral = find(
                        player->getNeutralEnemies().begin(),
                        player->getNeutralEnemies().end(),
                        neighOwner
                    ) != player->getNeutralEnemies().end();

                }

                //Skip if owned by self, neutral enemy, or no one
                if(neighOwner == player || isNeutral || neighOwner == nullptr){ continue; }

                //Append enemy info
                retStr << neighbor -> getID() << " (" << neighbor -> getNumArmies() << " armies)";
                foundEnemy = true;

                //Comma separation if not last valid enemy
                if(i < neighbors.size() - 1){ retStr << ", "; }

            }

            if(!foundEnemy) {
                retStr << "no enemies";
            }

            retStr << endl;
        }

        return retStr.str();
    }

    unordered_map<Territory*, Territory*> HumanPlayerStrategy::toDefend(Player* player) {
        return player->computeDefendMap();
    }

    string HumanPlayerStrategy::toDefendString(Player* player) {
        //Get all territories owned by this player that border at least one enemy territory
        vector<Territory*> borderTerritories = player->getTerritoriesAdjacentToEnemy();

        if(borderTerritories.empty()) { //This should not happen
            stringstream retStr;
            retStr << "Player " << player->getPlayerName() << " has no territories adjacent to enemies." << endl;
            return retStr.str();
        }

        //Convert border territories to an unordered_set for O(1) lookup
        unordered_set<Territory*> visited(borderTerritories.begin(), borderTerritories.end());

        //Queue for BFS-style propagation
        queue<pair<Territory*, int>> q; //Each pair holds a territory, and a layer distance

        //Add adjacent territories to queue
        for(Territory* t : borderTerritories){ if(t != nullptr){ q.push({t, 0}); } }

        //Map: layer number -> territories at that layer
        unordered_map<int, vector<Territory*>> layerMap;

        //Map: territory -> next defensive target toward the enemy
        unordered_map<Territory*, Territory*> defenseRouting;

        //Perform BFS to propagate inward through owned territories
        while(!q.empty()) {

            //Get current territory info
            pair<Territory*, int> currTerrInfo = q.front();
            q.pop();

            Territory* current = currTerrInfo.first;
            int layer = currTerrInfo.second;

            if(current == nullptr) { continue; }

            //Add this territory to its corresponding layer
            layerMap[layer].push_back(current);

            //Propagate to neighboring owned territories not yet visited
            for(Territory* neighbor : current -> getNeighbors()) {

                if(neighbor == nullptr) { continue; }

                Player* owner = neighbor -> getOwner();

                //Only expand to territories owned by this player and not yet visited
                if(owner == player && visited.find(neighbor) == visited.end()) {

                    visited.insert(neighbor);
                    q.push({neighbor, layer + 1});

                    //Mark that this territory should send reinforcements toward "current"
                    defenseRouting[neighbor] = current;

                }

            }

            //Frontline (enemy-adjacent) territories have no next defensive target
            if(defenseRouting.find(current) == defenseRouting.end()) {
                defenseRouting[current] = nullptr;
            }

        }

        //Build the string output
        stringstream retStr;
        retStr << "Player " << player->getPlayerName() << " defensive propagation structure:" << endl;

        //Sort layers numerically (0, 1, 2, ...)
        vector<int> layers;
        for(pair<int, vector<Territory*>> entry : layerMap) { layers.push_back(entry.first); }
        sort(layers.begin(), layers.end());

        for(int layer : layers) {

            vector<Territory*>& terrs = layerMap[layer];
            if(terrs.empty()) { continue; }

            //Sort territories within this layer by army count (weakest first)
            sort(terrs.begin(), terrs.end(), Territory::territoryNumArmiesCompareAscend);

            if(layer == 0) { retStr << "  Layer " << layer << " (enemy-adjacent):" << endl; } 
            else { retStr << "  Layer " << layer << " (" << layer << " territories away from enemy):" << endl; }

            for(Territory* t : terrs) {

                if(t == nullptr) { continue; }

                Territory* next = defenseRouting[t];

                retStr << "    " << t -> getID() 
                    << " (" << t -> getNumArmies() << " armies)"
                    << "  to  " 
                    << (next ? next -> getID() : "None")
                    << endl;

            }

        }

        return retStr.str();
    }

    PlayerStrategy* HumanPlayerStrategy::clone() const {
        return new HumanPlayerStrategy();
    }

    // ======================================================== //
    // ============== AggressivePlayerStrategy ================ //
    // ======================================================== //

    AggressivePlayerStrategy::AggressivePlayerStrategy() {
        this->strategyType = PlayerStrategyType::AGGRESSIVE;
    }

    string AggressivePlayerStrategy::issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) {
        if (!player || !player->getPlayerOrders()) 
            return "[Aggressive] Error: Player or order list not initialized.\n";

        // === Step 1: Find the strongest owned territory ===
        vector<Territory*> owned = player->getOwnedTerritories().getTerritories();
        if (owned.empty()) return "[Aggressive] Player owns no territories.\n";

        sort(owned.begin(), owned.end(), Territory::territoryNumArmiesCompareDescend);
        Territory* strongest = owned.front(); // Highest army count

        if (!strongest) return "[Aggressive] Strongest territory is invalid.\n";

        if (!surpressOutput) {
            output << "[Aggressive] " << player->getPlayerName()
                   << " has chosen " << strongest->getID()
                   << " as their strongest territory.\n";
        }

        // === Step 2: Deploy all reinforcements to the strongest territory ===
        int reinforcements = player->getReinforcementPool();
        if (reinforcements > 0) {
            Order* deployOrder = new WarzoneOrder::Deploy(player, strongest, reinforcements);
            player->getPlayerOrders()->addOrder(deployOrder);
            player->setReinforcementPool(0);

            if (!surpressOutput) {
                output << "[Aggressive] " << player->getPlayerName()
                       << " deploys all " << reinforcements
                       << " reinforcements to " << strongest->getID() << ".\n";
            }
        }

        // === Step 3: Use aggressive cards (Bomb, Airlift) if available ===
        Hand* hand = player->getHand();
        if (hand) {
            if (hand->hasCardOfType(WarzoneCard::CardType::Bomb)) {
                Card* bombCard = hand->getCardOfType(WarzoneCard::CardType::Bomb);
                hand->removeCardFromHand(bombCard);
                gameDeck->returnToDeck(bombCard);
                if (!surpressOutput)
                    output << "[Aggressive] " << player->getPlayerName() << " uses a Bomb card aggressively.\n";
            }
            if (hand->hasCardOfType(WarzoneCard::CardType::Airlift)) {
                Card* airliftCard = hand->getCardOfType(WarzoneCard::CardType::Airlift);
                hand->removeCardFromHand(airliftCard);
                gameDeck->returnToDeck(airliftCard);
                if (!surpressOutput)
                    output << "[Aggressive] " << player->getPlayerName() << " uses an Airlift card to support attacks.\n";
            }
        }

        // === Step 4: Attack neighboring enemy territories ===
        unordered_map<Territory*, Territory*> attackMap = toAttack(player);
        for (auto& [from, target] : attackMap) {
            if (!from || !target) continue;

            Player* targetOwner = target->getOwner();
            if (!targetOwner || targetOwner == player) continue;

            int sourceArmies = from->getNumArmies();
            if (sourceArmies <= 1) continue; // must leave one behind

            int armiesToSend = max(1, sourceArmies - 1);

            Order* advanceOrder = new WarzoneOrder::Advance(player, from, target, armiesToSend);
            player->getPlayerOrders()->addOrder(advanceOrder);

            if (!surpressOutput) {
                output << "[Aggressive] " << player->getPlayerName()
                       << " advances from " << from->getID()
                       << " (" << sourceArmies << " armies)"
                       << " to attack " << target->getID()
                       << " with " << armiesToSend << " armies.\n";
            }
        }

        output << "[Aggressive] " << player->getPlayerName() << " has finished issuing all aggressive orders.\n";
        return output.str();
    }

    unordered_map<Territory*, Territory*> AggressivePlayerStrategy::toAttack(Player* player) {
        return player->computeAttackMap();
    }

    string AggressivePlayerStrategy::toAttackString(Player* player) {
        stringstream ss;
        ss << "[Aggressive] " << player->getPlayerName() << " focuses attacks from strongest territories.\n";
        auto attackMap = player->computeAttackMap();
        for (auto& [from, to] : attackMap)
            if (from && to)
                ss << "  " << from->getID() << " → " << to->getID() << "\n";
        return ss.str();
    }

    unordered_map<Territory*, Territory*> AggressivePlayerStrategy::toDefend(Player* player) {
        // Minimal defense — only keeps track of frontlines
        unordered_map<Territory*, Territory*> defendMap;
        auto borders = player->getTerritoriesAdjacentToEnemy();
        for (Territory* t : borders)
            defendMap[t] = nullptr;
        return defendMap;
    }

    string AggressivePlayerStrategy::toDefendString(Player* player) {
        return "[Aggressive] " + player->getPlayerName() +
               " focuses entirely on offense — minimal defense maintained.\n";
    }

    PlayerStrategy* AggressivePlayerStrategy::clone() const {
        return new AggressivePlayerStrategy();
    }

    // ======================================================== //
    // =============== BenevolentPlayerStrategy =============== //
    // ======================================================== //

    BenevolentPlayerStrategy::BenevolentPlayerStrategy() {
        this->strategyType = PlayerStrategyType::BENEVOLENT;
    }

    string BenevolentPlayerStrategy::issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) {
        if (!player || !player->getPlayerOrders())
            return "[Benevolent] Error: Player or order list not initialized.\n";

        vector<Territory*> owned = player->getOwnedTerritories().getTerritories();
        if (owned.empty()) return "[Benevolent] Player owns no territories.\n";

        // === STEP 1: Identify the weakest owned territories ===
        sort(owned.begin(), owned.end(), Territory::territoryNumArmiesCompareAscend);
        vector<Territory*> weakest;
        int numToReinforce = min((int)owned.size(), 3); // Reinforce up to 3 weakest
        for (int i = 0; i < numToReinforce; i++) {
            if (owned[i]) weakest.push_back(owned[i]);
        }

        if (!surpressOutput) {
            output << "[Benevolent] " << player->getPlayerName()
                   << " identified weakest territories for reinforcement:\n";
            for (auto* t : weakest)
                if (t) output << "  - " << t->getID() << " (" << t->getNumArmies() << " armies)\n";
        }

        // === STEP 2: Deploy reinforcements evenly among weakest territories ===
        int reinforcements = player->getReinforcementPool();
        if (reinforcements > 0 && !weakest.empty()) {
            int perTerritory = max(1, reinforcements / (int)weakest.size());
            int remaining = reinforcements;

            for (size_t i = 0; i < weakest.size(); i++) {
                Territory* target = weakest[i];
                if (!target) continue;

                int deployAmount = (i == weakest.size() - 1)
                                       ? remaining
                                       : min(perTerritory, remaining);
                remaining -= deployAmount;

                player->getPlayerOrders()->addOrder(
                    new WarzoneOrder::Deploy(player, target, deployAmount)
                );

                if (!surpressOutput)
                    output << "[Benevolent] " << player->getPlayerName()
                           << " deploys " << deployAmount
                           << " armies to " << target->getID() << ".\n";
            }

            player->setReinforcementPool(0);
        }

        // === STEP 3: Use defensive cards if available ===
        Hand* hand = player->getHand();
        if (hand) {
            if (hand->hasCardOfType(WarzoneCard::CardType::Blockade)) {
                Card* bombCard = hand->getCardOfType(WarzoneCard::CardType::Blockade);
                hand->removeCardFromHand(bombCard);
                gameDeck->returnToDeck(bombCard);
                if (!surpressOutput)
                    output << "[Aggressive] " << player->getPlayerName() << " uses a Blockade card aggressively.\n";
            }
            if (hand->hasCardOfType(WarzoneCard::CardType::Airlift)) {
                Card* airliftCard = hand->getCardOfType(WarzoneCard::CardType::Airlift);
                hand->removeCardFromHand(airliftCard);
                gameDeck->returnToDeck(airliftCard);
                if (!surpressOutput)
                    output << "[Aggressive] " << player->getPlayerName() << " uses an Airlift card to support attacks.\n";
            }
        }

        // === STEP 4: Redistribute armies from strong to weak (defensive reallocation) ===
        vector<Territory*> strongSources = player->getSourcesWithManyArmies();
        unordered_map<Territory*, Territory*> defendMap = toDefend(player);

        for (auto& [src, dest] : defendMap) {
            if (!src || !dest) continue;

            int srcArmies = src->getNumArmies();
            if (srcArmies <= 1) continue;

            int transfer = max(1, srcArmies / 2);
            player->getPlayerOrders()->addOrder(
                new WarzoneOrder::Advance(player, src, dest, transfer)
            );

            if (!surpressOutput)
                output << "[Benevolent] " << player->getPlayerName()
                       << " reinforces " << dest->getID()
                       << " from " << src->getID()
                       << " with " << transfer << " armies.\n";
        }

        output << "[Benevolent] " << player->getPlayerName()
               << " completed all defensive reinforcement orders.\n";

        return output.str();
    }

    unordered_map<Territory*, Territory*> BenevolentPlayerStrategy::toAttack(Player* player) {
        // Implementation for BenevolentPlayerStrategy's toAttack
        return unordered_map<Territory*, Territory*>();
    }

    string BenevolentPlayerStrategy::toAttackString(Player* player) {
        if (!player) return "[Benevolent] Invalid player.\n";
        return "[Benevolent] " + player->getPlayerName() +
               " will not attack — focuses solely on defense.\n";
    }

    unordered_map<Territory*, Territory*> BenevolentPlayerStrategy::toDefend(Player* player) {
        unordered_map<Territory*, Territory*> defendMap;
        if (!player) return defendMap;

        // Use BFS-style logic like Player::computeDefendMap
        auto baseMap = player->computeDefendMap();
        for (auto& [t, towardEnemy] : baseMap)
            defendMap[t] = towardEnemy;

        return defendMap;
    }

    string BenevolentPlayerStrategy::toDefendString(Player* player) {
        if (!player) return "[Benevolent] Invalid player.\n";

        stringstream ss;
        ss << "[Benevolent] " << player->getPlayerName()
           << " strengthens weaker territories and maintains tight defenses.\n";

        auto defendMap = player->computeDefendMap();
        for (auto& [from, to] : defendMap) {
            if (from && to)
                ss << "  " << from->getID() << " → " << to->getID() << " (defensive path)\n";
        }

        return ss.str();
    }

    PlayerStrategy* BenevolentPlayerStrategy::clone() const {
        return new BenevolentPlayerStrategy();
    }

    // ======================================================== //
    // ================ NeutralPlayerStrategy ================= //
    // ======================================================== //

    NeutralPlayerStrategy::NeutralPlayerStrategy() {
        this->strategyType = PlayerStrategyType::NEUTRAL;
    }

    string NeutralPlayerStrategy::issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) {
        if (!player)
            return "[Neutral] Error: Player not initialized.\n";

        // Neutral player takes no action.
        if (!surpressOutput) {
            output << "[Neutral] " << player->getPlayerName()
                   << " remains passive and issues no orders this turn.\n";
        }

        return output.str();
    }

    unordered_map<Territory*, Territory*> NeutralPlayerStrategy::toAttack(Player* player) {
        // Implementation for NeutralPlayerStrategy's toAttack
        return unordered_map<Territory*, Territory*>();
    }

    string NeutralPlayerStrategy::toAttackString(Player* player) {
        if (!player) return "[Neutral] Invalid player.\n";
        return "[Neutral] " + player->getPlayerName() +
               " does not attack — remains peaceful.\n";
    }

    unordered_map<Territory*, Territory*> NeutralPlayerStrategy::toDefend(Player* player) {
        // Implementation for NeutralPlayerStrategy's toDefend
        return unordered_map<Territory*, Territory*>();
    }

    string NeutralPlayerStrategy::toDefendString(Player* player) {
        if (!player) return "[Neutral] Invalid player.\n";
        return "[Neutral] " + player->getPlayerName() +
                " does not prepare any defenses — stays neutral.\n";
    }

    PlayerStrategy* NeutralPlayerStrategy::clone() const {
        return new NeutralPlayerStrategy();
    }

    // ========================================================= //
    // ================= CheaterPlayerStrategy ================= //
    // ========================================================= //

    CheaterPlayerStrategy::CheaterPlayerStrategy() {
        this->strategyType = PlayerStrategyType::CHEATER;
    }

    string CheaterPlayerStrategy::issueOrder(Player* player, ostringstream& output, bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) {
        if (!player)
            return "[Cheater] Error: Player not initialized.\n";

        if (!surpressOutput) {
            output << "[Cheater] " << player->getPlayerName()
                   << " is cheating — instantly conquering adjacent enemy territories!\n";
        }

        // Step 1: Collect all enemy territories adjacent to any owned territory
        vector<Territory*> owned = player->getOwnedTerritories().getTerritories();
        unordered_set<Territory*> toConquer;

        for (Territory* ownedTerr : owned) {
            if (!ownedTerr) continue;

            for (Territory* neighbor : ownedTerr->getNeighbors()) {
                if (!neighbor) continue;

                Player* neighOwner = neighbor->getOwner();
                if (neighOwner && neighOwner != player) {
                    toConquer.insert(neighbor);
                }
            }
        }

        // Step 2: Instantly conquer those territories (change ownership)
        for (Territory* t : toConquer) {
            Player* oldOwner = t->getOwner();
            if (!t || !oldOwner || oldOwner == player) continue;

            oldOwner->removeOwnedTerritories(t);
            player->addOwnedTerritories(t);

            if (!surpressOutput) {
                output << "[Cheater] " << player->getPlayerName()
                       << " automatically conquers " << t->getID()
                       << " from " << oldOwner->getPlayerName() << "!\n";
            }
        }

        output << "[Cheater] " << player->getPlayerName()
               << " has completed all cheating conquests for this turn.\n";

        return output.str();
    }

    unordered_map<Territory*, Territory*> CheaterPlayerStrategy::toAttack(Player* player) {
        unordered_map<Territory*, Territory*> attackMap;

        if (!player) return attackMap;

        vector<Territory*> owned = player->getOwnedTerritories().getTerritories();
        for (Territory* ownedTerr : owned) {
            if (!ownedTerr) continue;

            for (Territory* neighbor : ownedTerr->getNeighbors()) {
                if (!neighbor) continue;
                Player* neighOwner = neighbor->getOwner();
                if (neighOwner && neighOwner != player) {
                    attackMap[ownedTerr] = neighbor;
                    break; // only need one attack target per territory
                }
            }
        }

        return attackMap;
    }

    string CheaterPlayerStrategy::toAttackString(Player* player) {
        if (!player) return "[Cheater] Invalid player.\n";

        stringstream ss;
        ss << "[Cheater] " << player->getPlayerName()
           << " will automatically conquer all adjacent enemy territories.\n";

        auto attackMap = toAttack(player);
        for (auto& [from, target] : attackMap)
            if (from && target)
                ss << "  " << from->getID() << " → " << target->getID() << "\n";

        return ss.str();
    }

    unordered_map<Territory*, Territory*> CheaterPlayerStrategy::toDefend(Player* player) {
        // Implementation for CheaterPlayerStrategy's toDefend
        return unordered_map<Territory*, Territory*>();
    }

    string CheaterPlayerStrategy::toDefendString(Player* player) {
        if (!player) return "[Cheater] Invalid player.\n";
        return "[Cheater] " + player->getPlayerName() +
               " does not defend — they simply take everything around them!\n";
    }

    PlayerStrategy* CheaterPlayerStrategy::clone() const {
        return new CheaterPlayerStrategy();
    }
}