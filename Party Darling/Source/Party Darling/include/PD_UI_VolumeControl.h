#pragma once

#include <sweet/UI.h>

class PD_UI_VolumeControl : public VerticalLinearLayout{
private:
	Shader * textShader;
public:
	PD_UI_VolumeControl(BulletWorld * _world, Shader * _textShader);

	void addSlider(std::string _text, float * _target);
};