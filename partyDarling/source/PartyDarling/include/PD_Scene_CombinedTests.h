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


#include <PD_Character.h>

class PointLight;
class RampTexture;

class PD_Scene_CombinedTests : public Scene{
public:
	ComponentShaderBase * toonShader;

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


	NodeUI * crosshairIndicator;

	NodeBulletBody * currentHoverTarget;

	Player * player;
	PointLight * playerLight;
	RampTexture * toonRamp;
	glm::vec3 lightStart;
	glm::vec3 lightEnd;
	float lightIntensity;

	PD_Scene_CombinedTests(PD_Game * _game);
	~PD_Scene_CombinedTests();
};