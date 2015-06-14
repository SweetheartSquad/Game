#pragma once

#include <NodeUI.h>
#include <LabelV2.h>

class PD_Button : public NodeUI{
public:

	LabelV2 * normalLabel;
	LabelV2 * downLabel;
	LabelV2 * overLabel;

	PD_Button(BulletWorld * _world, Scene * _scene);
	
	void update(Step * _step) override;
};