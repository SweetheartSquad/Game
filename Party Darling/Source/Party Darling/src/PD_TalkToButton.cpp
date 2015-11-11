#pragma once

#include <PD_TalkToButton.h>
#include <PD_TestScene.h>

PD_TalkToButton::PD_TalkToButton(Conversation * _convo, BulletWorld * _world, Scene * _scene) :
	PD_Button(_world, 5, 5),
	conversation(_convo),
	scene(_scene)
{
}

void PD_TalkToButton::click(){
	PD_Button::click();
	// start conversation
	Log::info("Should start conversation: " + conversation->id);

	((PD_TestScene *)scene)->dialogueDisplay->currentConversation = conversation;
	((PD_TestScene *)scene)->dialogueDisplay->sayNext();
}