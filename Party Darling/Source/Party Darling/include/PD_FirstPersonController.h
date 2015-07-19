#pragma once

#include <node/NodeChild.h>
#include <node/NodeUpdatable.h>

class Keyboard;
class Mouse;
class Joystick;
class NodeBulletBody;
class MousePerspectiveCamera ;

class PD_FirstPersonController : public virtual NodeChild, public virtual NodeUpdatable{
private:
	Keyboard * keyboard;
	Mouse * mouse;
	Joystick * joystick;
	
	NodeBulletBody * targetEntity;
	MousePerspectiveCamera * targetCamera;
	
public:
	PD_FirstPersonController(NodeBulletBody * _targetEntity, MousePerspectiveCamera  * _targetCamera);

	virtual void update(Step * _step) override;
};