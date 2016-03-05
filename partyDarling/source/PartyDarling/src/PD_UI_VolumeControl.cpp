#pragma once

#include <PD_UI_VolumeControl.h>
#include <PD_ResourceManager.h>
#include <OpenAlSound.h>

PD_UI_VolumeControl::PD_UI_VolumeControl(BulletWorld * _world, Shader * _textShader) :
	VerticalLinearLayout(_world),
	textShader(_textShader)
{
	verticalAlignment = kMIDDLE;

	addSlider("master", &OpenAL_Sound::masterGain);
	for(auto & s : OpenAL_Sound::categoricalGain){
		addSlider(s.first, &s.second);
	}
}

void PD_UI_VolumeControl::addSlider(std::string _text, float * _target){
	TextLabel * label = new TextLabel(world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
	SliderController * slider = new SliderController(world, _target, *_target, 0.f, 2.f);

	addChild(label);
	addChild(slider);
	
	label->setText(_text);
	slider->setRationalWidth(1.f, this);
	slider->setHeight(25);

	slider->thumb->background->meshTransform->scale(4)->translate(glm::vec3(-1,0,0));
	slider->thumb->setBackgroundColour(1,1,1,1);
	slider->thumb->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SLIDER-THUMB")->texture);
	slider->thumb->background->mesh->setScaleMode(GL_NEAREST);
	
	slider->setBackgroundColour(1,1,1,1);
	slider->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SLIDER-TRACK")->texture);
	slider->background->mesh->setScaleMode(GL_NEAREST);
	
	slider->fill->setBackgroundColour(1,1,1,1);
	slider->fill->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SLIDER-FILL")->texture);
	slider->fill->background->mesh->setScaleMode(GL_NEAREST);
}