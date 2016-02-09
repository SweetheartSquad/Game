#pragma once

#include <NodeUI.h>

class TextArea;
class Font;
class Shader;

class PD_Button : public NodeUI{
private:
	TextArea * text;
public:
	Scene * scene;

	PD_Button(BulletWorld * _world, Font * _font = nullptr, Shader * _textShader = nullptr, std::wstring _text = L"");
	
	void update(Step * _step) override;
	
	void setText(std::wstring _text);
	void setText(std::string _text);
};