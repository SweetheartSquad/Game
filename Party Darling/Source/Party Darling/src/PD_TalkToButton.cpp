#pragma once

#include <PD_TalkToButton.h>

PD_TalkToButton::PD_TalkToButton(Conversation * _convo, BulletWorld * _world, Scene * _scene) :
	PD_Button(_world, _scene, 5, 5),
	conversation(_convo)
{
}

void PD_TalkToButton::click(){
	PD_Button::click();
	// start conversation
	Log::info("Should start conversation: " + conversation->id);
}