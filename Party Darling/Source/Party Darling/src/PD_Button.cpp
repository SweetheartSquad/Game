#pragma once

#include <PD_Button.h>
#include <Font.h>
#include <shader\ComponentShaderBase.h>
#include <shader\ShaderComponentText.h>
#include <shader/ShaderComponentTexture.h>
#include <MeshFactory.h>

PD_Button::PD_Button(BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader, float _width) :
	TextArea(_world, _scene, _font, _textShader, _width, BUTTON_SIZE),
	NodeBulletBody(_world),
	Entity(),
	normalLabel(L"normal"),
	downLabel(L"down"),
	overLabel(L"over")
{
	setText(normalLabel);
}

void PD_Button::update(Step * _step){
	TextArea::update(_step);

	/*if(isHovered){
		if(isDown){
			setText(downLabel);
		}else{
			setText(overLabel);
		}
	}else{
		setText(normalLabel);
	}*/
}