#pragma once

#include <MeshEntity.h>
#include <BulletMeshEntity.h>

class Shader;
class PD_FurnitureComponentContainer;
class PD_FurnitureDefinition;

class PD_Furniture : public BulletMeshEntity{
public:

	PD_Furniture(BulletWorld * _bulletWorld, Shader * _shader, PD_FurnitureDefinition * _def, PD_FurnitureComponentContainer * _components);
};
