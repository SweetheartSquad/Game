#pragma once

#include <Scene.h>
#include <UILayer.h>
#include <BulletDebugDrawer.h>
#include <BulletWorld.h>
#include <PD_FirstPersonController.h>
#include <PD_TilemapGenerator.h>
#include <NodeBulletBody.h>


class PerspectiveCamera;
class MousePerspectiveCamera;

class Box2DWorld;
class Box2DDebugDrawer;
class Box2DMeshEntity;
class MeshEntity;

class ShaderComponentHsv;

class Shader;
class RenderSurface;
class StandardFrameBuffer;
class Material;
class Sprite;

class PointLight;

class BulletMeshEntity;
class NodeBulletBody;
class BulletRagdoll;
class ComponentShaderText;
class TextureColourTable;

class Room;

class PD_Nav : public Scene{
public:
	ComponentShaderBase * diffuseShader;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	PD_FirstPersonController * playerController;
	MousePerspectiveCamera * playerCam;
	NodeBulletBody * playerEntity;
	glm::vec3 lastPos;

	Sprite * crosshair;
	MousePerspectiveCamera * debugCam;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;

	UILayer uiLayer;

	PD_Nav(Game * _game);
	~PD_Nav();
};