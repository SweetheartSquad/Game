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
	
	// Loop through each of the root components of the furniture definition
	for(unsigned long int i = 0; i < _def->components.size(); ++i) {
		// Get a random component for the current component type
		auto component = _components->getComponentForType(_def->components.at(i)->componentType);
		// Create a new mesh so we don't affect the mesh store in the components container
		TriMesh * tempMesh = new TriMesh();
		tempMesh->vertices.insert(tempMesh->vertices.end(), component->mesh->vertices.begin(), component->mesh->vertices.end());
		tempMesh->indices.insert(tempMesh->indices.end(), component->mesh->indices.begin(), component->mesh->indices.end());
		// Add the temp mesh into meshes for the mesh combine which will occur later
		meshes.push_back(new MeshEntity(tempMesh));
		// Loop through each of the out components for the current component
		for(unsigned long int j = 0; j < _def->components.at(i)->outComponents.size(); ++j) {
			// Build that component and its children and add the returned mesh entity to the meshes
			// array for the mesh combinme which will occur later
			 MeshEntity * ent = _def->components.at(i)->outComponents.at(j)->buildChildren(
				 _components,  _def->components.at(i)->multipliers.at(j), 
				 component->connectors[_def->components.at(i)->outComponents.at(j)->componentType]);
			 meshes.push_back(ent);
		}
	}
	
	// Loop through all the meshes which were created
	for(auto m : meshes) {
		// Create a indice offset and vert offset
		// Since each mesh has indices for its verts we need to take into account
		// How many vertices have already been added to the combined mesh and 
		// offset the index value by that amount
		int indOffset = mesh->indices.size();
		int vertOffset = mesh->vertices.size();
		mesh->vertices.insert(mesh->vertices.end(), m->mesh->vertices.begin(), m->mesh->vertices.end());
		mesh->indices.insert(mesh->indices.end(), m->mesh->indices.begin(), m->mesh->indices.end());
		for(unsigned long int i = indOffset; i < mesh->indices.size(); ++i) {
			mesh->indices.at(i) = mesh->indices.at(i) + vertOffset;
		}
	}

	// Make the mesh dirty since the verts have changed
	// May be redunant but do it as a safe guard
	mesh->dirty = true;

	// Cleanup all the meshes we created
	for(auto m : meshes) {
		childTransform->removeChild(m->firstParent());
		delete m;
	}
}
