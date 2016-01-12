#pragma once

#include <PD_Furniture.h>
#include <PD_FurnitureParser.h>
#include <PD_FurnitureComponent.h>
#include <shader/Shader.h>
#include <MeshEntity.h>
#include <PD_FurnitureComponentDefinition.h>

PD_Furniture::PD_Furniture(Shader * _shader, PD_FurnitureDefinition * _def, PD_FurnitureComponentContainer * _components) :
	MeshEntity(new TriMesh(), _shader)
{
	std::vector<MeshEntity *>meshes;
	for(auto component : _def->components) {
		meshes.push_back(component->buildChildren(_components));
	}
	for(auto m : meshes) {
		int indOffset = mesh->indices.size();
		int offset = mesh->vertices.size();
		mesh->vertices.insert(mesh->vertices.end(), m->mesh->vertices.begin(), m->mesh->vertices.end());
		mesh->indices.insert(mesh->indices.end(), m->mesh->indices.begin(), m->mesh->indices.end());
		for(unsigned long int i = indOffset; i < mesh->indices.size(); ++i) {
			mesh->indices.at(i) = mesh->indices.at(i) + offset;
		}
	}
	mesh->dirty = true;
	for(auto m : meshes) {
		childTransform->removeChild(m->firstParent());
		delete m;
	}
}
