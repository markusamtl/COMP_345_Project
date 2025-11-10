#include "CommandProcessingDriver.h"
#include "../LoggingObserver/LoggingObserver.h"

using namespace WarzoneCommand;
using namespace WarzoneEngine;
using namespace WarzoneLog;
namespace fs = std::filesystem;

/*==============================================================*/
/*------------------- CommandProcessor CLI Test ----------------*/
/*==============================================================*/

void testCommandProcessorCLI(){

	cout << "=============================================\n";
	cout << "     TEST: CommandProcessor (CLI Input)      \n";
	cout << "=============================================\n\n";

	//--------------------------- Setup ---------------------------//
	GameEngine* engine = new GameEngine();
	engine->setMaxTurns(500000);

	CommandProcessor* processor = new CommandProcessor(engine);
	LogObserver* logger = new LogObserver();

	engine->attach(logger);
	processor->attach(logger);

	cout << "[Driver] LogObserver attached to GameEngine and CommandProcessor.\n";
	cout << "[Driver] Log file will be written to ../GameLogs/.\n\n";

	//--------------------------- Run Game ------------------------//
	processor->runGame();

	//--------------------------- Cleanup -------------------------//
	engine->detach(logger);
	processor->detach(logger);

	delete processor;
	delete engine;
	delete logger;

	cout << "=============================================\n";
	cout << "      CLI CommandProcessor Test Complete      \n";
	cout << "=============================================\n\n";
}


/*==============================================================*/
/*------------- FileCommandProcessorAdapter Test ----------------*/
/*==============================================================*/

void testFileCommandProcessorAdapter(const vector<string>& fileList){

	cout << "=============================================\n";
	cout << "  TEST: FileCommandProcessorAdapter Execution \n";
	cout << "=============================================\n\n";

	if(fileList.empty()){
		cerr << "[Driver] Error: No command files provided.\n";
		return;
	}

	//-------------------- Iterate Through Files ------------------//
	for(const string& fileName : fileList){

		cout << "---------------------------------------------------\n";
		cout << "[Driver] Starting new simulation using file: " << fileName << endl;
		cout << "---------------------------------------------------\n";

		//--------------------------- Setup -------------------------//
		GameEngine* engine = new GameEngine();
		engine->setMaxTurns(500000);

		FileCommandProcessorAdapter* adapter = new FileCommandProcessorAdapter(engine, fileName);
		if(!adapter){
			cerr << "[Driver] Error: Failed to initialize FileCommandProcessorAdapter for file: " << fileName << endl;
			delete engine;
			continue;
		}

		LogObserver* logger = new LogObserver();
		engine->attach(logger);
		adapter->attach(logger);

		cout << "[Driver] LogObserver attached to GameEngine and FileCommandProcessorAdapter.\n";
		cout << "[Driver] Logging session for: " << fileName << "\n\n";

		//--------------------------- Run Game ----------------------//
		adapter -> runGame();

		//--------------------------- Cleanup -----------------------//
		engine->detach(logger);
		adapter->detach(logger);

		delete adapter;
		delete engine;
		delete logger;

		cout << "[Driver] Completed simulation for file: " << fileName << "\n\n";
	}

	cout << "=============================================\n";
	cout << "     All FileCommandProcessor Tests Done      \n";
	cout << "=============================================\n\n";
}


/*==============================================================*/
/*---------- FileCommandProcessorAdapter Driver Setup -----------*/
/*==============================================================*/

