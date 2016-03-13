#pragma once

#include <ProgressManager.h>
#include <FileUtils.h>
#include <DateUtils.h>
#include <Log.h>
#include <PD_Scenario.h>
#include <PD_UI_DissBattle.h>
#include <PD_DissStats.h>
#include <Player.h>
#include <ctime>
#include <StringUtils.h>

ProgressManager::ProgressManager() :
	plotPosition(1)
{
}

void ProgressManager::setInt(std::string _name, int _val) {
	variables["intVariables"][_name] = _val;
}

void ProgressManager::setString(std::string _name, std::string _val) {
	variables["stringVariables"][_name] = _val;
}

int ProgressManager::getInt(std::string _name) {
	return variables["intVariables"][_name].asInt();
}

std::string ProgressManager::getString(std::string _name) {
	return variables["stringVariables"][_name].asString();
}

void ProgressManager::getNew(){
	// seed the RNG with the current time to give us different results
	sweet::NumberUtils::seed(time(nullptr));

	plotPosition = 1;
	scenarioFile = Json::Value();
	sweet::ShuffleVector<Json::Value> allSideDefs;
	sweet::ShuffleVector<Json::Value> allOmarDefs;
	std::vector<Json::Value> allPlotDefs;
	std::vector<Json::Value> allLabDefs;
	std::vector<Json::Value> allIntroDefs;

	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = sweet::FileUtils::readFile("assets/scenarios.json");
	bool parsingSuccessful = reader.parse(jsonLoaded, root);
	if (!parsingSuccessful) {
		ST_LOG_ERROR("Could not load scenarios listing");
	}

	for (auto scenarioDef : root) {
		ScenarioType type = static_cast<ScenarioType>(scenarioDef.get("type", 0).asInt());
		switch (type) {
		case kSIDE:
			allSideDefs.push(scenarioDef);
			break;
		case kOMAR:
			allOmarDefs.push(scenarioDef);
			break;
		case kPLOT:
			allPlotDefs.push_back(scenarioDef);
			break;
		case kLAB:
			allLabDefs.push_back(scenarioDef);
			break;
		case kINTRO:
			allIntroDefs.push_back(scenarioDef);
			break;
		default:
			ST_LOG_ERROR("Invalid Scenario Type");
			break;
		}
	}

	// Sort the plot and lab defs 
	// Each should have five items - each with a different order going from 1 to 5
	// The lab and plot defs mus line up with one another
	std::sort(allPlotDefs.begin(), allPlotDefs.end(), [](Json::Value a, Json::Value b){
		return a["order"] < b["order"];
	});

	std::sort(allIntroDefs.begin(), allIntroDefs.end(), [](Json::Value a, Json::Value b){
		return a["order"] < b["order"];
	});

	// seed the RNG with the current time to give us different results
	sweet::NumberUtils::seed(time(nullptr));

	assert(allPlotDefs.size() > 0);
	for(unsigned long int i = 0; i < allPlotDefs.size(); ++i) {
		Json::Value scenariosList;

		// This should add each of the plot scenarios in the right order
		// The plot types go from 1 to 5
		// Since we sorted by order this should just work
		scenariosList.append(allPlotDefs[i]["src"].asString());

		assert(scenariosList.size() > 0);
		
		// pick some random side scenarios
		if (i + 1 != kBEGINNING && i + 1 < kEND){
			unsigned long int numSidePlots = sweet::NumberUtils::randomInt(3, 5);
			for(unsigned long int j = 0; j < numSidePlots; ++j) {
				if (allSideDefs.size() > 0){
					scenariosList.append(allSideDefs.pop(true)["src"].asString());
				}
			}
		}

		std::string omarScenario = "";

		// if we're in the middle, pick an omar scenario
		if (i + 1 != kBEGINNING && i + 1 < kEND){
			omarScenario = allOmarDefs.pop(true)["src"].asString(); 
			scenariosList.append(omarScenario);
		}

		// Add the intro scenario second last
		scenariosList.append(i < allIntroDefs.size() ? allIntroDefs[i]["src"].asString() : "intro-scenario.json");

		if (i + 1 != kBEGINNING && i + 1 < kEND){
			std::string omarPrefix = sweet::StringUtils::split(omarScenario, '_')[0];
			for(auto s : allLabDefs) {
				 if(s["src"].asString().find(omarPrefix) != std::string::npos) {
					 scenariosList.append(s["src"]);
					 break;
				 }
			}
		}else {
			for(auto s : allLabDefs) {
				if(s["order"].asInt() == i + 1) {
					scenariosList.append(s["src"]);
				}
			}
		}

		Json::Value outValue;
		outValue["scenarios"] = scenariosList;
		outValue["seed"] = sweet::NumberUtils::randomInt();
		scenarioFile.append(outValue);
	}
	variables = Json::Value();
	variables["intVariables"] = Json::Value();
	variables["stringVariables"] = Json::Value();
	// seed the RNG with 0 to make sure there aren't any differences elswhere
	sweet::NumberUtils::seed(0);
}


