#pragma once

#include <PD_FurnitureComponentDefinition.h>
#include <MeshEntity.h>

PD_FurnitureComponentDefinition::PD_FurnitureComponentDefinition(Json::Value _jsonDef) {
	componentType = _jsonDef.get("componentType", "UNDEFINED").asString();
	required = _jsonDef.get("required", true).asBool();
	multiplier = _jsonDef.get("multiplier", 1).asInt();

	for(auto mult : _jsonDef["multipliers"]) {
		multipliers.push_back(mult.asInt());
	}

	for(auto jsonComp : _jsonDef["outComponents"]) {
		outComponents.push_back(new PD_FurnitureComponentDefinition(jsonComp));
	}
}

MeshEntity * PD_FurnitureComponentDefinition::buildChildren(PD_FurnitureComponentContainer * _componentContainer, int _multiplier, std::vector<glm::vec3> _positions) const {
	
	// The number of positions should match the multiplier
	assert(_positions.size() == _multiplier);

	// An array of meshes which will be combined into a final mesh
	std::vector<MeshEntity *>meshes;
	
	// The final mesh entity which is populated with verts and returned
	MeshEntity * ent = new MeshEntity(new TriMesh());
	
	// Get a component for the component type - ex : Leg, Seat, Etc
	PD_FurnitureComponent * component = _componentContainer->getComponentForType(componentType);

	// We need to loop for the value of _multiplier as there may be multiple of one component
	// for example a chair seat has 4 legs - so the leg multiplier is 4
	for(unsigned long int multIdx = 0; multIdx < _multiplier; multIdx++){
		// Create a temporary tri mesh and copy in the verts and indices from the component mesh
		// We do this so that we don't affect the original mesh when freezing transformations
		// As this will screw things up here, as well as outside the scope of this class
		TriMesh * tempMesh = new TriMesh();
		tempMesh->vertices.insert(tempMesh->vertices.end(), component->mesh->vertices.begin(), component->mesh->vertices.end());
		tempMesh->indices.insert(tempMesh->indices.end(), component->mesh->indices.begin(), component->mesh->indices.end());
		MeshEntity * compMesh = new MeshEntity(tempMesh);

		// Translate the MeshEntity created from the temporary mesh
		// by the value located in _positions at the current multiplier value
		compMesh->meshTransform->translate(_positions[multIdx]);
		// Freeze the transformations
		compMesh->freezeTransformation();
		// Add it to the list of meshes
		meshes.push_back(compMesh);
		// Loop through each of the out components for this component definition
		for(unsigned long int compIdx = 0; compIdx < outComponents.size(); ++compIdx){
			// Make sure we have the connector data for the 
			if(component->connectors.find(outComponents.at(compIdx)->componentType) != component->connectors.end()) {
				// Build the component and its children
				MeshEntity * mesh = outComponents.at(compIdx)->buildChildren(
					_componentContainer, multipliers[compIdx], 
					component->connectors[outComponents.at(compIdx)->componentType]);
				// Add the resulting mesh to the meshes vector
				meshes.push_back(mesh);
			}else {
				ST_LOG_ERROR_V("Invalid connector data found when bulding furniture component");
			}
		}
	}
	for(auto mesh : meshes) {
		// Create a indice offset and vert offset
		// Since each mesh has indices for its verts we need to take into account
		// How many vertices have already been added to the combined mesh and 
		// offset the index value by that amount
		int vertOffset = ent->mesh->vertices.size();
		int indOffet = ent->mesh->indices.size();
		ent->mesh->vertices.insert(ent->mesh->vertices.end(), mesh->mesh->vertices.begin(), mesh->mesh->vertices.end());
		ent->mesh->indices.insert(ent->mesh->indices.end(), mesh->mesh->indices.begin(), mesh->mesh->indices.end());
		for(unsigned long int i = indOffet; i < ent->mesh->indices.size(); ++i) {
			ent->mesh->indices.at(i) = ent->mesh->indices.at(i) + vertOffset;
		}
	}
	// Delete temporary meshes
	for(auto m : meshes) {
		delete m;
	}
	// Returned the MeshEntity with the combined mesh
	return ent;
}
