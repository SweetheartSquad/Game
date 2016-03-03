#pragma once

#include <glm/glm.hpp>

#include <json/json.h>

#include <node/Node.h>
#include <Easing.h>

class PD_CharacterAnimationStep : public Node {	
public:

	std::string interpolation;
	float time;
	glm::vec2 leftArm;
	glm::vec2 rightArm;
	glm::vec2 leftLeg;
	glm::vec2 rightLeg;
	glm::vec2 body;

	explicit PD_CharacterAnimationStep(Json::Value _jsonDef);
	explicit PD_CharacterAnimationStep();
};
