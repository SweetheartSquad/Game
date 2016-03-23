#pragma once

#include <node/Node.h>
#include <json/json.h>
#include <NumberUtils.h>
#include <EventManager.h>
class Player;
class PD_UI_DissBattle;
class PD_UI_Inventory;

class ProgressManager : public Node{
public:
	// current progress save file
	Json::Value scenarioFile;
	Json::Value variables;
	// scenarios in the current plot position in the current progress save file
	Json::Value currentScenarios;

	unsigned long int plotPosition;

	ProgressManager();

	void save(const Player * const _player, PD_UI_DissBattle * const _uiDissBattle);
	void loadSave(Player * const _player = nullptr, PD_UI_DissBattle * const _uiDissBattle = nullptr, PD_UI_Inventory * const _uiInventory = nullptr);
	void eraseSave();

	void getNew();
	Json::Value getCurrentScenarios();

	void setInt(std::string _name, int _val);
	void setString(std::string _name, std::string _val);
	int getInt(std::string _name);
	std::string getString(std::string _name);
};