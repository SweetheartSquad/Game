#pragma once

#include <NodeUI.h>

class TextArea;
class Font;
class Shader;

class PD_Button : public NodeUI{
private:
	TextArea * text;
public:

	PD_Button(BulletWorld * _world, Scene * _scene, float _width, float _height, Font * _font = nullptr, Shader * _textShader = nullptr, std::wstring _text = L"");
	
	void update(Step * _step) override;

	void setText(std::wstring _text);
};