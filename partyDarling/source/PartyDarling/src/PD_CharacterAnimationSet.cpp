#pragma once

#include <PD_CharacterAnimationSet.h>
#include <PD_Character.h>

PD_CharacterAnimationSet::PD_CharacterAnimationSet(CharacterRenderer* _pr):
	leftArm(new Animation<glm::vec2>(&_pr->solverArmL->target)), 
	rightArm(new Animation<glm::vec2>(&_pr->solverArmR->target)), 
	leftLeg(new Animation<glm::vec2>(&_pr->solverLegL->target)), 
	rightLeg(new Animation<glm::vec2>(&_pr->solverLegR->target)), 
	body(new Animation<glm::vec2>(&_pr->solverBod->target)),
	translation(new Animation<glm::vec3>(&translationVec))
{
	leftArm->hasStart = true;
	rightArm->hasStart = true;
	leftLeg->hasStart = true;
	rightLeg->hasStart = true;
	body->hasStart = true;
	translation->hasStart = true;

	leftArm->startValue = glm::vec2();
	rightArm->startValue = glm::vec2();
	leftLeg->startValue = glm::vec2();
	rightLeg->startValue = glm::vec2();
	body->startValue = glm::vec2();
	translation->startValue = glm::vec3();
}

PD_CharacterAnimationSet::~PD_CharacterAnimationSet() {
	delete leftArm;
	delete rightArm;
	delete leftLeg;
	delete rightLeg;
	delete body;
}

void PD_CharacterAnimationSet::update(Step* _step) {
	leftArm->update(_step);
	rightArm->update(_step);
	leftLeg->update(_step);
	rightLeg->update(_step);
	body->update(_step);
	translation->update(_step);
}