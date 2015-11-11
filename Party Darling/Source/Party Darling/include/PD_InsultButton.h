#pragma once

#include <PD_Button.h>

class PD_InsultButton : public PD_Button{
public:
	bool isEffective;

	PD_InsultButton(BulletWorld * _world, Font * _font = nullptr, Shader * _textShader = nullptr, std::wstring _text = L"");

	void click() override;
};