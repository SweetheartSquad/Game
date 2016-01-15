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
#include <Player.h>


#include <PD_Character.h>

class PD_Scene_CombinedTests : public Scene{
public:
	ComponentShaderBase * shader;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	UILayer uiLayer;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;

	PD_UI_Inventory * uiInventory;
	PD_UI_Bubble * uiBubble;
	PD_UI_Dialogue * uiDialogue;
	PD_UI_YellingContest * uiYellingContest;
	ComponentShaderBase * characterShader;

	NodeUI * crosshairIndicator;

	NodeBulletBody * currentHoverTarget;

	Player * player;

	PD_Scene_CombinedTests(Game * _game);
	~PD_Scene_CombinedTests();
};