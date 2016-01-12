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
	std::vector<MeshEntity *>meshes;
	MeshEntity * ent = new MeshEntity(new TriMesh());
	for(unsigned long int y = 0; y < _multiplier; y++){
		PD_FurnitureComponent * component = _componentContainer->getComponentForType(componentType);
		TriMesh * tempMesh = new TriMesh();
		tempMesh->vertices.insert(tempMesh->vertices.end(), component->mesh->vertices.begin(), component->mesh->vertices.end());
		tempMesh->indices.insert(tempMesh->indices.end(), component->mesh->indices.begin(), component->mesh->indices.end());
		MeshEntity * compMesh = new MeshEntity(tempMesh);
		compMesh->meshTransform->translate(_positions[y]);
		compMesh->freezeTransformation();
		meshes.push_back(compMesh);
		for(unsigned long int x = 0; x < outComponents.size(); ++x){
			if(component->connectors.find(outComponents.at(x)->componentType) != component->connectors.end()) {
				MeshEntity * mesh = outComponents.at(x)->buildChildren(
					_componentContainer, multipliers[x], 
					component->connectors[outComponents.at(x)->componentType]);
				meshes.push_back(mesh);
				mesh->freezeTransformation();
			}
		}
		for(auto mesh : meshes) {
			int offset = ent->mesh->vertices.size();
			int indOffet = ent->mesh->indices.size();
			ent->mesh->vertices.insert(ent->mesh->vertices.end(), mesh->mesh->vertices.begin(), mesh->mesh->vertices.end());
			ent->mesh->indices.insert(ent->mesh->indices.end(), mesh->mesh->indices.begin(), mesh->mesh->indices.end());
			for(unsigned long int i = indOffet; i < ent->mesh->indices.size(); ++i) {
				ent->mesh->indices.at(i) = ent->mesh->indices.at(i) + offset;
			}
		}
		meshes.clear();
	}
	ent->freezeTransformation();
	return ent;
}
