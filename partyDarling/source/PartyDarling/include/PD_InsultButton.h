#pragma once

#include <sweet/UI.h>

class PD_InsultButton : public NodeUI_NineSliced{
public:
	TextLabel * label;
	bool isEffective;

	PD_InsultButton(BulletWorld * _world, Font * _font = nullptr, Shader * _textShader = nullptr, std::wstring _text = L"");
};