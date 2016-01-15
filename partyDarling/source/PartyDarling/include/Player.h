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

#include <OpenALSound.h>

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
	bool enabled;

	Keyboard * keyboard;
	Mouse * mouse;
	Joystick * joystick;

	float camYpos;

	Animation<float> * headBobble;
	Tween<float> * headBobbleTween1;
	Tween<float> * headBobbleTween2;
	float bobbleVal;
	float bobbleInterpolation;
	int currentBobbleTween;
	int lastBobbleTween;
	bool tweenBobbleChange;

	bool isGrounded;

	/*Animation<float> * easeIntoBobble;
	Tween<float> * easeIntoBobbleTween1;
	float easeIntoBobbleVal;

	Animation<float> * easeOutOfBobble;
	Tween<float> * easeOutOfBobbleTween1;
	float easeOutOfBobbleVal;*/
	
	OpenAL_SoundSimple * footSteps;
	
	OpenAL_SoundSimple * jumpSound;

	OpenAL_SoundSimple * landSound;

	float lastYVel;
	float currentYVel;

	float playerHeight;
	float playerRad;

	glm::vec2 glmLastVelocityXZ;
	glm::vec2 glmCurVelocityXZ;

	glm::vec3 lastPos;

	glm::vec3 startPos;

	Sprite * crosshair;
	
public:
	MousePerspectiveCamera * playerCamera;

	float playerSpeed;
	float mass;
	float initSpeed;
	float sprintSpeed;

	Player(BulletWorld * bulletWorld);
	~Player();

	glm::vec3 getPlayerLinearVelocity();
	glm::vec3 getPlayerPosition();

	virtual void update(Step * _step) override;

	void enable();
	void disable();
	bool isEnabled();
};