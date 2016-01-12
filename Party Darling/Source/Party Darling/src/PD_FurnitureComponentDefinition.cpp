#pragma once

#include <PD_FurnitureComponentDefinition.h>
#include <MeshEntity.h>

PD_FurnitureComponentDefinition::PD_FurnitureComponentDefinition(Json::Value _jsonDef) {
	componentType = _jsonDef.get("componentType", "UNDEFINED").asString();
	bool required = _jsonDef.get("required", true).asBool();
	int multiplier = _jsonDef.get("multiplier", 1).asInt();

	for(auto jsonComp : _jsonDef["outComponents"]) {
		outComponents.push_back(new PD_FurnitureComponentDefinition(jsonComp));
	}
}

MeshEntity * PD_FurnitureComponentDefinition::buildChildren(PD_FurnitureComponentContainer* _componentContainer) const {
	PD_FurnitureComponent * component = _componentContainer->getComponentForType(componentType);
	MeshEntity * ent = new MeshEntity(component->mesh);
	std::vector<MeshEntity *>meshes;
	for(auto def : outComponents) {
		MeshEntity * mesh = def->buildChildren(_componentContainer);
		ent->childTransform->addChild(mesh);
		if(component->connectors.find(def->componentType) != component->connectors.end()) {
			mesh->firstParent()->translate(component->connectors.at(def->componentType));
		}
		meshes.push_back(def->buildChildren(_componentContainer));
	}
	for(auto mesh : meshes) {
		int offset = ent->mesh->vertices.size();
		int indOffet = ent->mesh->indices.size();
		mesh->freezeTransformation();
		ent->mesh->vertices.insert(ent->mesh->vertices.end(), mesh->mesh->vertices.begin(), mesh->mesh->vertices.end());
		ent->mesh->indices.insert(ent->mesh->indices.end(), mesh->mesh->indices.begin(), mesh->mesh->indices.end());
		for(unsigned long int i = indOffet; i < ent->mesh->indices.size(); ++i) {
			ent->mesh->indices.at(i) = ent->mesh->indices.at(i) + offset;
		}
	}
	ent->freezeTransformation();
	return ent;
}
