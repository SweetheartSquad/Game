#pragma once
#include <string>
#include <vector>
#include <json/json.h>
#include "PD_FurnitureParser.h"
#include <BulletMeshEntity.h>

class MeshEntity;

struct PD_BuildResult{
	TriMesh * mesh;
	btCompoundShape * collider;
};

class PD_FurnitureComponentDefinition {
public:
	std::vector<std::string> componentTypes;
	bool required;
	std::vector<PD_FurnitureComponentDefinition *> outComponents;
	unsigned long int multiplier;

	glm::vec3 scale;

	explicit PD_FurnitureComponentDefinition(Json::Value _jsonDef);

	// recusively builds this component and all of its children,
	// returning the combined mesh
	PD_BuildResult build();
};
