#pragma once

#include <Scene.h>
#include <SoundManager.h>
#include <UILayer.h>
#include <Joystick.h>
#include <JoystickManager.h>

#include <Font.h>
#include <Label.h>

#include <btBulletDynamicsCommon.h>
#include <BulletDebugDrawer.h>

class MousePerspectiveCamera;
class FollowCamera;

class Box2DWorld;
class Box2DDebugDraw;
class Box2DMeshEntity;
class MeshEntity;

class ShaderComponentHsv;

class Shader;
class RenderSurface;
class StandardFrameBuffer;
class Material;
class Sprite;

class PD_Player;



class PD_TestScene : public Scene{
public:
	BaseComponentShader * shader;
	BaseComponentShader * textShader;
	ShaderComponentHsv * hsvComponent;
	MousePerspectiveCamera * mouseCam;
	MousePerspectiveCamera * debugCam;
	FollowCamera * gameCam;

	Box2DWorld * world;
	Box2DDebugDraw * drawer;
	PD_Player * player;

	Font  * font;
	Label * label;

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

	PD_TestScene(Game * _game);
	~PD_TestScene();


	// bullet test
	btCollisionConfiguration * collisionConfig;
	btDispatcher * dispatcher;
	btBroadphaseInterface * broadphase;
	btConstraintSolver * solver;
	btDynamicsWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	std::vector<btRigidBody *> bodies;
	std::vector<MeshEntity *> bodies2;

	void addThing();
};