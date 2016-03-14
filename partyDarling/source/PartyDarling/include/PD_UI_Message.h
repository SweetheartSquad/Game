#pragma once

#include <sweet/UI.h>

class PD_Item;
class Timeout;

class PD_UI_Message : public VerticalLinearLayout{
private:
	NodeUI_NineSliced * textBubble;
	NodeUI * image;
	TextLabel * text;
	ComponentShaderText * textShader;

	std::vector<Timeout *> animationTimeouts;
public:
	PD_UI_Message(BulletWorld * _world);
	~PD_UI_Message();

	void gainItem(PD_Item * _item);
	void loseItem(PD_Item * _item);
	void gainLifeToken(std::string _namel, Texture * _tex);
	void displayMessage(std::string _message);

	void setItemTexture(PD_Item * _item);
	void animate(float _p);

	virtual void update(Step * _step) override;
};