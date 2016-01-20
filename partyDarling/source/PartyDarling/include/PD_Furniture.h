#pragma once

#include <MeshEntity.h>
#include <BulletMeshEntity.h>
#include <RoomObject.h>

class Shader;
class PD_FurnitureComponentContainer;
class PD_FurnitureDefinition;

class PD_Furniture : public RoomObject{
public:

	PD_Furniture(BulletWorld * _bulletWorld, PD_FurnitureDefinition * _def, Shader * _shader, Anchor_t _anchor = GROUND);
};
