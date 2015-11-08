#pragma once

#include <PD_InsultButton.h>

PD_InsultButton::PD_InsultButton(BulletWorld * _world, Scene * _scene) :
	PD_Button(_world, _scene, 5, 5),
	isEffective(false)
{
}