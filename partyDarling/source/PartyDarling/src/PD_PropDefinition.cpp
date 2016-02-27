#pragma once

#include <PD_PropDefinition.h>
#include <PD_FurnitureDefinition.h>
#include <json/json.h>

#include <Resource.h>

#include <Texture.h>

PD_PropDefinition::PD_PropDefinition(Json::Value _jsonDef) :
	mesh(nullptr),
	mass(_jsonDef.get("mass", 0.f).asFloat()),
	padding(_jsonDef.get("padding", 0.f).asFloat()),
	parentDependent(_jsonDef.get("parentDependent", false).asBool()),
	twist(_jsonDef.get("twist", false).asBool()),
	flare(_jsonDef.get("flare", false).asBool())
{
	for(auto parent : _jsonDef["parents"].getMemberNames()) {
		PD_ParentDef def;
		def.parent = parent;
		if(_jsonDef["parents"][parent].size() == 0) {
			for(unsigned long int i = 0; i < 5; ++i) {
				def.sides.push_back(static_cast<PD_Side>(i));
			}
		}else {
			for(auto side : _jsonDef["parents"][parent]) {
				def.sides.push_back(PD_FurnitureSides::fromString(side.asString()));
			}
		}
		parents.push_back(def);
	}
	for(auto type : _jsonDef["roomTypes"]) {
		roomTypes.push_back(type.asString());
	}

	// get the mesh for the prop
	mesh = new TriMesh(false);
	std::string src =  _jsonDef.get("src", "NO_SRC").asString();
	std::vector<TriMesh *> meshes = Resource::loadMeshFromObj("assets/meshes/props/" + src + ".obj", false);
	for(const TriMesh * const m : meshes){
		mesh->insertVertices(*m);
		delete m;
	}
	mesh->setScaleMode(GL_NEAREST);
	
	// get the texture for the prop
	Texture * tex = new Texture("assets/textures/props/" + src + ".png", false, true, true);
	mesh->pushTexture2D(tex);
}

PD_PropDefinition::~PD_PropDefinition(){
	delete mesh;
}