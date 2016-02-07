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
#include <PD_UI_YellingContest.h>
#include <PD_Game.h>
#include <Player.h>

#include <Colour.h>


#include <PD_Character.h>
#include <PD_PhraseGenerator_Incidental.h>

class PointLight;
class RampTexture;

class PD_Scene_Main : public Scene{
public:
	ComponentShaderBase * toonShader;
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	UILayer uiLayer;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	PD_UI_Inventory * uiInventory;
	PD_Item * selectedItem;
	PD_UI_Bubble * uiBubble;
	PD_UI_Dialogue * uiDialogue;
	PD_UI_YellingContest * uiYellingContest;
	ComponentShaderBase * characterShader;
	
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


	Room * currentRoom;
	void goToNewRoom();



	std::vector<Scenario *> activeScenarios;

	// randomly pick one main plot scenario and a few side plot scenarios
	void pickScenarios();
	// try to combine definitions in the scenarios
	void bundleScenarios();
	// convert the bundled scenarios into actual rooms and stuff
	void buildHouse();
};