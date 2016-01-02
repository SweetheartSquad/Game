#pragma once

#include <PD_Door.h>
#include <MeshFactory.h>
#include <MeshInterface.h>

PD_Door::PD_Door(BulletWorld * _world, Texture * _texture, Shader * _shader) :
	PD_Item(_world, _texture, _shader, false, true)
{
	childTransform->scale(10);
	freezeTransformation();
	
	setColliderAsBoundingBox();
	createRigidBody(0);
}