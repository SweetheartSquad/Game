#pragma once

#include <MeshEntity.h>
#include <BulletMeshEntity.h>
#include <RoomObject.h>

class Shader;
class PD_PropDefinition;

class PD_Prop : public RoomObject{
public:

	PD_Prop(BulletWorld * _bulletWorld, PD_PropDefinition * _def, Shader * _shader, Anchor_t _anchor = GROUND);
};
