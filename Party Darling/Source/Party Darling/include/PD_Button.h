#pragma once

#include <NodeUI.h>
#include <Label.h>

class PD_Button : public NodeUI{
public:

	Label * normalLabel;
	Label * downLabel;
	Label * overLabel;

	PD_Button(BulletWorld * _world, Scene * _scene);
	
	void update(Step * _step) override;
};