void testFileCommandProcessorAdapterDriver(){

	cout << "=============================================\n";
	cout << "      TEST: FileCommandProcessorAdapter      \n";
	cout << "=============================================\n\n";

	const string basePath = "../CommandProcessing/FileCommandProcessorAdapterInputs/";

	//---------------------- Directory Setup ----------------------//
	if(!fs::exists(basePath)){
		cout << "[Driver] Directory not found: " << basePath << endl;
		cout << "[Driver] Creating directory...\n";

		try{
			fs::create_directories(basePath);
			cout << "[Driver] Directory created successfully.\n\n";
		}
		catch(const fs::filesystem_error& e){
			cerr << "[Driver] Error: Failed to create directory: " << e.what() << endl;
			return;
		}
	}

	else{ cout << "[Driver] Directory already exists: " << basePath << "\n\n"; }

	//---------------------- File Definitions ---------------------//
	vector<string> fileNames = {
		"test1.txt","test2.txt","test3.txt","test4.txt",
		"test5.txt","test6.txt","test7.txt","test8.txt"
	};

	vector<vector<string>> fileContents;

	// (1) Normal game flow - Brazil map, 6 players, quit
	fileContents.push_back({
		"loadmap ../Map/test_maps/Brazil/Brazil.map",
		"validatemap",
		"addplayer Alice","addplayer Bob","addplayer Carl",
		"addplayer Dan","addplayer Erica","addplayer Francine",
		"gamestart","quit"
	});

	// (2) Replay game sequence
	fileContents.push_back({
		"loadmap ../Map/test_maps/Brazil/Brazil.map",
		"validatemap",
		"addplayer Alice","addplayer Bob","addplayer Carl",
		"addplayer Dan","addplayer Erica","addplayer Francine",
		"gamestart","replay",
		"loadmap ../Map/test_maps/Ohio/Ohio.map",
		"validatemap",
		"addplayer Alice","addplayer Bob","addplayer Carl",
		"addplayer Dan","addplayer Erica","addplayer Francine",
		"gamestart","quit"
	});

	// (3) Multiple map load attempts and too many players
	vector<string> cmds = {
		"loadmap ../Map/test_maps/Montreal/Montreal.map",
		"loadmap ../Map/test_maps/005_I72_V-22/005_I72_V-22.map",
		"validatemap",
		"loadmap ../Map/test_maps/jksdhiufhre/jksdhiufhre.map",
		"loadmap ../Map/test_maps/Brazil/Brazil.map",
		"validatemap"
	};
	for(int i = 1; i <= 67; i++){ cmds.push_back("addplayer Player" + to_string(i)); }
	cmds.push_back("gamestart");
	cmds.push_back("quit");
	fileContents.push_back(cmds);

	// (4) Empty file
	fileContents.push_back({""});

	// (5) Only loadmap
	fileContents.push_back({"loadmap ../Map/test_maps/Brazil/Brazil.map"});

	// (6) Ends after validatemap
	fileContents.push_back({
		"loadmap ../Map/test_maps/Brazil/Brazil.map","validatemap"
	});

	// (7) Ends after addplayer
	fileContents.push_back({
		"loadmap ../Map/test_maps/Brazil/Brazil.map","validatemap","addplayer Alice"
	});

	// (8) Ends after gamestart
	fileContents.push_back({
		"loadmap ../Map/test_maps/Brazil/Brazil.map","validatemap",
		"addplayer Alice","addplayer Bob","gamestart"
	});

	//---------------------- Write Test Files ----------------------//
	for(size_t i = 0; i < fileNames.size(); i++){
		string fullPath = basePath + fileNames[i];
		ofstream out(fullPath);

		if(!out.is_open()){
			cerr << "[Driver] Error: Could not create file: " << fullPath << endl;
			continue;
		}

		cout << "[Driver] Building file: " << fullPath << endl;
		for(const string& line : fileContents[i]) out << line << "\n";
		out.close();

		cout << "[Driver] Successfully wrote " << fileContents[i].size()
			 << " lines to file: " << fileNames[i] << "\n\n";
	}

	//---------------------- Execute Tests ------------------------//
	vector<string> fullPaths;
	fullPaths.reserve(fileNames.size());
	for(const string& f : fileNames){
		fullPaths.push_back(basePath + f);
	}

	testFileCommandProcessorAdapter(fullPaths);
}


/*==============================================================*/
/*--------------------- Unified Test Runner --------------------*/
/*==============================================================*/

void testCommandProcessor(){
	testCommandProcessorCLI();
	testFileCommandProcessorAdapterDriver();
}
