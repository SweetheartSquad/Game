#pragma once

#include <PD_TalkToButton.h>

PD_TalkToButton::PD_TalkToButton(Conversation * _convo, BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader, float _width) :
	PD_Button(_world, _scene, _font, _textShader, _width),
	conversation(_convo),
	NodeBulletBody(_world)
{
}

void PD_TalkToButton::click(){
	PD_Button::click();
	// start conversation
	Log::info("Should start conversation: " + conversation->id);
}