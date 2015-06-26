#pragma once

#include <Scene.h>
#include <SoundManager.h>
#include <UILayer.h>
#include <Joystick.h>
#include <JoystickManager.h>

#include <Font.h>

#include <BulletDebugDrawer.h>
#include <BulletWorld.h>
#include <TextLabel.h>

class MousePerspectiveCamera;
class FollowCamera;

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


class PD_TestScene : public Scene{
public:
	ComponentShaderBase * shader;
	ComponentShaderText * textShader;
	ShaderComponentHsv * hsvComponent;
	MousePerspectiveCamera * mouseCam;
	MousePerspectiveCamera * debugCam;
	FollowCamera * gameCam;
	glm::vec3 lastPos;

	Font  * font;

	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	Material * phongMat;

	float sceneHeight;
	float sceneWidth;

	bool firstPerson;
	JoystickManager * joy;

	virtual void update(Step * _step) override;
	virtual void render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;

	std::vector<MeshEntity *> audioVisualizer;

	UILayer uiLayer;
	Sprite * crosshair;
	Sprite * playerIndicator;
	Sprite * mouseIndicator;
	Sprite * volumeIndicator;

	TextArea * textArea;

	PD_TestScene(Game * _game);
	~PD_TestScene();

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;

	BulletMeshEntity * addThing();

	BulletRagdoll * ragdoll;
};