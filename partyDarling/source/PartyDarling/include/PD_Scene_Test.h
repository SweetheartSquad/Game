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

class PD_Scene_Test : public Scene{
private:
	
public:
	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	UILayer * uiLayer;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;

	NodeUI * crosshairIndicator;

	Player * player;

	PD_Scene_Test(PD_Game * _game);
	~PD_Scene_Test();
};