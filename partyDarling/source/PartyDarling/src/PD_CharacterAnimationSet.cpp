#pragma once

#include <PD_CharacterAnimationSet.h>
#include <PD_Character.h>

PD_CharacterAnimationSet::PD_CharacterAnimationSet(PersonRenderer* _pr):
	leftArm(&_pr->solverArmL->target), 
	rightArm(&_pr->solverArmR->target), 
	leftLeg(&_pr->solverLegL->target), 
	rightLeg(&_pr->solverLegR->target), 
	body(&_pr->solverBod->target)
{
	leftArm.hasStart = true;
	rightArm.hasStart = true;
	leftLeg.hasStart = true;
	rightLeg.hasStart = true;
	body.hasStart = true;
}

PD_CharacterAnimationSet::~PD_CharacterAnimationSet() {
}

void PD_CharacterAnimationSet::update(Step* _step) {
	leftArm.update(_step);
	rightArm.update(_step);
	leftLeg.update(_step);
	rightLeg.update(_step);
	body.update(_step);
}
