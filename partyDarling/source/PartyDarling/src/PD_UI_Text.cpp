#pragma once

#include "PD_UI_Text.h"

#include <shader/Shader.h>

PD_UI_Text::PD_UI_Text(BulletWorld* _bulletWorld, Font* _font, ComponentShaderText* _textShader, float width, float height) :
	TextArea(_bulletWorld, _font, _textShader, width, height),
	textColour(0, 0, 0),
	downColour(0, 0, 0),
	overColour(0, 0, 0),
	curTextColour(0, 0, 0),
	textColourDirty(true),
	onClick(nullptr),
	onOver(nullptr),
	onDown(nullptr)
{
	eventManager.addEventListener("mousedown", [this](sweet::Event * _event){
		curTextColour = downColour;
		textColourDirty = true;
		if(onDown != nullptr){
			onDown(_event);
		}
	});
	
	eventManager.addEventListener("mousein", [this](sweet::Event * _event){
		curTextColour = overColour;
		textColourDirty = true;
		if(onOver != nullptr){
			onOver( _event);
		}
	});
	
	eventManager.addEventListener("mouseout", [this](sweet::Event * _event){
		curTextColour = textColour;
		textColourDirty = true;
	});

	eventManager.addEventListener("click", [this](sweet::Event * _event){
		onClick(_event);
	});
}

void PD_UI_Text::setTextColour(float _r, float _g, float _b) {
	textColour = glm::vec3(_r, _g, _b);
	textColourDirty = true;
}

void PD_UI_Text::setOverColour(float _r, float _g, float _b) {
	overColour = glm::vec3(_r, _g, _b);
	textColourDirty = true;
}

glm::vec3 PD_UI_Text::getOverColour() const {
	return overColour;
}

void PD_UI_Text::setDownColour(float _r, float _g, float _b) {
	downColour = glm::vec3(_r, _g, _b);
	textColourDirty = true;
}

glm::vec3 PD_UI_Text::getDownColour() const {
	return downColour;
}

glm::vec3 PD_UI_Text::getTextColour() const {
	return textColour;
}

void PD_UI_Text::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	
	if(textColourDirty){
		static_cast<ComponentShaderText*>(textShader)->setColor(curTextColour.r, curTextColour.g, curTextColour.b);
		textColourDirty = false;
	}
	
	TextArea::render(_matrixStack,  _renderOptions);
}
