#pragma once

#include <PD_Button.h>

#include <scenario/Scenario.h>

class PD_TalkToButton : public PD_Button{
public:
	Conversation * conversation;

	PD_TalkToButton(Conversation * _convo, BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader, float _width);

	void click() override;
};