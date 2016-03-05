#pragma once
#include <string>
#include <vector>
#include <json/json.h>
#include "PD_FurnitureParser.h"
#include <BulletMeshEntity.h>

class MeshEntity;

struct PD_BuildResult : public Node{
	TriMesh * mesh;
	btCompoundShape * collider;
	std::vector<Light *> lights;
	std::vector<Transform *> lightParents;
};

class PD_FurnitureComponentDefinition {
public:
	std::vector<std::string> componentTypes;
	bool required;
	std::vector<PD_FurnitureComponentDefinition *> outComponents;
	unsigned long int multiplier;

	glm::vec3 scale;

	explicit PD_FurnitureComponentDefinition(Json::Value _jsonDef);
	~PD_FurnitureComponentDefinition();

	// recusively builds this component and all of its children,
	// returning the combined mesh
	PD_BuildResult build(glm::vec3 _scale = glm::vec3(0.15f));
};
