#pragma once

#include <Animation.h>
#include <node/NodeUpdatable.h>

class PersonRenderer;

class PD_CharacterAnimationSet : public NodeUpdatable{
public:	

	Animation<glm::vec2> leftArm;
	Animation<glm::vec2> rightArm;
	Animation<glm::vec2> leftLeg;
	Animation<glm::vec2> rightLeg;
	Animation<glm::vec2> body;

	explicit PD_CharacterAnimationSet(PersonRenderer * _pr);
	~PD_CharacterAnimationSet();

	virtual void update(Step * _step) override;
};