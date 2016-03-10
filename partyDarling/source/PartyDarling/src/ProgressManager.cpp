#pragma once

#include <ProgressManager.h>
#include <FileUtils.h>
#include <Log.h>
#include <PD_Scenario.h>

ProgressManager::ProgressManager() :
	plotPosition(1)
{
	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = sweet::FileUtils::readFile("assets/scenarios.json");
	bool parsingSuccessful = reader.parse(jsonLoaded, root);
	if(!parsingSuccessful) {
		ST_LOG_ERROR("Could not load scenarios listing");
	}

	for(auto scenarioDef : root) {
		ScenarioType type = static_cast<ScenarioType>(scenarioDef.get("type", 0).asInt());
		switch(type) {
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

	std::sort(allLabDefs.begin(), allLabDefs.end(), [](Json::Value a, Json::Value b){
		return a["order"] < b["order"];
	});

	std::sort(allIntroDefs.begin(), allIntroDefs.end(), [](Json::Value a, Json::Value b){
		return a["order"] < b["order"];
	});
}

void ProgressManager::getNew(){

	assert(allPlotDefs.size() > 0);
	for(unsigned long int i = 0; i < allPlotDefs.size(); ++i) {
		Json::Value scenariosList;

		// This should add each of the plot scenarios in the right order
		// The plot types go from 1 to 5
		// Since we sorted by order this should just work
		scenariosList.append(allPlotDefs[i]["src"].asString());

		assert(scenariosList.size() > 0);
		
		// pick some random side scenarios
		int numSidePlots = sweet::NumberUtils::randomInt(3, 5);
		for(unsigned long int j = 0; j < numSidePlots; ++j) {
			if (allSideDefs.size() > 0){
				scenariosList.append(allSideDefs.pop()["src"].asString());
			}
		}

		// if we're in the middle, pick an omar scenario
		if(plotPosition != kBEGINNING && plotPosition != kEND){
			scenariosList.append(allOmarDefs.pop()["src"].asString());
		}

		// Add the intro scenario second last
		scenariosList.append(i < allIntroDefs.size() ? allIntroDefs[i]["src"].asString() : "intro-scenario.json");

		// Add the lab def last
		// We shouldn't need this check but we'll leave it here until all the scenarios are in
		scenariosList.append(i < allLabDefs.size() ? allLabDefs[i]["src"].asString() : "lab-scenario.json");

		Json::Value outValue;
		outValue["scenarios"] = scenariosList;
		outValue["seed"] = sweet::NumberUtils::randomInt(0, INT_MAX);
		scenarioFile.append(outValue);
	}
}

Json::Value ProgressManager::getCurrentScenarios(){
	Json::Value res;
	// grab the current scenario list from the save file
	int i = 0;
	for(auto scenariosList : scenarioFile) {
		if(++i == plotPosition){
			res = scenariosList;
			return res;
		}
	}
	Log::error("Scenarios for current plot position not found.");
}