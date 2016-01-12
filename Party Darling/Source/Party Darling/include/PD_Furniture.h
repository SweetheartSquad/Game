#pragma once

#include <MeshEntity.h>

class Shader;
class PD_FurnitureComponentContainer;
class PD_FurnitureDefinition;

class PD_Furniture : public MeshEntity{
public:

	PD_Furniture(Shader * _shader, PD_FurnitureDefinition * _def, PD_FurnitureComponentContainer * _components);
};
