#pragma once

#include <PD_FurnitureComponentDefinition.h>
#include <PD_ResourceManager.h>
#include <MeshEntity.h>
#include <NumberUtils.h>

PD_FurnitureComponentDefinition::PD_FurnitureComponentDefinition(Json::Value _jsonDef) :
	required(_jsonDef.get("required", true).asBool()),
	multiplier(_jsonDef.get("multiplier", 1).asInt()),
	scale(_jsonDef.isMember("scale") ? (
		_jsonDef["scale"].get(Json::Value::ArrayIndex(0), 1.f).asFloat(),
		_jsonDef["scale"].get(Json::Value::ArrayIndex(1), 1.f).asFloat(),
		_jsonDef["scale"].get(Json::Value::ArrayIndex(2), 1.f).asFloat())
		: 1
	)
{
	for(auto type : _jsonDef["componentTypes"]){
		componentTypes.push_back(type.asString());
	}
	for(auto jsonComp : _jsonDef["outComponents"]) {
		outComponents.push_back(new PD_FurnitureComponentDefinition(jsonComp));
	}
}

TriMesh * PD_FurnitureComponentDefinition::build(){
	// Get a component for the component type - ex : Leg, Seat, Etc
	std::string type = componentTypes.at(sweet::NumberUtils::randomInt(0, componentTypes.size()-1));
	PD_FurnitureComponent * component = PD_ResourceManager::furnitureComponents->getComponentForType(type);

	TriMesh * res = new TriMesh();
	res->vertices.insert(res->vertices.end(), component->mesh->vertices.begin(), component->mesh->vertices.end());
	res->indices.insert(res->indices.end(), component->mesh->indices.begin(), component->mesh->indices.end());

	for(auto outComponent : outComponents){
		// we always build the child components which are required
		// but randomize whether we build non-required components
		if(outComponent->required || sweet::NumberUtils::randomBool()){
			// retrieve a temporary mesh which is the combination of the outComponent and all of its child components
			TriMesh * tempMesh = outComponent->build();

			
			for(unsigned long int i = 0; i < outComponent->multiplier; ++i){
				TriMesh * duplicateTempMesh = new TriMesh();
				// copy the verts from the temporary mesh into this one
				duplicateTempMesh->vertices.insert(duplicateTempMesh->vertices.end(), tempMesh->vertices.begin(), tempMesh->vertices.end());
				duplicateTempMesh->indices.insert(duplicateTempMesh->indices.end(), tempMesh->indices.begin(), tempMesh->indices.end());

				MeshEntity * tempMeshEntity = new MeshEntity(duplicateTempMesh);
				// translate and scale the temporary mesh to match this components definition
				tempMeshEntity->meshTransform->translate(component->connectors[outComponent->componentTypes].at(i));
				tempMeshEntity->meshTransform->scale(outComponent->scale);
				tempMeshEntity->freezeTransformation();

				// save the number of vertices and indices so we can offset them later
				unsigned long int vertOffset = res->vertices.size();
				unsigned long int indOffet = res->indices.size();
		
				// copy the verts from the temporary mesh into this one
				res->vertices.insert(res->vertices.end(), duplicateTempMesh->vertices.begin(), duplicateTempMesh->vertices.end());
				res->indices.insert(res->indices.end(), duplicateTempMesh->indices.begin(), duplicateTempMesh->indices.end());
				
				// offset the inserted indices
				for(unsigned long int i = indOffet; i < res->indices.size(); ++i) {
					res->indices.at(i) += vertOffset;
				}
				// get rid of the individual temporary mesh
				delete duplicateTempMesh;
			}
			// get rid of the global temporary mesh
			delete tempMesh;
		}
	}

	return res;
}