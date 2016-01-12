#pragma once

#include <PD_Furniture.h>
#include <PD_FurnitureParser.h>
#include <PD_FurnitureComponent.h>
#include <shader/Shader.h>
#include <MeshEntity.h>
#include <PD_FurnitureComponentDefinition.h>

PD_Furniture::PD_Furniture(Shader * shader, PD_FurnitureDefinition * _def, PD_FurnitureComponentContainer * _components) :
	Transform()
{
	for(auto component : _def->components) {
		Transform::addChild(new MeshEntity(component->build(_components)->mesh, shader));
	}
}
