#pragma once

#include <PD_FurnitureComponentDefinition.h>
#include <PD_ResourceManager.h>
#include <MeshEntity.h>
#include <NumberUtils.h>
#include <PointLight.h>

PD_FurnitureComponentDefinition::PD_FurnitureComponentDefinition(Json::Value _jsonDef) :
	required(_jsonDef.get("required", true).asBool()),
	multiplier(_jsonDef.get("multiplier", 1).asInt()),
	scale(_jsonDef.isMember("scale") ? glm::vec3(
	_jsonDef["scale"].get(Json::Value::ArrayIndex(0), 1.f).asFloat(),
	_jsonDef["scale"].get(Json::Value::ArrayIndex(1), 1.f).asFloat(),
	_jsonDef["scale"].get(Json::Value::ArrayIndex(2), 1.f).asFloat())
	: glm::vec3(1)
	)
{
	for(auto type : _jsonDef["componentTypes"]){
		componentTypes.push_back(type.asString());
	}
	for(auto jsonComp : _jsonDef["outComponents"]) {
		outComponents.push_back(new PD_FurnitureComponentDefinition(jsonComp));
	}
}

PD_FurnitureComponentDefinition::~PD_FurnitureComponentDefinition(){
	while(outComponents.size() > 0){
		delete outComponents.back();
		outComponents.pop_back();
	}
}

