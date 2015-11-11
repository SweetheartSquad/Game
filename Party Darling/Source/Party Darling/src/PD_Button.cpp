#pragma once

#include <PD_Button.h>
#include <Font.h>
#include <shader\ComponentShaderBase.h>
#include <shader\ShaderComponentText.h>
#include <shader/ShaderComponentTexture.h>
#include <MeshFactory.h>
#include <sweet/UI.h>

PD_Button::PD_Button(BulletWorld * _world, float _width, float _height, Font * _font, Shader * _textShader, std::wstring _text) :
	NodeUI(_world, kENTITIES, true),
	text(new TextArea(_world, _font, _textShader, _width, _height))
{
	setWidth(_width);
	setHeight(_height);

	text->setWidth(1.0);
	text->setHeight(1.0);
	text->horizontalAlignment = kCENTER;
	text->verticalAlignment = kMIDDLE;
	addChild(text);
}

void PD_Button::setText(std::wstring _text){
	if (text->font && text->textShader){
		text->setText(_text);
	}
}

void PD_Button::update(Step * _step){
	NodeUI::update(_step);
	if(isHovered){
		if(isDown){
			//setBackgroundColour(0.5, 0.5, 0.5);
		}else{
			//setBackgroundColour(2, 2, 2);
		}
	}else{
		//setBackgroundColour(1, 1, 1);
	}
}