void ProgressManager::eraseSave() {
	if(sweet::FileUtils::fileExists("data/save.json")){
		// save a backup file
		std::stringstream backupFilename;
		backupFilename << "data/save." << sweet::DateUtils::getDatetime() << ".json";
		std::ofstream backupFile;
		backupFile.open(backupFilename.str());
		backupFile << sweet::FileUtils::readFile("data/save.json");
		backupFile.close();
		
		// remove the existing file
		std::remove("data/save.json");
	}else{
		Log::warn("Tried to erase save, but no save file was found.");
	}
}

void ProgressManager::save(const Player * const _player, PD_UI_DissBattle * const _uiDissBattle) {
	Json::Value saveOut;
	saveOut["plotPosition"] = (int)plotPosition;
	saveOut["variables"] = variables;
	saveOut["stats"] = Json::Value();
	saveOut["stats"]["strength"] = _player->dissStats->getStrength();
	saveOut["stats"]["sass"] = _player->dissStats->getSass();
	saveOut["stats"]["defense"] = _player->dissStats->getDefense();
	saveOut["stats"]["insight"] = _player->dissStats->getInsight();
	saveOut["stats"]["experience"] = _player->experience;
	saveOut["stats"]["level"] = _player->level;
	for(unsigned long int i = 0; i < _uiDissBattle->lifeTokens.size(); ++i) {
		std::string fileName = "life_token_" + std::to_string(i) + ".tga";
		_uiDissBattle->lifeTokens[i]->saveImageData(fileName);
		saveOut["lifeTokens"].append(fileName);
	}

	saveOut["progress"] = scenarioFile;

	std::ofstream saveFile;
	saveFile.open ("data/save.json");
	saveFile << saveOut;
	saveFile.close();
}

void ProgressManager::loadSave(Player * const _player, PD_UI_DissBattle * const _uiDissBattle) {
	if(_player != nullptr && _uiDissBattle != nullptr){
		if(!sweet::FileUtils::fileExists("data/save.json")){
			// if a save file doesn't exist, create a new one, save it immediately, then load that instead
			getNew();
			save(_player, _uiDissBattle);
		}
	}

	// load the previous save file properties into the appropriate objects
	std::string saveJson = sweet::FileUtils::readFile("data/save.json");
	Json::Reader reader;
	Json::Value root;
	bool parsingSuccsessful = reader.parse(saveJson, root);
	assert(parsingSuccsessful);
	plotPosition = root["plotPosition"].asInt();

	if(_player != nullptr){
		_player->dissStats->incrementStrength(root["stats"]["strength"].asInt());
		_player->dissStats->incrementSass(root["stats"]["sass"].asInt());
		_player->dissStats->incrementDefense(root["stats"]["defense"].asInt());
		_player->dissStats->incrementInsight(root["stats"]["insight"].asInt());
		_player->experience = root["stats"]["experience"].asInt();
		_player->level = root["stats"]["level"].asInt();
	}
	if(_uiDissBattle != nullptr){
		for(auto tex : root["lifeTokens"]) {
			Texture * texture = new Texture("data/images/" + tex.asString(), true, true);
			texture->load();
			_uiDissBattle->addLife(texture);
		}
	}

	variables = root["variables"];
	scenarioFile = root["progress"];
	currentScenarios = scenarioFile[(int)plotPosition-1];
	sweet::NumberUtils::seed(currentScenarios["seed"].asInt());
}