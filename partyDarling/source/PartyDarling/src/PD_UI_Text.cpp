#pragma once

#include "PD_UI_Text.h"
#include "PD_ResourceManager.h"

#include <shader/Shader.h>

PD_UI_Text::PD_UI_Text(BulletWorld* _bulletWorld, Font* _font, ComponentShaderText* _textShader) :
	TextArea(_bulletWorld, _font, _textShader),
	textColour(0, 0, 0),
	downColour(0, 0, 0),
	overColour(0, 0, 0),
	curTextColour(0, 0, 0),
	enabled(true)
{
	eventManager->addEventListener("mousedown", [this](sweet::Event * _event){
		curTextColour = downColour;
	});

	eventManager->addEventListener("mouseup", [this](sweet::Event * _event){
		curTextColour = overColour;
	});

	eventManager->addEventListener("mousein", [this](sweet::Event * _event){
		curTextColour = overColour;
		PD_ResourceManager::scenario->getAudio("hover")->sound->play();
	});

	eventManager->addEventListener("mouseout", [this](sweet::Event * _event){
		curTextColour = textColour;
	});

	eventManager->addEventListener("click", [this](sweet::Event * _event){
		PD_ResourceManager::scenario->getAudio("click")->sound->play();
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

void PD_UI_Text::enable(){
	enabled = true;
	setMouseEnabled(true);
}

void PD_UI_Text::disable(){
	enabled = false;
	setMouseEnabled(false);
	curTextColour = textColour;
}

bool PD_UI_Text::isEnabled(){
	return enabled;
}

void PD_UI_Text::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	glm::vec4 curCol = static_cast<ComponentShaderText *>(textShader)->textComponent->getColor();

	bool textColourDirty = abs(curCol.x - curTextColour.x) >= FLT_EPSILON ||
		abs(curCol.y - curTextColour.y) >= FLT_EPSILON ||
		abs(curCol.z - curTextColour.z) >= FLT_EPSILON ||
		abs(curCol.a - enabled ? 1.f : 0.5f >= FLT_EPSILON);

	if(textColourDirty){
		static_cast<ComponentShaderText*>(textShader)->setColor(curTextColour.r, curTextColour.g, curTextColour.b, enabled ? 1.f : 0.5f);
	}

	TextArea::render(_matrixStack,  _renderOptions);

	if(textColourDirty){
		static_cast<ComponentShaderText*>(textShader)->setColor(curCol.r, curCol.g, curCol.b, enabled ? 1.f : 0.5f);
	}
}