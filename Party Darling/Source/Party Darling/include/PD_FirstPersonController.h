#pragma once

#include <node/NodeChild.h>
#include <node/NodeUpdatable.h>

class Keyboard;
class Mouse;
class Joystick;
class NodeBulletBody;
class PerspectiveCamera;

class PD_FirstPersonController : public virtual NodeChild, public virtual NodeUpdatable{
private:
	Keyboard * keyboard;
	Mouse * mouse;
	Joystick * joystick;
	
	NodeBulletBody * targetEntity;
	PerspectiveCamera * targetCamera;
	
public:
	PD_FirstPersonController(NodeBulletBody * _targetEntity, PerspectiveCamera * _targetCamera);

	virtual void update(Step * _step) override;
};