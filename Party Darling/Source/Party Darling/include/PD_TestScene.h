#pragma once

#include <Scene.h>
#include <UILayer.h>
#include <BulletDebugDrawer.h>
#include <BulletWorld.h>
#include <PD_FirstPersonController.h>

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
class BulletRagdoll;
class ComponentShaderText;
class TextureColourTable;


class PD_TestScene : public Scene{
public:
	
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;

	ComponentShaderBase * shader;
	ComponentShaderText * textShader;
	ShaderComponentHsv * hsvComponent;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	
	PD_FirstPersonController * playerController;
	MousePerspectiveCamera * playerCam;
	BulletMeshEntity * playerEntity;
	glm::vec3 lastPos;
	TextureColourTable * playerPalette;
	
	Sprite * mouseIndicator;
	Sprite * crosshair;
	MousePerspectiveCamera * debugCam;

	virtual void update(Step * _step) override;
	virtual void render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;

	UILayer uiLayer;

	PD_TestScene(Game * _game);
	~PD_TestScene();
};