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

	std::vector<Timeout *> animationTimeouts;
public:
	PD_UI_ItemGainLoss(BulletWorld * _world);
	~PD_UI_ItemGainLoss();

	void gainItem(PD_Item * _item);
	void loseItem(PD_Item * _item);
	void displayMessage(std::string _message);

	void animate(float _p);

	virtual void update(Step * _step) override;
};