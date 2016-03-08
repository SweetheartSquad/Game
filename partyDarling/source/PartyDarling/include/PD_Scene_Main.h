#pragma once

#include <Scene.h>
#include <Sprite.h>
#include <UILayer.h>
#include <shader/ComponentShaderBase.h>

#include <BulletDebugDrawer.h>
#include <BulletWorld.h>
#include <BulletMeshEntity.h>

#include <PD_UI_Inventory.h>
#include <PD_UI_Bubble.h>
#include <PD_UI_Dialogue.h>
#include <PD_UI_DissBattle.h>
#include <PD_UI_Map.h>
#include <PD_UI_Fade.h>
#include <PD_UI_DissCard.h>
#include <PD_Game.h>
#include <Player.h>

#include <Colour.h>
#include <LabRoom.h>


#include <PD_Character.h>
#include <PD_PhraseGenerator_Incidental.h>
#include "PD_Scenario.h"

class PointLight;
class RampTexture;
class PD_Prop;

class PD_Scene_Main : public Scene{
private:
	// removes _room from the scene/physics
	void removeRoom(Room * _room);
	// adds _room to the scene/physics
	void addRoom(Room * _room);

	LabRoom * labRoom;
public:
	float panSpeed;
	bool panLeft;
	bool panRight;

	float trackSpeed;
	bool trackLeft;
	bool trackRight;

	ScenarioOrder plotPosition;

	ComponentShaderBase * toonShader;
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	UILayer * uiLayer;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	PD_UI_Inventory * uiInventory;
	PD_Item * selectedItem;
	PD_UI_Bubble * uiBubble;
	PD_UI_Dialogue * uiDialogue;
	PD_UI_DissBattle * uiDissBattle;
	PD_UI_Map * uiMap;
	PD_UI_Fade * uiFade;
	ComponentShaderBase * characterShader;
	ComponentShaderBase * emoteShader;
	
	PD_PhraseGenerator_Incidental incidentalPhraseGenerator;

	NodeUI * crosshairIndicator;

	NodeBulletBody * currentHoverTarget;

	Player * player;
	PointLight * playerLight;
	RampTexture * toonRamp;
	glm::vec3 lightStart;
	glm::vec3 lightEnd;
	float lightIntensity;
	
	// how much the screen has transitioned in
	// i.e. 0 is completely out, 1 is normal
	float transition;
	// how much the screen should transitioned in
	// i.e. 0 is completely out, 1 is normal
	float transitionTarget;
	static Colour wipeColour;

	PD_Scene_Main(PD_Game * _game);
	~PD_Scene_Main();


	// the room which the player is currently in
	Room * currentRoom;
	
	// Moves to a new room
	// if _relative is true, currentHousePosition += _movement
	// if _relative is false, currentHousePosition = _movement
	void navigate(glm::ivec2 _movement, bool _relative = true);

	// the house is a square grid layout with houseSize*houseSize cells
	unsigned long int houseSize;
	// the player's current position within the house grid
	glm::ivec2 currentHousePosition;
	// a map which stores all of the currently available rooms using their positions as the keys
	std::map<std::pair<int, int>, Room *> houseGrid;

	Scenario * introScenario;
	Scenario * labScenario;
	std::vector<Scenario *> activeScenarios;


	void updateSelection();
	// randomly pick one main plot scenario and a few side plot scenarios
	void pickScenarios();
	// try to combine definitions in the scenarios
	void bundleScenarios();
	// convert the bundled scenarios into actual rooms and stuff
	std::vector<Room *> buildRooms();
	// place the rooms into the house grid
	void placeRooms(std::vector<Room *> _rooms);
	// checks the cells directly above, below, and beside _pos, and returns those of which are within the house's bounds and haven't been used yet
	std::vector<glm::ivec2> getAdjacentCells(glm::ivec2 _pos, std::map<std::pair<int,int>, bool> &_cells, int _maxSize);
	// trigger a diss battle with another character
	void triggerDissBattle(PD_Character * _enemy);


	// creates a texture which contains whatever is currently rendered, placed into a circle with a pink border
	Texture * getToken();
	// Resets crosshair texture to a crosshair
	void resetCrosshair();

	void save();
	void loadSave();

	// carrying stuff

	PD_Prop * carriedProp;
	float carriedPropDistance;



	// diss battle intro stuff
	Timeout * dissBattleStartTimeout;

	NodeUI * dissBattleStartLayout;
	NodeUI * levelUp;

	PD_Character * dissEnemy;
	PD_UI_DissCard * playerCard, * enemyCard;
	NodeUI * vs;

	Timeout * dissBattleXPGainTimeout;
	Timeout * dissBattleXPPause;
	Timeout * dissBattleLevelUpTimeout;
};