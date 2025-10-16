#include "CommandProcessingDriver.h"

void testCommandProcessorCLI() {

    GameEngine* engine = new GameEngine();
    engine -> setMaxTurns(500000);
    CommandProcessor* processor = new CommandProcessor(engine);

    processor -> runGame();

    delete processor;
    delete engine;

}

void testFileCommandProcessorAdapter(const vector<string>& fileList){

    cout << "=============================================\n";
    cout << "  TEST: FileCommandProcessorAdapter Execution \n";
    cout << "=============================================\n\n";

    if(fileList.empty()){

        cerr << "[Driver] Error: No command files provided.\n";
        return;

    }

    //---------------------------- File Processing Loop ----------------------------//
    for(const string& fileName : fileList){

        cout << "---------------------------------------------------\n";
        cout << "[Driver] Starting new simulation using file: " << fileName << endl;
        cout << "---------------------------------------------------\n";

        //---------------------------- Setup Phase ----------------------------//
        GameEngine* engine = new GameEngine();
        engine -> setMaxTurns(500000);
        FileCommandProcessorAdapter* adapter = new FileCommandProcessorAdapter(engine, fileName);

        //---------------------------- Validation ----------------------------//
        if(!adapter){

            cerr << "[Driver] Error: Failed to initialize FileCommandProcessorAdapter for file: " << fileName << endl;
            delete engine;
            continue;

        }

        //---------------------------- Execution ----------------------------//
        adapter -> runGame();

        //---------------------------- Cleanup ----------------------------//
        delete adapter;
        delete engine;

        cout << "[Driver] Completed simulation for file: " << fileName << "\n\n";

    }

    cout << "=============================================\n";
    cout << "     All FileCommandProcessor Tests Done      \n";
    cout << "=============================================\n\n";

}

/**
 * @brief Driver function to create test files for the CommandProcessor and FileCommandProcessorAdapter.
 * 
 * This function automatically generates 8 test input files in the directory:
 * "../CommandProcessor/FileCommandProcessorAdapterInputs/"
 * 
 * Each file simulates a different game startup or edge case scenario for the
 * Warzone Command Processor (as required by A2 - Part 1 testing specifications).
 * 
 * All map files are assumed to exist under "../Map/test_maps/".
 */
void testFileCommandProcessorAdapterDriver(){

    cout << "=============================================\n";
    cout << "      TEST: FileCommandProcessorAdapter      \n";
    cout << "=============================================\n\n\n";

    //---------------------------- Setup Phase ----------------------------//
    const string basePath = "../CommandProcessing/FileCommandProcessorAdapterInputs/";

    if(!fs::exists(basePath)){

        cout << "[Driver] Directory not found: " << basePath << endl;
        cout << "[Driver] Creating directory...\n";

        try{

            fs::create_directories(basePath);
            cout << "[Driver] Directory created successfully.\n\n";

        }catch(const fs::filesystem_error& e){

            cerr << "[Driver] Error: Failed to create directory: " << e.what() << endl;
            return;

        }

    } else {

        cout << "[Driver] Directory already exists: " << basePath << "\n\n";

    }

    vector<string> fileNames = {
        "test1.txt",
        "test2.txt",
        "test3.txt",
        "test4.txt",
        "test5.txt",
        "test6.txt",
        "test7.txt",
        "test8.txt",
    };

    //---------------------------- File Contents ----------------------------//
    cout << "=============================================\n";
    cout << "       Setting Up files to be read from      \n";
    cout << "=============================================\n\n";

    vector<vector<string>> fileContents;

    //(1) Working game with Brazil map, 6 players, quit after start
    fileContents.push_back({
        "loadmap ../Map/test_maps/Brazil/Brazil.map",
        "validatemap",
        "addplayer Alice",
        "addplayer Bob",
        "addplayer Carl",
        "addplayer Dan",
        "addplayer Erica",
        "addplayer Francine",
        "gamestart",
        "quit"
    });

    //(2) Replay game instead of quitting, call 'ohio'
    fileContents.push_back({
        "loadmap ../Map/test_maps/Brazil/Brazil.map",
        "validatemap",
        "addplayer Alice",
        "addplayer Bob",
        "addplayer Carl",
        "addplayer Dan",
        "addplayer Erica",
        "addplayer Francine",
        "gamestart",
        "replay",
        "loadmap ../Map/test_maps/Ohio/Ohio.map",
        "validatemap",
        "addplayer Alice",
        "addplayer Bob",
        "addplayer Carl",
        "addplayer Dan",
        "addplayer Erica",
        "addplayer Francine",
        "gamestart",
        "quit"
    });

    //(3) Montreal fails to be loaded since it's not written properly, then tries to load giberish, then Brazil loads but adds 67 players, plays until end, quits
    vector<string> cmds = {
        
        "loadmap ../Map/test_maps/Montreal/Montreal.map",
        "loadmap ../Map/test_maps/005_I72_V-22/005_I72_V-22.map",
        "validatemap",
        "loadmap ../Map/test_maps/jksdhiufhre/jksdhiufhre.map",
        "loadmap ../Map/test_maps/Brazil/Brazil.map",
        "validatemap"
    
    };

    // Add 67 players (Brazil has 66 territories)
    for(int i = 1; i <= 67; i++){

        cmds.push_back("addplayer Player" + to_string(i));

    }

    cmds.push_back("gamestart");
    cmds.push_back("quit");

    fileContents.push_back(cmds);
    

    //(5) NO TEXT
    fileContents.push_back({""});

    //(5) Only loadmap (empty scenario after map load)
    fileContents.push_back({ "loadmap ../Map/test_maps/Brazil/Brazil.map" });

    //(6) Ends after validatemap
    fileContents.push_back({
        "loadmap ../Map/test_maps/Brazil/Brazil.map",
        "validatemap"
    });

    // (7) Ends after addplayer
    fileContents.push_back({
        "loadmap ../Map/test_maps/Brazil/Brazil.map",
        "validatemap",
        "addplayer Alice"
    });

    //(8) Ends after gamestart
    fileContents.push_back({
        "loadmap ../Map/test_maps/Brazil/Brazil.map",
        "validatemap",
        "addplayer Alice",
        "addplayer Bob",
        "gamestart"
    });

    //---------------------------- File Creation Loop ----------------------------//
    for(size_t i = 0; i < fileNames.size(); i++){

        string fullPath = basePath + fileNames[i];
        ofstream out(fullPath);

        if(!out.is_open()){

            cerr << "[Driver] Error: Could not create file: " << fullPath << endl;
            continue;

        }

        cout << "[Driver] Building file: " << fullPath << endl;

        for(const string& line : fileContents[i]){

            out << line << "\n";

        }

        out.close();
        cout << "[Driver] Successfully wrote " << fileContents[i].size() 
             << " lines to file: " << fileNames[i] << "\n\n";

    }

    cout << "=============================================\n";
    cout << "    END: FileCommandProcessorAdapter Test    \n";
    cout << "=============================================\n\n";

    vector<string> newFileNames;
    newFileNames.reserve(fileNames.size());
    ostringstream os;

    for(string file : fileNames){ 

        os << basePath << file;
        newFileNames.push_back(os.str());

        //Reset buffer
        os.str("");
        os.clear();

    }

    testFileCommandProcessorAdapter(newFileNames);

}

void testCommandProcessor(){

    testCommandProcessorCLI();
    //testFileCommandProcessorAdapterDriver();

}