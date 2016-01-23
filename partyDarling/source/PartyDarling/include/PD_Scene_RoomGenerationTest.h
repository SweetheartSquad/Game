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


#include <PD_Character.h>

class Room;

class PD_Scene_RoomGenerationTest : public Scene{
public:
	ComponentShaderBase * shader;

	Room * room;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	UILayer uiLayer;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	ComponentShaderBase * characterShader;

	NodeUI * crosshairIndicator;

	PD_Scene_RoomGenerationTest(Game * _game);
	~PD_Scene_RoomGenerationTest();

	std::vector<glm::vec3> getLocalBoundingBoxVertices(std::vector<glm::vec3> _verts, glm::mat4 _mmA, glm::mat4 _mmB);
};