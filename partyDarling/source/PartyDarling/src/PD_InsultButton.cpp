#pragma once

#include <PD_InsultButton.h>
#include <PD_ResourceManager.h>

PD_InsultButton::PD_InsultButton(BulletWorld * _world, Font * _font, Shader * _textShader, std::wstring _text) :
	NodeUI_NineSliced(_world, PD_ResourceManager::scenario->getNineSlicedTexture("YELLING-CONTEST-OFFENSE-CHOICE")),
	isEffective(false)
{
	setBorder(20.f);

	label = new TextLabel(world, _font, _textShader);
	label->horizontalAlignment = kCENTER;
	label->verticalAlignment = kMIDDLE;
	label->setRationalWidth(1.f, this);
	label->setRationalHeight(1.f, this);

	addChild(label);
}