#pragma once

#include <node/Node.h>
#include <json/json.h>
#include <NumberUtils.h>

class ProgressManager : public Node{
public:
	sweet::ShuffleVector<Json::Value> allSideDefs;
	sweet::ShuffleVector<Json::Value> allOmarDefs;
	std::vector<Json::Value> allPlotDefs;
	std::vector<Json::Value> allLabDefs;
	std::vector<Json::Value> allIntroDefs;

	// current progress save file
	Json::Value scenarioFile;

	unsigned long int plotPosition;

	ProgressManager();

	void getNew();
	Json::Value getCurrentScenarios();
};