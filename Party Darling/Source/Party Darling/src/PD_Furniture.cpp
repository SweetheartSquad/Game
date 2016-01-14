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
	for(unsigned long int i = 0; i < _def->components.size(); ++i) {
		auto component = _components->getComponentForType(_def->components.at(i)->componentType);
		MeshInterface * tempMesh = new TriMesh();
		tempMesh->vertices.insert(tempMesh->vertices.end(), component->mesh->vertices.begin(), component->mesh->vertices.end());
		tempMesh->indices.insert(tempMesh->indices.end(), component->mesh->indices.begin(), component->mesh->indices.end());
		meshes.push_back(new MeshEntity(tempMesh));
		for(unsigned long int j = 0; j < _def->components.at(i)->outComponents.size(); ++j) {
			 MeshEntity * ent = _def->components.at(i)->outComponents.at(j)->buildChildren(
				 _components,  _def->components.at(i)->multipliers.at(j), 
				 component->connectors[_def->components.at(i)->outComponents.at(j)->componentType]);
			 meshes.push_back(ent);
		}
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
