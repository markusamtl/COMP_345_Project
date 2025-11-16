#include "CommandProcessing.h"

namespace WarzoneCommand {

    //====================================== COMMAND CLASS ===========================================//

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Command::Command() {
        commandName = "";
        commandArgs = {};
        effect = "";
    }

    Command::Command(string inputString) {
        inputString = StringHandling::trim(inputString);

        // Special sentinel to represent EOF from CommandProcessor
        if (inputString == "__EOF__") {
            commandName = "__EOF__";
            commandArgs.clear();
            effect = "[Command] EOF marker received.";
            return;
        }

        vector<string> tempStrVec = StringHandling::split(inputString, ' ');

        if (!tempStrVec.empty()) {
            commandName = StringHandling::toLower(StringHandling::trim(tempStrVec.front()));
            tempStrVec.erase(tempStrVec.begin()); // Remove command name
            commandArgs.swap(tempStrVec);
        } else {
            // If empty, ensure clean defaults
            commandName = "";
            commandArgs.clear();
        }

        effect = "";
    }

    Command::Command(const string& name, const vector<string>& args) {
        commandName = StringHandling::toLower(StringHandling::trim(name));
        commandArgs = args;
        effect = "";
    }

    Command::Command(const Command& other) {
        commandName = other.commandName;
        commandArgs = other.commandArgs;
        effect = other.effect;
    }

    Command::~Command() {} // No dynamically allocated memory

    Command& Command::operator=(const Command& other) {
        if (this != &other) {
            commandName = other.commandName;
            commandArgs = other.commandArgs;
            effect = other.effect;
        }
        return *this;
    }

    ostream& operator<<(ostream& os, const Command& c) {
        os << "[Command] Name: " << (c.commandName.empty() ? "None" : c.commandName)
           << " | Args: ";

        if (c.commandArgs.empty()) {
            os << "None";
        } else {
            for (size_t i = 0; i < c.commandArgs.size(); ++i) {
                os << c.commandArgs[i];
                if (i < c.commandArgs.size() - 1) os << ", ";
            }
        }

        os << " | Effect: " << (c.effect.empty() ? "None" : c.effect);
        return os;
    }

    //------------------------------ Accessors & Mutators ------------------------------//

    string Command::getCommandName() const { return commandName; }
    void Command::setCommandName(const string& name) { commandName = name; }

    vector<string> Command::getCommandArgs() const { return commandArgs; }
    void Command::setCommandArgs(const vector<string>& args) { commandArgs = args; }

    string Command::getEffect() const { return effect; }
    void Command::setEffect(const string& e){ this -> effect = e; }

    //------------------------------- Public Methods ------------------------------------//

    string Command::processInput(string inputString) {
        ostringstream os;

        inputString = StringHandling::trim(inputString);

        // Handle EOF sentinel explicitly
        if (inputString == "__EOF__") {
            commandName = "__EOF__";
            commandArgs.clear();
            return "[Command] EOF marker received.";
        }

        vector<string> splitString = StringHandling::split(inputString, ' ');

        // Check if input is empty
        if (splitString.empty()) {
            return "Error: Current command is empty.";
        }

        // Extract command name
        string tempCmdName = StringHandling::toLower(StringHandling::trim(splitString.front()));
        splitString.erase(splitString.begin()); // Remove command name from arguments vector

        // Check if the command name is valid
        if (!isCommandNameValid(tempCmdName)) {
            os << "Error: Command name '" << tempCmdName << "' is NOT a valid command name.";
            return os.str();
        }

        // Validate argument count based on command name
        if (!validate(tempCmdName, splitString)) {
            os << "Error: Invalid arguments for command '" << tempCmdName << "'. ";

            if (splitString.empty()) {
                os << "(No arguments provided)";
            } else {
                os << "Provided arguments: ";
                for (size_t i = 0; i < splitString.size(); i++) {
                    os << splitString[i];
                    if ((i + 1) != splitString.size()) os << " ";
                }
            }
            return os.str();
        }

        // If we reach here, the command is valid
        os << "The following string IS a valid command: " << tempCmdName;

        if (!splitString.empty()) {
            os << " ";
            for (size_t i = 0; i < splitString.size(); i++) {
                os << splitString[i];
                if (i < splitString.size() - 1) os << " ";
            }
        }

        os << endl;

        // Set command fields
        commandName = tempCmdName;
        commandArgs = splitString;

        return os.str();
    }

