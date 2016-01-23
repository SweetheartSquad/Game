#pragma once

#include <PD_UI_VolumeControl.h>
#include <PD_ResourceManager.h>
#include <OpenAlSound.h>

PD_UI_VolumeControl::PD_UI_VolumeControl(BulletWorld * _world, Shader * _textShader) :
	VerticalLinearLayout(_world),
	textShader(_textShader)
{
	setRationalWidth(1.f);
	setRationalHeight(1.f);
	verticalAlignment = kMIDDLE;

	addSlider("master", &OpenAL_Sound::masterGain);
	for(auto & s : OpenAL_Sound::categoricalGain){
		addSlider(s.first, &s.second);
	}
}

void PD_UI_VolumeControl::addSlider(std::string _text, float * _target){
	TextLabel * label = new TextLabel(world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
	SliderController * slider = new SliderController(world, _target, 1.f, 0.f, 2.f);

	addChild(label);
	addChild(slider);
	
	label->setText(_text);
	slider->setRationalWidth(1.f, this);
	slider->setHeight(25);

	slider->thumb->background->meshTransform->scale(2);
	slider->thumb->setBackgroundColour(1,1,1,1);
	slider->thumb->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SLIDER-THUMB")->texture);
	
	slider->setBackgroundColour(1,1,1,1);
	slider->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SLIDER-TRACK")->texture);
	
	slider->fill->setBackgroundColour(1,1,1,1);
	slider->fill->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SLIDER-FILL")->texture);
}