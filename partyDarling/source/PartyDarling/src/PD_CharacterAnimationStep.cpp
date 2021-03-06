#pragma once

#include <PD_CharacterAnimationStep.h>

PD_CharacterAnimationStep::PD_CharacterAnimationStep(Json::Value _jsonDef) {
	interpolation = _jsonDef.get("interpolation", "NO_INTERPOLATION").asString();
	time  = _jsonDef.get("time", 0.0f).asFloat();
	leftArm = glm::vec2(_jsonDef["leftArm"][0].asFloat(), _jsonDef["leftArm"][1].asFloat());
	rightArm = glm::vec2(_jsonDef["rightArm"][0].asFloat(), _jsonDef["rightArm"][1].asFloat());
	leftLeg = glm::vec2(_jsonDef["leftLeg"][0].asFloat(), _jsonDef["leftLeg"][1].asFloat());
	rightLeg = glm::vec2(_jsonDef["rightLeg"][0].asFloat(), _jsonDef["rightLeg"][1].asFloat());
	body = glm::vec2(_jsonDef["body"][0].asFloat(), _jsonDef["body"][1].asFloat());
	if(_jsonDef.isMember("translation")) {
		translation = glm::vec3(
			_jsonDef["translation"][0].asFloat(),
			_jsonDef["translation"][1].asFloat(),
			_jsonDef["translation"][2].asFloat()
			);
	}
}

PD_CharacterAnimationStep::PD_CharacterAnimationStep() :
	interpolation("LINEAR"),
	time(0.0f),
	leftArm(glm::vec2(0.f, 0.f)),
	rightArm(glm::vec2(0.f, 0.f)),
	leftLeg(glm::vec2(0.f, 0.f)),
	rightLeg(glm::vec2(0.f, 0.f)),
	body(glm::vec2(0.f, 0.f))
{
}