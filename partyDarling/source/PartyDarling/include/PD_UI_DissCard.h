#pragma once

#include <sweet/UI.h>

class Player;
class PD_Character;
class PD_DissStats;

class PD_UI_DissCard : public NodeUI_NineSliced{
private:
	void init();
public:
	PD_DissStats * dissStats;
	bool showSlider;
	
	float * const experience;
	std::string name;

	ComponentShaderText * textShader;
	NodeUI * stars[4][5];
	TextLabel * label;

	SliderControlled * slider;
	TextLabel * level;

	PD_UI_DissCard(BulletWorld * _world, Player * _player);
	PD_UI_DissCard(BulletWorld * _world);
	~PD_UI_DissCard();

	void updateStats();
	void setLevel(int _level);
	void setEnemy(PD_Character * _enemy);
};