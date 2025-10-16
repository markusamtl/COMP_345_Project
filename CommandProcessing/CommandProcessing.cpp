#include "CommandProcessing.h"

namespace WarzoneCommand {

    //====================================== COMMAND CLASS ===========================================//

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Command::Command() {

        this -> commandName = "";
        this -> commandArgs = {};
        this -> effect = "";

    }

    Command::Command(string inputString) {

        inputString = StringHandling::trim(inputString);
        vector<string> tempStrVec = StringHandling::split(inputString, ' ');

        if(!tempStrVec.empty()){

            this -> commandName = StringHandling::toLower(StringHandling::trim(tempStrVec.front()));

            tempStrVec.erase(tempStrVec.begin()); //Remove command name
            this -> commandArgs.swap(tempStrVec);

        } else {

            //If empty, ensure clean defaults
            this -> commandName = "";
            this -> commandArgs.clear();

        }

        this -> effect = "";

    }

    Command::Command(const string& name, const vector<string>& args) {

        this -> commandName = StringHandling::toLower(StringHandling::trim(name));
        this -> commandArgs = args;
        this -> effect = "";

    }

    Command::Command(const Command& other) {

        this -> commandName = other.commandName;
        this -> commandArgs = other.commandArgs;
        this -> effect = other.effect;

    }

    Command::~Command(){} //No dynamically allocated memory; nothing to release

