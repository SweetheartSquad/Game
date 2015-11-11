#pragma once

#include <PD_UI_YellingContest.h>
#include <sweet/UI.h>

class Shader;
class PD_InsultButton;
class Keyboard;

class PD_UI_YellingContest : public VerticalLinearLayout{
public:
	HorizontalLinearLayout * enemyBubble;
	TextArea * enemyBubbleText;

	HorizontalLinearLayout * playerBubble;
	TextArea * playerBubbleText;
	
	PD_InsultButton * pBubbleBtn1;
	PD_InsultButton * pBubbleBtn2;

	Keyboard * keyboard;

	bool modeOffensive;

	PD_UI_YellingContest(BulletWorld * _bulletWorld, Font * _font, Shader * _textShader);
	void setEnemyText();
	void setPlayerText();

	virtual void update(Step * _step) override;

	void setUIMode(bool _isOffensive);
	void interject();
	void insult(bool _isEffective);
};