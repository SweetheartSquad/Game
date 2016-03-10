#pragma once

#include <node/Node.h>
#include <json/json.h>
#include <NumberUtils.h>
class Player;
class PD_UI_DissBattle;

class ProgressManager : public Node{
public:
	sweet::ShuffleVector<Json::Value> allSideDefs;
	sweet::ShuffleVector<Json::Value> allOmarDefs;
	std::vector<Json::Value> allPlotDefs;
	std::vector<Json::Value> allLabDefs;
	std::vector<Json::Value> allIntroDefs;

	// current progress save file
	Json::Value scenarioFile;
	// scenarios in the current plot position in the current progress save file
	Json::Value currentScenarios;

	unsigned long int plotPosition;

	ProgressManager();

	void save(const Player * const _player, PD_UI_DissBattle * const _uiDissBattle);
	void loadSave(Player * const _player, PD_UI_DissBattle * const _uiDissBattle);
	void eraseSave();

	void getNew();
	Json::Value getCurrentScenarios();
};