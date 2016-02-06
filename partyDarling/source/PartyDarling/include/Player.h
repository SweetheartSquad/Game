#pragma once

#include <BulletFirstPersonController.h>
class Room;
class Timeout;

class Player : public virtual BulletFirstPersonController{
public:
	float shakeIntensity;
	Timeout * shakeTimeout;

	Player(BulletWorld * bulletWorld);
};