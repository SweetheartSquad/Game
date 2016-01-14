#pragma once

#include <PD_Button.h>

#include <scenario/Scenario.h>

class PD_TalkToButton : public PD_Button{
public:
	Conversation * conversation;
	Scene * scene;
	PD_TalkToButton(Conversation * _convo, BulletWorld * _world, Scene * _scene);

	void click() override;
};