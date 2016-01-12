#pragma once

#include <Transform.h>

class Shader;
class PD_FurnitureComponentContainer;
class PD_FurnitureDefinition;

class PD_Furniture : public Transform{
public:

	PD_Furniture(Shader * _shader, PD_FurnitureDefinition * _def, PD_FurnitureComponentContainer * _components);
};
