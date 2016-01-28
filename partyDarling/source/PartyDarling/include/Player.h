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
class Timeout;

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
	double jumpTime;

	bool isGrounded;

	Animation<float> * headZoom;
	Tween<float> * headZoomTween1;
	Tween<float> * headZoomTween2;
	float zoomVal;
	/*Animation<float> * easeIntoBobble;
	Tween<float> * easeIntoBobbleTween1;
	float easeIntoBobbleVal;

	Animation<float> * easeOutOfBobble;
	Tween<float> * easeOutOfBobbleTween1;
	float easeOutOfBobbleVal;*/
	
	OpenAL_Sound * footSteps;
	
	OpenAL_Sound * jumpSound;

	OpenAL_Sound * landSound;

	float lastYVel;
	float currentYVel;

	const float playerHeight;
	const float playerRad;

	glm::vec3 lastPos;

	glm::vec3 startPos;

	Sprite * crosshair;
	
public:
	MousePerspectiveCamera * playerCamera;
	glm::vec3 camOffset;
	float shakeIntensity;
	Timeout * shakeTimeout;

	// base player speed
	float playerSpeed;
	// sprint multiplier (cumulative with playerSpeed)
	float sprintSpeed;
	// impulse applied upwards when jumping
	float jumpSpeed;
	float mass;

	Player(BulletWorld * bulletWorld);
	~Player();

	glm::vec3 getLinearVelocity() const;
	glm::vec3 getPosition() const;

	virtual void update(Step * _step) override;

	void enable();
	void disable();
	bool isEnabled();
};