#pragma once

#include <Animation.h>
#include <node/NodeUpdatable.h>

class CharacterRenderer;

class PD_CharacterAnimationSet : public NodeUpdatable{
public:

	glm::vec3 translationVec;

	Animation <glm::vec2> * leftArm;
	Animation <glm::vec2> * rightArm;
	Animation <glm::vec2> * leftLeg;
	Animation <glm::vec2> * rightLeg;
	Animation <glm::vec2> * body;
	Animation <glm::vec3> * translation;

	explicit PD_CharacterAnimationSet(CharacterRenderer * _pr);
	~PD_CharacterAnimationSet();

	virtual void update(Step * _step) override;
};