    string Command::toString() const {
        stringstream ss;
        ss << commandName;

        if (!commandArgs.empty()) {
            ss << " ";
            for (size_t i = 0; i < commandArgs.size(); i++) {
                ss << commandArgs[i];
                if (i < commandArgs.size() - 1) ss << " ";
            }
        }

        return ss.str();
    }

    bool Command::validate() const {
        long long commandNameHash = StringHandling::hashStringToNum(commandName);

        switch (commandNameHash) {
            // Commands that require 1 argument
            case LOADMAP_COMMAND_HASH:
            case ADDPLAYER_COMMAND_HASH:
                return commandArgs.size() == 1;

            // Commands that require no arguments
            case VALIDATEMAP_COMMAND_HASH:
            case GAMESTART_COMMAND_HASH:
            case REPLAY_COMMAND_HASH:
            case QUIT_COMMAND_HASH:
                return true;

            default:
                return false;
        }
    }

    bool Command::validate(const string& inputCommandName, const vector<string>& inputCommandArgs) {
        long long commandNameHash = StringHandling::hashStringToNum(inputCommandName);

        switch (commandNameHash) {
            // Commands that require 1 argument
            case LOADMAP_COMMAND_HASH:
            case ADDPLAYER_COMMAND_HASH:
                return inputCommandArgs.size() == 1;

            // Commands that require no arguments
            case VALIDATEMAP_COMMAND_HASH:
            case GAMESTART_COMMAND_HASH:
            case REPLAY_COMMAND_HASH:
            case QUIT_COMMAND_HASH:
                return true;

            default:
                return false;
        }
    }

    bool Command::isCommandNameValid(const string& inputStr) const {
        long long commandNameHash = StringHandling::hashStringToNum(inputStr);

        switch (commandNameHash) {
            case LOADMAP_COMMAND_HASH:
            case ADDPLAYER_COMMAND_HASH:
            case VALIDATEMAP_COMMAND_HASH:
            case GAMESTART_COMMAND_HASH:
            case REPLAY_COMMAND_HASH:
            case QUIT_COMMAND_HASH:
                return true;
            default:
                return false;
        }
    }

    string Command::stringToLog() {
        return "Command's Effect: " + effect;
    }

    /*------------------------------------------ COMMAND PROCESSOR CLASS --------------------------------------------------*/

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    CommandProcessor::CommandProcessor(GameEngine* engine) {
        this->engine = engine;
        commandList = {};
    }

    CommandProcessor::~CommandProcessor() {
        // Don't delete engine!
        for (Command* c : commandList) {
            delete c;
        }
        commandList.clear();
    }

    CommandProcessor::CommandProcessor(const CommandProcessor& other) {
        engine = other.engine;
        for (Command* c : other.commandList) {
            if (c != nullptr) commandList.push_back(new Command(*c));
        }
    }

    CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
        if (this != &other) {
            for (Command* c : commandList) {
                delete c;
            }
            commandList.clear();

            engine = other.engine;

            for (Command* c : other.commandList) {
                if (c != nullptr) commandList.push_back(new Command(*c));
            }
        }

