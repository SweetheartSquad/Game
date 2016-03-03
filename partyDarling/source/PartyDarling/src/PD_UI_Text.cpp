#pragma once

#include "PD_UI_Text.h"

#include <shader/Shader.h>

PD_UI_Text::PD_UI_Text(BulletWorld* _bulletWorld, Font* _font, ComponentShaderText* _textShader) :
	TextArea(_bulletWorld, _font, _textShader),
	textColour(0, 0, 0),
	downColour(0, 0, 0),
	overColour(0, 0, 0),
	curTextColour(0, 0, 0),
	onClick(nullptr),
	onOver(nullptr),
	onDown(nullptr)
{
	eventManager->addEventListener("mousedown", [this](sweet::Event * _event){
		curTextColour = downColour;
		if(onDown != nullptr){
			onDown(_event);
		}
	});

	eventManager->addEventListener("mouseup", [this](sweet::Event * _event){
		curTextColour = overColour;
	});
	
	eventManager->addEventListener("mousein", [this](sweet::Event * _event){
		curTextColour = overColour;
		if(onOver != nullptr){
			onOver( _event);
		}
	});
	
	eventManager->addEventListener("mouseout", [this](sweet::Event * _event){
		curTextColour = textColour;
	});

	eventManager->addEventListener("click", [this](sweet::Event * _event){
		if(onClick != nullptr){
			onClick(_event);
		}
	});
}

void PD_UI_Text::setTextColour(float _r, float _g, float _b) {
	textColour = glm::vec3(_r, _g, _b);
}

void PD_UI_Text::setOverColour(float _r, float _g, float _b) {
	overColour = glm::vec3(_r, _g, _b);
}

glm::vec3 PD_UI_Text::getOverColour() const {
	return overColour;
}

void PD_UI_Text::setDownColour(float _r, float _g, float _b) {
	downColour = glm::vec3(_r, _g, _b);
}

glm::vec3 PD_UI_Text::getDownColour() const {
	return downColour;
}

glm::vec3 PD_UI_Text::getTextColour() const {
	return textColour;
}

void PD_UI_Text::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	
	glm::vec4 curCol = static_cast<ComponentShaderText *>(textShader)->textComponent->getColor();

	bool textColourDirty = abs(curCol.x - curTextColour.x) >= FLT_EPSILON ||  
		abs(curCol.y - curTextColour.y) >= FLT_EPSILON ||
		abs(curCol.z - curTextColour.z) >= FLT_EPSILON;

	if(textColourDirty){
		static_cast<ComponentShaderText*>(textShader)->setColor(curTextColour.r, curTextColour.g, curTextColour.b);
	}
	
	TextArea::render(_matrixStack,  _renderOptions);

	if(textColourDirty){
		static_cast<ComponentShaderText*>(textShader)->setColor(curCol.r, curCol.g, curCol.b);
	}
}