PD_BuildResult PD_FurnitureComponentDefinition::build(glm::vec3 _scale){
	_scale *= scale;

	PD_BuildResult res;
	// Get a component for the component type - ex : Leg, Seat, Etc
	std::string type = componentTypes.at(sweet::NumberUtils::randomInt(0, componentTypes.size()-1));
	PD_FurnitureComponent * component = PD_ResourceManager::furnitureComponents->getComponentForType(type);

	// copy the component mesh (we don't want to directly edit it since it's re-used for other furniture)
	res.mesh = new TriMesh(true);
	if(component->mesh != nullptr){
		res.mesh->insertVertices(*component->mesh);
	}

	res.collider = new btCompoundShape();

	// collider for the component mesh
	sweet::Box bb = res.mesh->calcBoundingBox();
	btVector3 boxHalfExtents(bb.width*0.5f*_scale.x, bb.height*0.5f*_scale.y, bb.depth*0.5f*_scale.z);
	btBoxShape * box = new btBoxShape(boxHalfExtents);
	btTransform local;
	local.setIdentity();
	local.setOrigin(btVector3(bb.x + bb.width*0.5f, bb.y + bb.height*0.5f, bb.z + bb.depth*0.5f) * btVector3(_scale.x, _scale.y, _scale.z));
	res.collider->addChildShape(local, box);

	if(type == "lampshade") {
		Transform * t = new Transform();
		PointLight * light = new PointLight(glm::vec3(4.0f), 0.0f, 0.099f, -1);
		t->addChild(light, false);
		res.lights.push_back(light);
		res.lightParents.push_back(t);
	}

	/*btConvexHullShape * shape = new btConvexHullShape();
	for (unsigned long int i = 0; i < res.mesh->vertices.size(); i++){
	btVector3 v = btVector3(res.mesh->vertices[i].x * 0.15f, res.mesh->vertices[i].y * 0.15f, res.mesh->vertices[i].z * 0.15f);
	static_cast<btConvexHullShape *>(shape)->addPoint(v);
	}
	btTransform local;
	local.setIdentity();
	//local.setOrigin(btVector3(-bb.x - bb.width*0.5f, -bb.y - bb.height*0.5f, -bb.z - bb.depth*0.5f) * btVector3(scale.x, scale.y, scale.z) * -0.15f);
	res.collider->addChildShape(local, shape);*/

	for(auto outComponent : outComponents){
		// we always build the child components which are required
		// but randomize whether we build non-required components
		if(outComponent->required || sweet::NumberUtils::randomBool()){
			_scale *= outComponent->scale;
			// retrieve a temporary mesh which is the combination of the outComponent and all of its child components
			PD_BuildResult componentBuildResult = outComponent->build(_scale);
			if(componentBuildResult.lights.size() > 0){
				res.lights.insert(res.lights.end(), componentBuildResult.lights.begin(), componentBuildResult.lights.end());
				res.lightParents.insert(res.lightParents.end(), componentBuildResult.lightParents.begin(), componentBuildResult.lightParents.end());
			}

			assert(outComponent->multiplier <= component->connectors[outComponent->componentTypes].size());

			for(unsigned long int i = 0; i < outComponent->multiplier; ++i){
				_scale *= component->connectors[outComponent->componentTypes].at(i).scale;

				TriMesh * duplicateTempMesh = new TriMesh(false);
				// copy the mesh verts from the build result into a temp
				duplicateTempMesh->insertVertices(*componentBuildResult.mesh);

				// copy the collider from the build result into a temp
				btCompoundShape * s = new btCompoundShape(*componentBuildResult.collider);
				btTransform childShapeTransform;
				childShapeTransform.setIdentity();
				childShapeTransform.setOrigin(btVector3(
					component->connectors[outComponent->componentTypes].at(i).position.x * _scale.x,
					component->connectors[outComponent->componentTypes].at(i).position.y * _scale.y,
					component->connectors[outComponent->componentTypes].at(i).position.z * _scale.z));
				/*s->setLocalScaling(btVector3(_scale.x,
				_scale.y,
				_scale.z));*/
				childShapeTransform.setRotation(btQuaternion(
					component->connectors[outComponent->componentTypes].at(i).rotation.x,
					component->connectors[outComponent->componentTypes].at(i).rotation.y,
					component->connectors[outComponent->componentTypes].at(i).rotation.z));
				res.collider->addChildShape(childShapeTransform, s);

				// translate and scale the temporary mesh to match this components definition
				Transform t;
				t.translate(component->connectors[outComponent->componentTypes].at(i).position);
				t.scale(component->connectors[outComponent->componentTypes].at(i).scale);
				t.rotate(component->connectors[outComponent->componentTypes].at(i).rotation.x, 1, 0, 0, kOBJECT);
				t.rotate(component->connectors[outComponent->componentTypes].at(i).rotation.y, 0, 1, 0, kOBJECT);
				t.rotate(component->connectors[outComponent->componentTypes].at(i).rotation.z, 0, 0, 1, kOBJECT);
				for(auto & l : res.lightParents){
					Transform * lt = new Transform();
					lt->addChild(l, false);
					lt->translate(component->connectors[outComponent->componentTypes].at(i).position);
					lt->scale(component->connectors[outComponent->componentTypes].at(i).scale);
					lt->rotate(component->connectors[outComponent->componentTypes].at(i).rotation.x, 1, 0, 0, kOBJECT);
					lt->rotate(component->connectors[outComponent->componentTypes].at(i).rotation.y, 0, 1, 0, kOBJECT);
					lt->rotate(component->connectors[outComponent->componentTypes].at(i).rotation.z, 0, 0, 1, kOBJECT);
					l = lt;
				}
				duplicateTempMesh->applyTransformation(&t);

				// transfer the temporary mesh verts
				res.mesh->insertVertices(*duplicateTempMesh);

				// get rid of the individual temporary mesh
				delete duplicateTempMesh;
				_scale /= component->connectors[outComponent->componentTypes].at(i).scale;
			}
			// get rid of the global temporary mesh
			delete componentBuildResult.mesh;
			//delete componentBuildResult.collider;
			_scale /= outComponent->scale;
		}
	}

	// scale the combined result by this component's scale
	Transform t;
	t.scale(scale);
	for(auto & l : res.lightParents){
		Transform * lt = new Transform();
		lt->addChild(l, false);
		lt->scale(scale);
		l = lt;
	}
	res.mesh->applyTransformation(&t);
	_scale /= scale;

	return res;
}