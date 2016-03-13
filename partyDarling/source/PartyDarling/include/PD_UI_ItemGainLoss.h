#pragma once

#include <sweet/UI.h>

class PD_Item;
class Timeout;

class PD_UI_ItemGainLoss : public VerticalLinearLayout{
private:
	VerticalLinearLayout * container;
	NodeUI * image;
	TextLabel * text;
	ComponentShaderText * textShader;
	Timeout * animationTimeout;
public:
	PD_UI_ItemGainLoss(BulletWorld * _world);
	~PD_UI_ItemGainLoss();

	void gainItem(PD_Item * _item);
	void loseItem(PD_Item * _item);
};