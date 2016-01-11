#pragma once

#include <node/NodeChild.h>
#include <node/NodeUpdatable.h>

#include <Scene.h>
#include <BulletDebugDrawer.h>
#include <BulletWorld.h>
#include <NodeBulletBody.h>
#include <MousePerspectiveCamera.h>

#include <Animation.h>
#include <Tween.h>

class Keyboard;
class Mouse;
class Joystick;


class PerspectiveCamera;
class MousePerspectiveCamera;

class ShaderComponentHsv;

class Shader;
class RenderSurface;
class StandardFrameBuffer;
class Material;
class Sprite;

class BulletMeshEntity;
class NodeBulletBody;
class BulletRagdoll;
class ComponentShaderText;
class TextureColourTable;

class Room;

class Player : public virtual NodeBulletBody{
private:
	Keyboard * keyboard;
	Mouse * mouse;
	Joystick * joystick;

	MousePerspectiveCamera * playerCamera;
	float camYpos;

	Animation<float> * headBobble;
	Tween<float> * headBobbleTween1;
	Tween<float> * headBobbleTween2;
	float bobbleVal;
	float bobbleInterpolation;

	/*Animation<float> * easeIntoBobble;
	Tween<float> * easeIntoBobbleTween1;
	float easeIntoBobbleVal;

	Animation<float> * easeOutOfBobble;
	Tween<float> * easeOutOfBobbleTween1;
	float easeOutOfBobbleVal;*/

	ComponentShaderBase * diffuseShader;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	glm::vec3 lastPos;

	glm::vec3 startPos;

	Sprite * crosshair;
	
public:

	float playerSpeed;
	float mass;
	float initSpeed;
	float sprintSpeed;

	Player(BulletWorld * bulletWorld, MousePerspectiveCamera * playerCamera);

	glm::vec3 getPlayerLinearVelocity();
	glm::vec3 getPlayerPosition();

	virtual void update(Step * _step) override;
};