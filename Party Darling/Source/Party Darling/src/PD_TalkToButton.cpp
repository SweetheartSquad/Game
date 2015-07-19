#pragma once

#include <PD_TalkToButton.h>

PD_TalkToButton::PD_TalkToButton(Conversation * _convo, BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader, float _width) :
	PD_Button(_world, _scene, _font, _textShader, _width),
	conversation(_convo),
	NodeBulletBody(_world)
{
}

void PD_TalkToButton::up(){
	PD_Button::up();
	if(isHovered){
		// start conversation
		Log::info("Should start conversation: " + conversation->id);
	}
}