#pragma once

#include <FirstPersonController.h>
class Room;
class Timeout;

class Player : public virtual FirstPersonController{
public:
	float shakeIntensity;
	Timeout * shakeTimeout;

	Player(BulletWorld * bulletWorld);
};