    Command& Command::operator=(const Command& other) {

        if(this != &other){

            this -> commandName = other.commandName;
            this -> commandArgs = other.commandArgs;
            this -> effect = other.effect;

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Command& c) {

        os << "[Command] Name: " << (c.commandName.empty() ? "None" : c.commandName)
        << " | Args: ";

        if(c.commandArgs.empty()){
            
            os << "None"; 

        } else {

            for(size_t i = 0; i < c.commandArgs.size(); ++i){
                
                os << c.commandArgs[i];
                if(i < c.commandArgs.size() - 1) os << ", ";
            
            }

        }

        os << " | Effect: " << (c.effect.empty() ? "None" : c.effect);
        return os;

    }

    //------------------------------ Accessors & Mutators ------------------------------//

    string Command::getCommandName() const { return commandName; }
    void Command::setCommandName(const string& name){ this -> commandName = name; }

    vector<string> Command::getCommandArgs() const { return commandArgs; }
    void Command::setCommandArgs(const vector<string>& args){ this -> commandArgs = args; }

    string Command::getEffect() const { return effect; }
    void Command::setEffect(const string& e){ this -> effect = e; }

    //------------------------------- Public Methods ------------------------------------//

    string Command::processInput(string inputString) {

        ostringstream os; //Collect string output
        vector<string> splitString = StringHandling::split(StringHandling::trim(inputString), ' '); //Tokenize string input

        //Check if input is empty
        if(splitString.empty()){ return "Error: Current command is empty."; }

        //Extract command name (normalize ONLY this string)
        string tempCmdName = StringHandling::toLower(StringHandling::trim(splitString.front()));
        splitString.erase(splitString.begin()); //Remove command name from arguments vector

        //Check if the command name is valid
        if(!isCommandNameValid(tempCmdName)){

            os << "Error: Command name '" << tempCmdName << "' is NOT a valid command name.";
            return os.str();

        }

        //Validate argument count based on command name
        if(!validate(tempCmdName, splitString)){

            os << "Error: Invalid arguments for command '" << tempCmdName << "'. ";

            if(splitString.empty()){

                os << "(No arguments provided)";

            } else {

                os << "Provided arguments: ";

                for(size_t i = 0; i < splitString.size(); i++){

                    os << splitString[i];
                    if((i + 1) != splitString.size()){ os << " "; }
                
                }

            }

            return os.str();

        }

        //If we reach here, the command is valid
        os << "The following string IS a valid command: " << tempCmdName;

        if(!splitString.empty()){

            os << " ";

            for(size_t i = 0; i < splitString.size(); i++){

                os << splitString[i];
                if(i < splitString.size() - 1){ os << " "; }

            }

        }

        os << endl;

        //Set command to have the valid fields
        commandName = tempCmdName;
        commandArgs = splitString;

        return os.str();

    }

    string Command::toString() const {

        stringstream ss;
        
        ss << commandName; //Rebuild commandName

        if(!commandArgs.empty()){ //Rebuild commandArgs

            ss << " ";

            for(size_t i = 0; i < commandArgs.size(); i++){

                ss << commandArgs[i];
                if(i < commandArgs.size() - 1) ss << " ";
            
            }

        }

        return ss.str();

    }

    bool Command::validate() const {

        long long commandNameHash = StringHandling::hashStringToNum(commandName);

        switch(commandNameHash){

            //Commands that require 1 argument
            case(LOADMAP_COMMAND_HASH):
            case(ADDPLAYER_COMMAND_HASH):

                return commandArgs.size() == 1;

            //Commands that require no arguments
            case(VALIDATEMAP_COMMAND_HASH):
            case(GAMESTART_COMMAND_HASH):
            case(REPLAY_COMMAND_HASH):
            case(QUIT_COMMAND_HASH):

                return true;

            //Any other faulty commands
            default:

                return false;


        }
        

    }

    bool Command::validate(const string& inputCommandName, const vector<string>& inputCommandArgs) {

        long long commandNameHash = StringHandling::hashStringToNum(inputCommandName);

        switch(commandNameHash){

            //Commands that require 1 argument
            case(LOADMAP_COMMAND_HASH):
            case(ADDPLAYER_COMMAND_HASH):

                return inputCommandArgs.size() == 1;

            //Commands that require no arguments
            case(VALIDATEMAP_COMMAND_HASH):
            case(GAMESTART_COMMAND_HASH):
            case(REPLAY_COMMAND_HASH):
            case(QUIT_COMMAND_HASH):

                return true;

            //Any other faulty commands
            default:

                return false;


        }
        
    }

    bool Command::isCommandNameValid(const string& inputStr) const {

        long long commandNameHash = StringHandling::hashStringToNum(inputStr);

        switch(commandNameHash){

            //Precalculated command name Hashes
            case(LOADMAP_COMMAND_HASH):
            case(ADDPLAYER_COMMAND_HASH):
            case(VALIDATEMAP_COMMAND_HASH):
            case(GAMESTART_COMMAND_HASH):
            case(REPLAY_COMMAND_HASH):
            case(QUIT_COMMAND_HASH):

                return true;

            //Any other commands
            default:

                return false;

        }

    }

    /*------------------------------------------ COMMAND PROCESSOR CLASS --------------------------------------------------*/

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    CommandProcessor::CommandProcessor(GameEngine* engine) {

        this -> engine = engine;
        this -> commandList = {};

    }

    CommandProcessor::~CommandProcessor() {

        //Don't delete engine!

        for(Command* c : commandList){ delete c; }

        commandList.clear();

    }

    CommandProcessor::CommandProcessor(const CommandProcessor& other) {

        this -> engine = other.engine;
        for(Command* c : other.commandList){

            if(c != nullptr){ commandList.push_back(new Command(*c)); }

        }

    }

    CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {

        if(this != &other){

            for(Command* c : commandList){

                delete c;

            }

            commandList.clear();
            engine = other.engine;

            for(Command* c : other.commandList){

                if(c != nullptr){ commandList.push_back(new Command(*c)); }

            }

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const CommandProcessor& cp) {

        os << "[CommandProcessor] Stored Commands (" << cp.commandList.size() << "):\n";

        for(Command* c : cp.commandList){

            if(c != nullptr){ os << "  " << c -> toString() << endl; }

        }

        return os;

    }

    //------------------------------ Accessors & Mutators ------------------------------//

    vector<Command*> CommandProcessor::getCommandList() const { return commandList; }

    GameEngine* CommandProcessor::getEngine() const { return engine; }

    //------------------------------- Protected Methods ------------------------------------//

    string CommandProcessor::readCommandFromSource() {

        string input;
        cout << "Enter command: ";

        if(!getline(cin, input)) {
            
            if(cin.eof()) { cout << "\n[CommandProcessor] End-of-input detected (Ctrl+Z pressed). Exiting command loop.\n"; } 
            else { cerr << "\n[CommandProcessor] Input error encountered.\n"; }

            cin.clear(); //Clear read-in buffer
            return "__EOF__"; //Sentinel value to quit loops

        }

        return input;
        
    }

    Command* CommandProcessor::readCommand(){

        string rawInput = readCommandFromSource();

        Command* newCmd = new Command(rawInput);
        string parseResult = newCmd -> processInput(rawInput);

        cout << parseResult << endl;
        newCmd -> setEffect(parseResult);

        saveCommand(newCmd);
        return newCmd;

    }

    void CommandProcessor::saveCommand(Command* command){

        if(command == nullptr){ return; }

        commandList.push_back(command);

        //Effect string will later be used in Part 5 for logging
        cout << "[CommandProcessor] Command saved: " << command -> toString() << endl;

    }

    void CommandProcessor::executeGame(){

        //---------------------------- Setup Phase ----------------------------
        GameEngine* engine = this -> engine; 

        cout << "=============================================\n";
        cout << "   Simulating Game From CommandProcessor     \n";
        cout << "=============================================\n\n";
        //---------------------------- Initialization ----------------------------

        bool running = true;
        ostringstream sessionLog; //Accumulate all console I/O before gameplayPhase

        //---------------------------- Command Loop ----------------------------
        while(running){

            //Show valid commands for current state
            EngineState state = engine -> getState();

            cout << "\n[CommandProcessor] Current State: " << engine -> getStateAsString() << endl;
            sessionLog << "\n[CommandProcessor] Current State: " << engine -> getStateAsString() << endl;

            cout << "[CommandProcessor] Available commands: ";
            sessionLog << "[CommandProcessor] Available commands: ";

            switch(state){

                case EngineState::Start:

                    cout << "loadmap <mapfile>\n";
                    sessionLog << "loadmap <mapfile>\n";
                    break;

                case EngineState::MapLoaded:

                    cout << "validatemap\n";
                    sessionLog << "validatemap\n";
                    break;

                case EngineState::MapValidated:

                    cout << "addplayer <playername>\n";
                    sessionLog << "addplayer <playername>\n";
                    break;

                case EngineState::PlayersAdded:

                    cout << "addplayer <playername>  OR  gamestart\n";
                    sessionLog << "addplayer <playername>  OR  gamestart\n";
                    break;

                case EngineState::Win:

                    cout << "replay  OR  quit\n";
                    sessionLog << "replay  OR  quit\n";
                    break;

                case EngineState::AssignReinforcement:
                case EngineState::IssueOrders:
                case EngineState::ExecuteOrders:

                    cout << "Game running...\n";
                    sessionLog << "Game running...\n";
                    break;

                default:

                    cout << "Invalid State!\n";
                    sessionLog << "Invalid State!\n";
                    break;

            }

            cout << "[CommandProcessor] Please enter your command: " << endl;
            sessionLog << "[CommandProcessor] Please enter your command:\n";

            //Get command from processor
            Command* cmd = this -> getCommand();

            //Break gameloop gracefully
            if (cmd -> getCommandName() == "__EOF__") {

                cout << "[CommandProcessor] Exiting simulation (EOF reached).\n";
                break;

            }

            else if(cmd == nullptr){ continue; }
            else{ sessionLog << cmd -> toString(); }

            if(cmd == nullptr){ continue; }
            else{ sessionLog << cmd -> toString(); }

            string cmdName = cmd -> getCommandName();
            vector<string> args = cmd -> getCommandArgs();
            long long cmdHash = StringHandling::hashStringToNum(cmdName);
            ostringstream output;

            //Validate command against current GameEngine state
            if(!this -> validate(cmd)){

                string effect = "[CommandProcessor] Invalid command '" + cmd -> toString() + "' in state: " + engine -> getStateAsString() + "\n";
                cmd -> setEffect(effect);
                cout << effect;
                sessionLog << effect;
                continue;

            }

            //----------------------------- Command Execution -------------------------//
            switch(cmdHash){

                case LOADMAP_COMMAND_HASH: {

                    if(args.empty()){

                        output << "[FileCommandProcessorAdapter] Error: Missing map file argument.\n";
                        break;

                    }

                    string result = engine -> engineLoadMap(args[0], true);
                    output << result;

                    if(result.find("failed") != string::npos || result.find("error") != string::npos){

                        output << "\n[FileCommandProcessorAdapter] Map loading failed. Returning to Start state.\n";
                        engine -> setState(EngineState::Start);

                    } else {

                        engine -> setState(EngineState::MapLoaded);

                    }

                    break;

                }

                case VALIDATEMAP_COMMAND_HASH: {

                    string result = engine -> engineValidateMap(true);
                    output << result << endl;

                    if(engine -> getState() == EngineState::Start){

                        output << "[FileCommandProcessorAdapter] Map validation failed. Returning to Start state.\n";
                        
                    }

                    break;

                }

                case ADDPLAYER_COMMAND_HASH: {

                    if(args.empty()){

                        output << "[FileCommandProcessorAdapter] Error: Missing player name argument.\n";
                        break;

                    }

                    string result = engine -> engineAddPlayer(args[0], true);
                    output << result << endl;
                    break;

                }

                case GAMESTART_COMMAND_HASH: {

                    if(engine -> getPlayers().size() < 2) {

                        output << "[FileCommandProcessorAdapter] Error: You need at LEAST 2 players to play a game.\n" << endl;
                        break;

                    }

                    string result = engine -> engineGameStart(true);
                    output << result << endl;

                    //Pass session log + file path to gameplayPhase
                    engine -> gameplayPhase(true, sessionLog.str(), "../CommandProcessing/Games/");

                    //When game is played, clear stringbuilder buffer
                    sessionLog.str("");
                    sessionLog.clear();
                    break;

                }

                case REPLAY_COMMAND_HASH: {

                    output << "[FileCommandProcessorAdapter] Restarting game session.\n" << endl;

                    //When game is played, clear stringbuilder buffer
                    sessionLog.str("");
                    sessionLog.clear();

                    break;

                }

                case QUIT_COMMAND_HASH: {

                    output << "[FileCommandProcessorAdapter] Exiting game. Goodbye!\n" << endl;
                    engine -> engineEnd();
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
            cmd -> setEffect(effectText);

        }

        //---------------------------- Cleanup ----------------------------
        cout << "[CommandProcessor] Simulation complete. Gameplay logs handled by GameEngine.\n";

    }

    //------------------------------- Public Methods ------------------------------------//

    Command* CommandProcessor::getCommand(){

        Command* command = readCommand();
        if(command == nullptr){ return nullptr; }

        bool valid = validate(command);

        if(valid){

            command -> setEffect("[CommandProcessor] Command is valid for current state: " + engine -> getStateAsString());

        } else {

            command -> setEffect("[CommandProcessor] Command invalid for state: " + engine -> getStateAsString());

        }

        return command;

    }

    bool CommandProcessor::validate(Command* command){

        if(command == nullptr || engine == nullptr){ return false; }

        string cmdName = command -> getCommandName();
        EngineState state = engine -> getState();

        long long hash = StringHandling::hashStringToNum(cmdName);

        switch(hash){

            case(LOADMAP_COMMAND_HASH):

                return (state == EngineState::Start || state == EngineState::MapLoaded);

            case(VALIDATEMAP_COMMAND_HASH):
            
                return (state == EngineState::MapLoaded);

            case(ADDPLAYER_COMMAND_HASH):

                return (state == EngineState::MapValidated || state == EngineState::PlayersAdded);

            case(GAMESTART_COMMAND_HASH):

                return (state == EngineState::PlayersAdded);

            case(REPLAY_COMMAND_HASH):

                return (state == EngineState::Win);

            case(QUIT_COMMAND_HASH):

                return (state == EngineState::Win);

            default:

                return false;

        }

    }

    void CommandProcessor::runGame(){

        executeGame();

    }

    /*------------------------------------------ FILE COMMAND PROCESSOR ADAPTER --------------------------------------------------*/

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    FileCommandProcessorAdapter::FileCommandProcessorAdapter(GameEngine* engine, const string& filename):CommandProcessor(engine) {

        //Safety: ensure any previous stream state is reset
        if(fileStream.is_open()){ fileStream.close(); }

        fileStream.clear(); //Reset flags (EOF, failbit, etc.)

        //Attempt to open file 
        fileStream.open(filename);

        if(!fileStream.is_open()){ cerr << "[FileCommandProcessorAdapter] Error: Could not open file '" << filename << "'." << endl; } 
        else { cout << "[FileCommandProcessorAdapter] File opened successfully: " << filename << endl; }

    }

    FileCommandProcessorAdapter::~FileCommandProcessorAdapter(){

        if(fileStream.is_open()){ fileStream.close(); }
        fileStream.clear();

    }

    FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other) : CommandProcessor(other) {

        if(other.fileStream.is_open()){

            //Cannot directly copy stream position, reopen with same filename if possible
            cerr << "[FileCommandProcessorAdapter] Warning: Copying adapter will not duplicate stream position.\n";

        }

    }

    FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other){

        if(this != &other){

            CommandProcessor::operator=(other);

            if(fileStream.is_open()){ fileStream.close(); }

            if(other.fileStream.is_open()){

                cerr << "[FileCommandProcessorAdapter] Warning: Assignment operator does not clone stream position.\n";

            }

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& fcp){

        os << "[FileCommandProcessorAdapter] Adapter currently ";

        if(fcp.fileStream.is_open()){

            os << "has an open file stream.\n";

        } else {

            os << "does not have an open file stream.\n";

        }

        os << static_cast<const CommandProcessor&>(fcp); //Output base class data
        return os;

    }

    //------------------------------- Protected Methods ------------------------------------//

    string FileCommandProcessorAdapter::readCommandFromSource(){

        string line;

        if(fileStream.is_open() && getline(fileStream, line)){

            cout << "[FileCommandProcessorAdapter] Read from file: " << line << endl;
            return line;

        } else {

            cout << "[FileCommandProcessorAdapter] End of file reached or failed to read line." << endl;
            return "";

        }

    }

    void FileCommandProcessorAdapter::executeGame(){

        //---------------------------- Setup Phase ----------------------------
        GameEngine* engine = this -> getEngine(); 

        if(engine == nullptr){

            cerr << "[FileCommandProcessorAdapter] Error: GameEngine reference is null.\n";
            return;

        }

        if(!fileStream.is_open()){

            cerr << "[FileCommandProcessorAdapter] Error: No valid input file is open.\n";
            return;

        }

        cout << "\n=============================================\n";
        cout << "   Simulating Game From FileCommandProcessor  \n";
        cout << "=============================================\n";

        //---------------------------- Initialization ----------------------------
        bool running = true;
        ostringstream sessionLog; //Accumulate all file-based I/O before gameplayPhase
        string rawInput;

        //---------------------------- File Command Loop ----------------------------
        while(running && getline(fileStream, rawInput)){

            //Skip empty lines
            if(StringHandling::trim(rawInput).empty()){ continue; }

            cout << "\n[FileCommandProcessorAdapter] Current State: " << engine -> getStateAsString() << endl;
            sessionLog << "\n[FileCommandProcessorAdapter] Current State: " << engine -> getStateAsString() << endl;

            cout << "[FileCommandProcessorAdapter] Reading Command: " << rawInput << endl;
            sessionLog << "[FileCommandProcessorAdapter] Reading Command: " << rawInput << endl;

            //Build command object
            Command* cmd = new Command(rawInput);
            string parseResult = cmd -> processInput(rawInput);
            cmd -> setEffect(parseResult);
            saveCommand(cmd);

            string cmdName = cmd -> getCommandName();
            vector<string> args = cmd -> getCommandArgs();
            long long cmdHash = StringHandling::hashStringToNum(cmdName);
            ostringstream output;

            //Validate command against current GameEngine state
            if(!this -> validate(cmd)){

                string effect = "[FileCommandProcessorAdapter] Invalid command '" + cmd -> toString() 
                                + "' in state: " + engine -> getStateAsString() + "\n";
                cmd -> setEffect(effect);
                cout << effect;
                sessionLog << effect;
                continue;

            }

            //----------------------------- Command Execution -------------------------//
            switch(cmdHash){

                case LOADMAP_COMMAND_HASH: {

                    if(args.empty()){

                        output << "[FileCommandProcessorAdapter] Error: Missing map file argument.\n";
                        break;

                    }

                    string result = engine -> engineLoadMap(args[0], true);
                    output << result;

                    if(result.find("failed") != string::npos || result.find("error") != string::npos){

                        output << "\n[FileCommandProcessorAdapter] Map loading failed. Returning to Start state.\n";
                        engine -> setState(EngineState::Start);

                    } else {

                        engine -> setState(EngineState::MapLoaded);

                    }

                    break;

                }

                case VALIDATEMAP_COMMAND_HASH: {

                    string result = engine -> engineValidateMap(true);
                    output << result << endl;

                    if(engine -> getState() == EngineState::Start){

                        output << "[FileCommandProcessorAdapter] Map validation failed. Returning to Start state.\n";
                        
                    }

                    break;

                }

                case ADDPLAYER_COMMAND_HASH: {

                    if(args.empty()){

                        output << "[FileCommandProcessorAdapter] Error: Missing player name argument.\n";
                        break;

                    }

                    string result = engine -> engineAddPlayer(args[0], true);
                    output << result << endl;
                    break;

                }

                case GAMESTART_COMMAND_HASH: {

                    if(engine -> getPlayers().size() < 2) {

                        output << "[FileCommandProcessorAdapter] Error: You need at LEAST 2 players to play a game.\n" << endl;
                        break;

                    }

                    string result = engine -> engineGameStart(true);
                    output << result << endl;

                    //Pass session log + file path to gameplayPhase
                    engine -> gameplayPhase(true, sessionLog.str(), "../CommandProcessing/Games/");

                    //When game is played, clear stringbuilder buffer
                    sessionLog.str("");
                    sessionLog.clear();
                    break;

                }

                case REPLAY_COMMAND_HASH: {

                    output << "[FileCommandProcessorAdapter] Restarting game session.\n" << endl;

                    //When game is played, clear stringbuilder buffer
                    sessionLog.str("");
                    sessionLog.clear();

                    break;

                }

                case QUIT_COMMAND_HASH: {

                    output << "[FileCommandProcessorAdapter] Exiting game. Goodbye!\n" << endl;
                    engine -> engineEnd();
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
            cmd -> setEffect(effectText);

        }

        //---------------------------- EOF Safety Check ----------------------------//
        if(engine -> getState() != EngineState::End){

            cout << "\n[FileCommandProcessorAdapter] Warning: Command file ended before reaching END state.\n";
            cout << "[FileCommandProcessorAdapter] Current state: " << engine -> getStateAsString() << endl;
            cout << "[FileCommandProcessorAdapter] Terminating simulation and cleaning up...\n";

            //Gracefully reset engine and free memory
            engine -> clearGame();       //Frees map, deck, and all players
            engine -> setState(EngineState::End);

            //Close file stream if still open
            if(fileStream.is_open()){ fileStream.close(); }

            cout << "[FileCommandProcessorAdapter] Game terminated prematurely. All resources freed.\n";
            return;

        }

        //---------------------------- Cleanup ----------------------------//
        cout << "[FileCommandProcessorAdapter] Simulation complete. Gameplay logs handled by GameEngine.\n";

    }

    //------------------------------- Public Methods ------------------------------------//

    void FileCommandProcessorAdapter::runGame() {

        executeGame();

    }

}