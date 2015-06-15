#pragma once

#include <NodeUI.h>
#include <TextLabel.h>

class PD_Button : public NodeUI{
public:

	TextLabel * normalLabel;
	TextLabel * downLabel;
	TextLabel * overLabel;

	PD_Button(BulletWorld * _world, Scene * _scene);
	
	void update(Step * _step) override;
};