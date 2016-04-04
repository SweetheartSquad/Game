#pragma once

#include <PD_InsultButton.h>
#include <PD_ResourceManager.h>

PD_InsultButton::PD_InsultButton(BulletWorld * _world, Font * _font, Shader * _textShader, std::wstring _text) :
	NodeUI_NineSliced(_world, PD_ResourceManager::scenario->getNineSlicedTexture("PLAYER-BUTTON")),
	isEffective(false)
{
	setRenderMode(kTEXTURE);
	setBorder(_font->getLineHeight() * 0.7f);

	VerticalLinearLayout * layout = new VerticalLinearLayout(_world);
	addChild(layout);
	layout->setRationalWidth(1.f, layout->nodeUIParent);
	layout->setRationalHeight(1.f, layout->nodeUIParent);
	layout->horizontalAlignment = kCENTER;
	layout->verticalAlignment = kMIDDLE;

	label = new TextLabel(world, _font, _textShader);
	layout->addChild(label);
	label->setRationalWidth(1.f, this);
	label->setPixelHeight(_font->getLineHeight());
	label->horizontalAlignment = kCENTER;
	label->verticalAlignment = kMIDDLE;
}