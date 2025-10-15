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

    //------------------------------- Class Methods ------------------------------------//

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

    //-- Accessors & Mutators --//

    vector<Command*> CommandProcessor::getCommandList() const { return commandList; }

    GameEngine* CommandProcessor::getEngine() const { return engine; }

    //-- Core Private Methods --//

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

    //-- Protected Method (Adapter Hook) --//

    string CommandProcessor::readCommandFromSource(){

        string input;
        cout << "Enter command: ";
        getline(cin, input);
        return input;

    }

    //------- Public Methods ---------//

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

    /*------------------------------------------ FILE COMMAND PROCESSOR ADAPTER --------------------------------------------------*/

    FileCommandProcessorAdapter::FileCommandProcessorAdapter(GameEngine* engine, const string& filename)
    : CommandProcessor(engine) {

        fileStream.open(filename);

        if(!fileStream.is_open()){

            cerr << "[FileCommandProcessorAdapter] Error: Could not open file '" << filename << "'." << endl;

        }

    }

    FileCommandProcessorAdapter::~FileCommandProcessorAdapter(){

        if(fileStream.is_open()){ fileStream.close(); }

    }

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

}