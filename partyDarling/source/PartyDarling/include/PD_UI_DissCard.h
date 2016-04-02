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
	float * const level;
	std::string name;

	ComponentShaderText * textShader;
	NodeUI * stars[4][5];
	TextLabel * label;

	SliderControlled * slider;
	TextLabelControlled * levelLabel;

	int increments[4]; // for animating last star(s)

	PD_UI_DissCard(BulletWorld * _world, Player * _player);
	PD_UI_DissCard(BulletWorld * _world);
	~PD_UI_DissCard();

	void updateStats(bool _hideLostStats = true);
	void setEnemy(PD_Character * _enemy);
	void animateNewStats(float _p);
	void animateStar(int _idx, int _dissStat, int _delta, float _p);
};