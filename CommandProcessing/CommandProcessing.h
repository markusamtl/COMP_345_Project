#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../GameEngine/GameEngine.h"

using namespace std;
using namespace WarzoneEngine;

namespace WarzoneCommand {

    //--- Values of running lowercase command name through hashStringToNum()
    const long long LOADMAP_COMMAND_HASH = 5709642908645ll;
    const long long VALIDATEMAP_COMMAND_HASH = 10383033028812ll;
    const long long ADDPLAYER_COMMAND_HASH = 17777300026399ll;
    const long long GAMESTART_COMMAND_HASH = 16405556169060ll;
    const long long REPLAY_COMMAND_HASH = 3935729223788ll;
    const long long QUIT_COMMAND_HASH = 21035786512688ll;

    using WarzoneEngine::EngineState;
    using WarzoneMap::StringHandling;
    /*------------------------------------------ COMMAND CLASS --------------------------------------------------*/

    /**
     * @class Command
     * @brief Represents a single user or file-issued command.
     * 
     * Each Command object stores:
     * - The command keyword (e.g., "loadmap")
     * - Any optional argument (e.g., "Brazil.map")
     * - The resulting effect or validation message after processing
     * 
     * Commands are validated against the current GameEngine state
     * and stored by the CommandProcessor.
     */
    class Command {

        private:

            string commandName;
            vector<string> commandArgs; 
            string effect;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Default constructor.
             */
            Command();

            /**
             * @brief Construct a new Command object, using a string object for construction
             * 
             * @param inputString input string
             */
            Command(string inputString);

            /**
             * @brief Parameterized constructor.
             * 
             * @param name The command keyword (e.g., "loadmap", "addplayer").
             * @param arg The arguments associated with the command (e.g., "Brazil.map", "Alice").
             */
            Command(const string& name, const vector<string>& args);

            /**
             * @brief Destructor.
             */
            ~Command();

            /**
             * @brief Copy constructor.
             * 
             * @param other The Command object to copy from.
             */
            Command(const Command& other);

            /**
             * @brief Assignment operator.
             * 
             * @param other The Command object to assign from.
             * @return Reference to this Command instance after assignment.
             */
            Command& operator=(const Command& other);

            /**
             * @brief Stream insertion operator.
             * 
             * @param os The output stream reference.
             * @param c The Command object to print.
             * 
             * @return Reference to the same output stream for chaining.
             */
            friend ostream& operator<<(ostream& os, const Command& c);

            //-- Accessors & Mutators --//

            /**
             * @brief Accessor for commandName
             * @return String for commandName
             */
            string getCommandName() const;

            /**
             * @brief Mutator for commandName
             * @param commandName string to set 
             */
            void setCommandName(const string& commandName);

            /**
             * @brief Accessor for commandArgs
             * @return String vector for commandArgs
             */
            vector<string> getCommandArgs() const;

            /**
             * @brief Mutator for commandArgs
             * @param commandArgs string to set 
             */
            void setCommandArgs(const vector<string>& commandArgs);

            /**
             * @brief Accessor for effect
             * @return String for effect
             */
            string getEffect() const;

            /**
             * @brief Mutator for effect
             * @param effect string to set 
             */
            void setEffect(const string& effect);

            //---------- CLASS METHODS ------------//

            /**
             * @brief Parse an input string into a command-readable format.
             * 
             * @param inputString 
             * @return String detailing method output
             */
            string processInput(string inputString);

            /**
             * @brief Gets string representation of a command
             * 
             * @return String representation of a command 
             */
            string toString() const;

            /**
             * @brief Get the Engine State object, based on the name of a command
             * 
             * @return EngineState 
             */
            EngineState getEngineState();

            /**
             * @brief Ensure that a command object is valid
             * @return boolean value to see whether a command is valid or not
             */
            bool validate() const;

            /**
             * @brief Method to see if ANY inputted command name and command args forms a valid command
             * @param commandName 
             * @param commandArgs 
             * @return Boolean value, whether current inputted name and args follows the set rules detailed in A2.  
             */
            static bool validate(const string& inputCommandName, const vector<string>& inputCommandArgs);

