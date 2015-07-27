#pragma once

#include <PD_Button.h>
#include <Font.h>
#include <shader\ComponentShaderBase.h>
#include <shader\ShaderComponentText.h>
#include <shader/ShaderComponentTexture.h>
#include <MeshFactory.h>

PD_Button::PD_Button(BulletWorld * _world, Scene * _scene, float _width, float _height) :
	NodeUI(_world, _scene, true)
{
	setWidth(_width);
	setHeight(_height);
}

void PD_Button::update(Step * _step){
	NodeUI::update(_step);
	if(isHovered){
		if(isDown){
			setBackgroundColour(0, -1, -1);
		}else{
			setBackgroundColour(-1, 0, -1);
		}
	}else{
		setBackgroundColour(-1, -1, 0);
	}
}