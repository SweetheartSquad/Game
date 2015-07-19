#pragma once

#include <NodeUI.h>

class PD_Button : public NodeUI{
public:
	PD_Button(BulletWorld * _world, Scene * _scene, float _width, float _height);
	
	void update(Step * _step) override;
};