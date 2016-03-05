#pragma once

#include <BulletFirstPersonController.h>
class Room;
class Timeout;

class Player : public virtual BulletFirstPersonController{
public:
	float shakeIntensity;
	Timeout * shakeTimeout;

	// base player speed
	float playerSpeed;
	// sprint multiplier (cumulative with playerSpeed)
	float sprintSpeed;
	// impulse applied upwards when jumping
	float jumpSpeed;
	float mass;
	float maxSpeed;

	int defense;
	int insight;
	int strength;
	int sass;
	bool wonLastYellingContest;
	Player(BulletWorld * bulletWorld);
	~Player();

	glm::vec3 calculateInputs(Step * _step) override;

	virtual void update(Step * _step) override;
};