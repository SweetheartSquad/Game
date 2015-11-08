#pragma once

#include <PD_Button.h>

class PD_InsultButton : public PD_Button{
public:
	bool isEffective;

	PD_InsultButton(BulletWorld * _world, Scene * _scene);
};