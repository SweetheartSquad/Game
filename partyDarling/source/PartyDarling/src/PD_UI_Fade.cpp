#pragma once

#include <PD_UI_Fade.h>
#include <Easing.h>

PD_UI_Fade::PD_UI_Fade(BulletWorld * _world) :
	NodeUI(_world),
	color(0)
{
	setBackgroundColour(0,0,0,0);

	fadeOutTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		setBackgroundColour(color.r, color.g, color.b, 1);
	});
	fadeOutTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		p = Easing::easeInOutCubic(p, 0, 1, 1);
		setBackgroundColour(color.r, color.g, color.b, p);
	});


	fadeInTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		setBackgroundColour(color.r, color.g, color.b, 0);
	});
	fadeInTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		p = Easing::easeInOutCubic(p, 0, 1, 1);
		setBackgroundColour(color.r, color.g, color.b, 1.f-p);
	});
	
	childTransform->addChild(fadeOutTimeout, false);
	childTransform->addChild(fadeInTimeout, false);
}

PD_UI_Fade::~PD_UI_Fade(){

}

void PD_UI_Fade::fadeIn(glm::uvec3 _color, float _length){
	color = glm::vec3(_color) / 255.f;

	fadeOutTimeout->stop();
	fadeInTimeout->targetSeconds = _length;
	fadeInTimeout->restart();
}
void PD_UI_Fade::fadeOut(glm::uvec3 _color, float _length){
	color = glm::vec3(_color) / 255.f;

	fadeInTimeout->stop();
	fadeOutTimeout->targetSeconds = _length;
	fadeOutTimeout->restart();
}