            /**
             * @brief Checks to see if a command name's is a valid name
             * @param commandName Name of command being checked
             * @return Boolean to see if a command's name is valid. 
             */
            bool isCommandNameValid(const string& commandName) const;

    };

     /**
     * @class CommandProcessor
     * @brief Responsible for reading, validating, and storing commands from either console or adapter.
     * 
     * The CommandProcessor class implements the core logic to:
     * - Read user commands (console input)
     * - Validate them against current GameEngine states
     * - Save them in an internal list of Command objects
     * - Provide them to other game modules (e.g., GameEngine)
     * 
     * It also provides a hook method (readCommandFromSource) that can be
     * overridden by adapter subclasses to support different input sources (e.g., file input).
     */
    class CommandProcessor {

        private:

            //-- Class Attributes --//
            vector<Command*> commandList;
            GameEngine* engine;

            /**
             * @brief Reads a command string directly from console input and stores it.
             * @return Pointer to the created Command object.
             */
            Command* readCommand();

            /**
             * @brief Saves a validated Command object into commandList.
             * @param command Pointer to Command to store.
             */
            void saveCommand(Command* command);

        protected:

            /**
             * @brief Hook method for reading commands.
             * 
             * - Default implementation: reads from console.
             * - FileCommandProcessorAdapter overrides this to read from a file.
             * 
             * @return Raw input string from the input source.
             */
            virtual string readCommandFromSource();

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Default constructor
             * @param engine Pointer to the active GameEngine instance.
             */
            CommandProcessor(GameEngine* engine);

            /**
             * @brief Destructor
             */
            virtual ~CommandProcessor();

            /**
             * @brief Copy constructor
             * @param other CommandProcessor to copy from
             */
            CommandProcessor(const CommandProcessor& other);

            /**
             * @brief Assignment operator
             * @param other CommandProcessor to assign from
             * @return Reference to this CommandProcessor
             */
            CommandProcessor& operator=(const CommandProcessor& other);

            /**
             * @brief Stream insertion operator
             * @param os Output stream
             * @param cp CommandProcessor to output
             * @return Reference to the output stream
             */
            friend ostream& operator<<(ostream& os, const CommandProcessor& cp);

            //---------- Accessors & Mutators --------------//

            /**
             * @brief Accessor for the list of stored commands
             * @return Vector of Command pointers
             */
            vector<Command*> getCommandList() const;

            /**
             * @brief Accessor for the GameEngine
             * @return Pointer to GameEngine
             */
            GameEngine* getEngine() const;

            //------------- Class Methods --------------//

            /**
             * @brief Reads and returns the next valid Command object.
             * @return Pointer to the Command that was read and validated.
             */
            Command* getCommand();

            /**
             * @brief Validates whether a command is valid in the current GameEngine state.
             * @param command Pointer to the Command to validate.
             * @return True if valid, false otherwise.
             */
            bool validate(Command* command);

    };

    /*------------------------------------------ FILE COMMAND PROCESSOR ADAPTER CLASS --------------------------------------------------*/

    /**
     * @class FileCommandProcessorAdapter
     * @brief Adapter that enables CommandProcessor to read commands from a file instead of console.
     * 
     * This class implements the Adapter design pattern. It inherits from CommandProcessor,
     * overriding readCommandFromSource() to read commands sequentially from a file stream.
     */
    class FileCommandProcessorAdapter : public CommandProcessor {

        private:

            ifstream fileStream;

        protected:

            /**
             * @brief Overridden method to read commands from a text file instead of console.
             * @return Next line of command input from the file.
             */
            string readCommandFromSource() override;

        public:

            /**
             * @brief Parameterized constructor.
             * @param engine Pointer to the active GameEngine instance.
             * @param filename Name of the input command file.
             */
            FileCommandProcessorAdapter(GameEngine* engine, const string& filename);

            /**
             * @brief Destructor
             */
            ~FileCommandProcessorAdapter() override;

    };

} 