        return *this;
    }

    ostream& operator<<(ostream& os, const CommandProcessor& cp) {
        os << "[CommandProcessor] Stored Commands (" << cp.commandList.size() << "):\n";
        for (Command* c : cp.commandList) {
            if (c != nullptr) os << "  " << c->toString() << endl;
        }
        return os;
    }

    //------------------------------ Accessors & Mutators ------------------------------//

    vector<Command*> CommandProcessor::getCommandList() const { return commandList; }

    GameEngine* CommandProcessor::getEngine() const { return engine; }

    //------------------------------- Protected Methods ------------------------------------//

    string CommandProcessor::readCommandFromSource(){

        string input;

        if(cin.peek() == '\n'){ cin.get(); } // consume just one newline


        if(!getline(cin, input)){

            if(cin.eof()){ cout << "\n[CommandProcessor] End-of-input detected (EOF). Exiting command loop.\n"; }
            else{ cerr << "\n[CommandProcessor] Input error encountered.\n"; }
            
            cin.clear();
            return "__EOF__"; // sentinel
        
        }

        input = WarzoneMap::StringHandling::trim(input);
        return input;
    
    }

    Command* CommandProcessor::readCommand(){

        string rawInput = readCommandFromSource();

        if(rawInput == "__EOF__"){
            Command* eofCmd = new Command("__EOF__");
            eofCmd->setEffect("[CommandProcessor] EOF command generated.");
            saveCommand(eofCmd);
            return eofCmd;
        }

        if(WarzoneMap::StringHandling::trim(rawInput).empty()){
            cout << "[CommandProcessor] Empty command ignored.\n";
            return nullptr;
        }

        Command* newCmd = new Command(rawInput);
        string parseResult = newCmd->processInput(rawInput);
        cout << parseResult << endl;
        newCmd->setEffect(parseResult);
        saveCommand(newCmd);
        return newCmd;

    }

    void CommandProcessor::saveCommand(Command* command) {
        if (command == nullptr) return;

        commandList.push_back(command);
        cout << "[CommandProcessor] Command saved: " << command->toString() << endl;

        //Effect string will later be used in Part 5 for logging
        cout << "[CommandProcessor] Command saved: " << command -> toString() << endl;

    }

    void CommandProcessor::executeGame(){

        //---------------------------- Setup Phase ----------------------------//
        GameEngine* engine = this->engine;

        cout << "=============================================\n";
        cout << "   Simulating Game From CommandProcessor     \n";
        cout << "=============================================\n\n";

        // -------------------- Ask for Output Suppression -------------------- //
        bool suppressOutput = false;
        char choice;

        while(true){
            cout << "[CommandProcessor] Would you like to suppress console output? (y/n): ";
            if(!(cin >> choice)){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            choice = static_cast<char>(tolower(choice));

            if(choice == 'y'){
                suppressOutput = true;
                cout << "[CommandProcessor] Console output suppression enabled.\n";
                break;
            } 
            else if(choice == 'n'){
                suppressOutput = false;
                cout << "[CommandProcessor] Console output suppression disabled.\n";
                break;
            } 
            else{
                cout << "Invalid choice. Please enter 'y' or 'n'.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        bool running = true;
        ostringstream sessionLog;

        //---------------------------- Command Loop ----------------------------//
        while(running){

            EngineState state = engine->getState();

            if(!suppressOutput){ 
                cout << "\n[CommandProcessor] Current State: " << engine->getStateAsString() << endl;
                cout << "[CommandProcessor] Available commands: ";
            }

            sessionLog << "\n[CommandProcessor] Current State: " << engine->getStateAsString() << endl;
            sessionLog << "[CommandProcessor] Available commands: ";

            switch(state){
                case EngineState::Start:
                    if(!suppressOutput){ cout << "loadmap <mapfile (../Map/test_maps/MAP_NAME_FOLDER/MAP_NAME.map)>\n"; }
                    sessionLog << "loadmap <mapfile>\n";
                    break;

                case EngineState::MapLoaded:
                    if(!suppressOutput){ cout << "validatemap\n"; }
                    sessionLog << "validatemap\n";
                    break;

                case EngineState::MapValidated:
                    if(!suppressOutput){ cout << "addplayer <playername>\n"; }
                    sessionLog << "addplayer <playername>\n";
                    break;

                case EngineState::PlayersAdded:
                    if(!suppressOutput){ cout << "addplayer <playername>  OR  gamestart\n"; }
                    sessionLog << "addplayer <playername>  OR  gamestart\n";
                    break;

                case EngineState::Win:
                    if(!suppressOutput){ cout << "replay  OR  quit\n"; }
                    sessionLog << "replay  OR  quit\n";
                    break;

                case EngineState::AssignReinforcement:
                case EngineState::IssueOrders:
                case EngineState::ExecuteOrders:
                    if(!suppressOutput){ cout << "Game running...\n"; }
                    sessionLog << "Game running...\n";
                    break;

                default:
                    if(!suppressOutput){ cout << "Invalid State!\n"; }
                    sessionLog << "Invalid State!\n";
                    break;
            }

            if(!suppressOutput){ cout << "[CommandProcessor] Please enter your command: "; }
            sessionLog << "[CommandProcessor] Please enter your command:\n";

            Command* cmd = this->getCommand();

            if(cmd && cmd->getCommandName() == "__EOF__"){
                if(!suppressOutput){ cout << "[CommandProcessor] Exiting simulation (EOF reached).\n"; }
                break;
            }

            if(!cmd){ continue; }

            sessionLog << cmd->toString();
            string cmdName = cmd->getCommandName();
            vector<string> args = cmd->getCommandArgs();
            long long cmdHash = StringHandling::hashStringToNum(cmdName);
            ostringstream output;

            if(!this->validate(cmd)){
                string effect = "[CommandProcessor] Invalid command '" + cmd->toString() +
                                "' in state: " + engine->getStateAsString() + "\n";
                cmd->setEffect(effect);
                if(!suppressOutput){ cout << effect; }
                sessionLog << effect;
                continue;
            }

            //----------------------------- Command Execution -------------------------//
            switch(cmdHash){

                case LOADMAP_COMMAND_HASH:{
                    if(args.empty()){
                        output << "[CommandProcessor] Error: Missing map file argument.\n";
                        break;
                    }

                    string result = engine->engineLoadMap(args[0], suppressOutput);
                    output << result;

                    if(result.find("failed") != string::npos || result.find("Error") != string::npos){
                        output << "\n[CommandProcessor] Map loading failed. Returning to Start state.\n";
                        engine->setState(EngineState::Start);
                    }
                    else{ engine->setState(EngineState::MapLoaded); }
                    break;
                }

                case VALIDATEMAP_COMMAND_HASH:{
                    string result = engine->engineValidateMap(suppressOutput);
                    output << result << endl;

                    if(engine->getState() == EngineState::Start){
                        output << "[CommandProcessor] Map validation failed. Returning to Start state.\n";
                    }
                    break;
                }

                case ADDPLAYER_COMMAND_HASH:{
                    if(args.empty()){
                        output << "[CommandProcessor] Error: Missing player name argument.\n";
                        break;
                    }

                    string result = engine->engineAddPlayer(args[0], suppressOutput);
                    output << result << endl;
                    break;
                }

                case GAMESTART_COMMAND_HASH:{
                   
                    if(engine->getPlayers().size() < 2){
                        output << "[CommandProcessor] Error: You need at LEAST 2 players to play a game.\n" << endl;
                        break;
                    }

                    string result = engine->engineGameStart(suppressOutput);
                    output << result << endl;

                    engine->gameplayPhase(suppressOutput);

                    sessionLog.str("");
                    sessionLog.clear();
                    break;
                }

                case REPLAY_COMMAND_HASH:{
                    
                    output << "[CommandProcessor] Restarting game session.\n" << endl;
                    engine -> clearGame(); 
                    engine -> setState(EngineState::Start);
                    sessionLog.str("");
                    sessionLog.clear();
                    break;
                
                }

                case QUIT_COMMAND_HASH:{
                    output << "[CommandProcessor] Exiting game. Goodbye!\n" << endl;
                    engine->engineEnd();
                    running = false;
                    break;
                }

                default:{

                    output << "[CommandProcessor] Unknown or unsupported command: " << cmdName << "\n" << endl;
                    break;
                
                }

            }

            //---------------------- Log & Display ----------------------//
            string effectText = output.str();
            if(!suppressOutput){ cout << effectText; }
            sessionLog << effectText;
            cmd -> setEffect(effectText);

        }

        //---------------------------- Cleanup ----------------------------//
        if(!suppressOutput){ cout << "[CommandProcessor] Simulation complete. Gameplay logs handled by GameEngine.\n"; }
    }


    //------------------------------- Public Methods ------------------------------------//

    Command* CommandProcessor::getCommand() {
        Command* command = readCommand();
        if (command == nullptr) {
            return nullptr;
        }

        bool validSyntax = command->validate();

        if (validSyntax) {
            command->setEffect("[CommandProcessor] Command is syntactically valid.");
        } else {
            command->setEffect("[CommandProcessor] Command has invalid syntax or argument count.");
        }

        return command;
    }

    bool CommandProcessor::validate(Command* command) {
        if (command == nullptr || engine == nullptr) return false;

        string cmdName = command->getCommandName();
        EngineState state = engine->getState();

        long long hash = StringHandling::hashStringToNum(cmdName);

        switch (hash) {
            case LOADMAP_COMMAND_HASH:
                return (state == EngineState::Start || state == EngineState::MapLoaded);

            case VALIDATEMAP_COMMAND_HASH:
                return (state == EngineState::MapLoaded);

            case ADDPLAYER_COMMAND_HASH:
                return (state == EngineState::MapValidated || state == EngineState::PlayersAdded);

            case GAMESTART_COMMAND_HASH:
                return (state == EngineState::PlayersAdded);

            case REPLAY_COMMAND_HASH:
            case QUIT_COMMAND_HASH:
                return (state == EngineState::Win);

            default:
                return false;
        }
    }

    void CommandProcessor::runGame() {
        executeGame();
    }

    string CommandProcessor::stringToLog() {
        if (commandList.empty()) return "Command: <none>";
        Command* lastCommand = commandList.back();
        return "Command: " + lastCommand->toString();
    }

    /*------------------------------------------ FILE COMMAND PROCESSOR ADAPTER --------------------------------------------------*/

    FileCommandProcessorAdapter::FileCommandProcessorAdapter(GameEngine* engine, const string& filename)
        : CommandProcessor(engine) {

        if (fileStream.is_open()) fileStream.close();
        fileStream.clear();
        fileStream.open(filename);

        if (!fileStream.is_open()) {
            cerr << "[FileCommandProcessorAdapter] Error: Could not open file '" << filename << "'." << endl;
        } else {
            cout << "[FileCommandProcessorAdapter] File opened successfully: " << filename << endl;
        }
    }

    FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
        if (fileStream.is_open()) fileStream.close();
        fileStream.clear();
    }

    FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other)
        : CommandProcessor(other) {

        if (other.fileStream.is_open()) {
            cerr << "[FileCommandProcessorAdapter] Warning: Copying adapter will not duplicate stream position.\n";
        }
    }

    FileCommandProcessorAdapter&
    FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
        if (this != &other) {
            CommandProcessor::operator=(other);

            if (fileStream.is_open()) fileStream.close();

            if (other.fileStream.is_open()) {
                cerr << "[FileCommandProcessorAdapter] Warning: Assignment operator does not clone stream position.\n";
            }
        }
        return *this;
    }

    ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& fcp) {
        os << "[FileCommandProcessorAdapter] Adapter currently ";
        if (fcp.fileStream.is_open()) os << "has an open file stream.\n";
        else os << "does not have an open file stream.\n";

        os << static_cast<const CommandProcessor&>(fcp);
        return os;
    }

    //------------------------------- Protected Methods ------------------------------------//

    string FileCommandProcessorAdapter::readCommandFromSource() {
        string line;

        if (fileStream.is_open() && getline(fileStream, line)) {
            cout << "[FileCommandProcessorAdapter] Read from file: " << line << endl;
            return line;
        } else {
            cout << "[FileCommandProcessorAdapter] End of file reached or failed to read line." << endl;
            // Return EOF sentinel so upper layers can stop if they reuse this hook
            return "__EOF__";
        }
    }

    void FileCommandProcessorAdapter::executeGame() {
        //---------------------------- Setup Phase ----------------------------
        GameEngine* engine = getEngine();

        if (engine == nullptr) {
            cerr << "[FileCommandProcessorAdapter] Error: GameEngine reference is null.\n";
            return;
        }

        if (!fileStream.is_open()) {
            cerr << "[FileCommandProcessorAdapter] Error: No valid input file is open.\n";
            return;
        }

        cout << "\n=============================================\n";
        cout << "   Simulating Game From FileCommandProcessor  \n";
        cout << "=============================================\n";

        bool running = true;
        ostringstream sessionLog;
        string rawInput;

        //---------------------------- File Command Loop ----------------------------
        while (running && getline(fileStream, rawInput)) {
            // Skip empty lines
            if (StringHandling::trim(rawInput).empty()) continue;

            cout << "\n[FileCommandProcessorAdapter] Current State: " << engine->getStateAsString() << endl;
            sessionLog << "\n[FileCommandProcessorAdapter] Current State: " << engine->getStateAsString() << endl;

            cout << "[FileCommandProcessorAdapter] Reading Command: " << rawInput << endl;
            sessionLog << "[FileCommandProcessorAdapter] Reading Command: " << rawInput << endl;

            // Build command object
            Command* cmd = new Command(rawInput);
            string parseResult = cmd->processInput(rawInput);
            cmd->setEffect(parseResult);
            saveCommand(cmd);

            string cmdName = cmd->getCommandName();
            vector<string> args = cmd->getCommandArgs();
            long long cmdHash = StringHandling::hashStringToNum(cmdName);
            ostringstream output;

            // Validate command against current GameEngine state
            if (!this->validate(cmd)) {
                string effect = "[FileCommandProcessorAdapter] Invalid command '" + cmd->toString()
                                + "' in state: " + engine->getStateAsString() + "\n";
                cmd->setEffect(effect);
                cout << effect;
                sessionLog << effect;
                continue;
            }

            //----------------------------- Command Execution -------------------------//
            switch (cmdHash) {
                case LOADMAP_COMMAND_HASH: {
                    if (args.empty()) {
                        output << "[FileCommandProcessorAdapter] Error: Missing map file argument.\n";
                        break;
                    }

                    string result = engine->engineLoadMap(args[0], true);
                    output << result;

                    if (result.find("failed") != string::npos || result.find("Error") != string::npos) {
                        output << "\n[FileCommandProcessorAdapter] Map loading failed. Returning to Start state.\n";
                        engine->setState(EngineState::Start);
                    } else {
                        engine->setState(EngineState::MapLoaded);
                    }

                    break;
                }

                case VALIDATEMAP_COMMAND_HASH: {
                    string result = engine->engineValidateMap(true);
                    output << result << endl;

                    if (engine->getState() == EngineState::Start) {
                        output << "[FileCommandProcessorAdapter] Map validation failed. Returning to Start state.\n";
                    }
                    break;
                }

                case ADDPLAYER_COMMAND_HASH: {
                    if (args.empty()) {
                        output << "[FileCommandProcessorAdapter] Error: Missing player name argument.\n";
                        break;
                    }

                    string result = engine->engineAddPlayer(args[0], true);
                    output << result << endl;
                    break;
                }

                case GAMESTART_COMMAND_HASH: {
                    if (engine->getPlayers().size() < 2) {
                        output << "[FileCommandProcessorAdapter] Error: You need at LEAST 2 players to play a game.\n" << endl;
                        break;
                    }

                    string result = engine->engineGameStart(true);
                    output << result << endl;

                    // Run full game
                    engine->gameplayPhase(true);

                    // Clear accumulated log after a full game
                    sessionLog.str("");
                    sessionLog.clear();
                    break;
                }

                case REPLAY_COMMAND_HASH: {
                    output << "[FileCommandProcessorAdapter] Restarting game session.\n" << endl;
                    sessionLog.str("");
                    sessionLog.clear();
                    break;
                }

                case QUIT_COMMAND_HASH: {
                    output << "[FileCommandProcessorAdapter] Exiting game. Goodbye!\n" << endl;
                    engine->engineEnd();
                    running = false;
                    break;
                }

                default: {
                    output << "[FileCommandProcessorAdapter] Unknown or unsupported command: " << cmdName << "\n" << endl;
                    break;
                }
            }

            //---------------------- Log & Display ----------------------//
            string effectText = output.str();
            cout << effectText;
            sessionLog << effectText;
            cmd->setEffect(effectText);
        }

        // Optional info if file ends before proper END
        if (engine->getState() != EngineState::End && engine->getState() != EngineState::Win) {
            cout << "\n[FileCommandProcessorAdapter] Warning: Command file ended before reaching END or WIN state.\n";
            cout << "[FileCommandProcessorAdapter] Current state: " << engine->getStateAsString() << endl;
            cout << "[FileCommandProcessorAdapter] Terminating simulation...\n";
            engine->engineEnd();
        }

        cout << "[FileCommandProcessorAdapter] Simulation complete. Gameplay logs handled by GameEngine.\n";
    }

    //------------------------------- Public Methods ------------------------------------//

    void FileCommandProcessorAdapter::runGame() {
        executeGame();
    }

}
