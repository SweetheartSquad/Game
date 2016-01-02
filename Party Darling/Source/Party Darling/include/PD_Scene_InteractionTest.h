#pragma once

#include <Scene.h>
#include <Sprite.h>
#include <UILayer.h>
#include <shader/ComponentShaderBase.h>

#include <BulletDebugDrawer.h>
#include <BulletWorld.h>
#include <BulletMeshEntity.h>

#include <PD_UI_Inventory.h>

class PD_Scene_InteractionTest : public Scene{
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

	PD_Scene_InteractionTest(Game * _game);
	~PD_Scene_InteractionTest();
};