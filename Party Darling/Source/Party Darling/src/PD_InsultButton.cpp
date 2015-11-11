#pragma once

#include <PD_InsultButton.h>
#include <PD_Scene_YellingContestTest.h>

PD_InsultButton::PD_InsultButton(BulletWorld * _world, Font * _font, Shader * _textShader, std::wstring _text) :
	PD_Button(_world, 5, 5, _font, _textShader, _text),
	isEffective(false)
{
}

void PD_InsultButton::click(){
	PD_Button::click();
	// start conversation
	Log::info("Click!!!");
}