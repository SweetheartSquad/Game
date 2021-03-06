#pragma once

#include <RoomObject.h>

#define FURNITURE_MASS_SCALE 0.05f
#define FURNITURE_SCALE 0.15f

class Light;
class Shader;
class PD_FurnitureDefinition;
class PD_FurnitureComponentContainer;

class PD_Furniture : public RoomObject{
public:
	PD_Furniture(BulletWorld * _bulletWorld, PD_FurnitureDefinition * _def, Shader * _shader, Anchor_t _anchor = GROUND);
};
