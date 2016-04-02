#pragma once

#include <BulletFirstPersonController.h>
#include <JoystickManager.h>
#include <Joystick.h>
#include <Joystick.h>
class Room;
class Timeout;
class PD_DissStats;

class Player : public virtual BulletFirstPersonController{
public:
	float shakeIntensity;
	Timeout * shakeTimeout;
	JoystickManager * joystickManager;

	// base player speed
	float playerSpeed;
	// sprint multiplier (cumulative with playerSpeed)
	float sprintSpeed;
	// impulse applied upwards when jumping
	float jumpSpeed;
	float mass;
	float maxSpeed;

	float experience;
	float level;

	PD_DissStats * dissStats;
	bool wonLastDissBattle;
	Player(BulletWorld * bulletWorld);
	~Player();

	glm::vec3 calculateInputs(Step * _step) override;

	virtual void update(Step * _step) override;


	bool wantsToJump();
	bool wantsToSprint();
	bool wantsToInteract();
	bool wantsToStopInteracting();
	bool wantsToKeepInteracting();
	bool wantsToQuit();
	bool wantsToMap();
	bool wantsToInventory();
	bool wantsToTaskList();

	bool wantsNextBubble();
	bool wantsPrevBubble();
	
	bool wantsToInterject();
	bool interjecting();
	bool wantsToInsultUp();
	bool wantsToInsultDown();
};