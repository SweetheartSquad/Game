#pragma once

#include <PD_Door.h>
#include <MeshFactory.h>
#include <MeshInterface.h>

PD_Door::PD_Door(BulletWorld * _world, Texture * _texture, Shader * _shader) :
	PD_Item("door", "how are you even holding this", _world, _texture, _shader, true, true)
{
	childTransform->scale(4);
	freezeTransformation();
}