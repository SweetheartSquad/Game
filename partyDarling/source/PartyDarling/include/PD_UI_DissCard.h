#pragma once

#include <sweet/UI.h>

class Player;
class PD_Character;

class PD_UI_DissCard : public NodeUI_NineSliced{
private:
	void init();
public:
	int defense, insight, strength, sass;
	bool showSlider;
	
	float * const experience;
	std::string name;

	ComponentShaderText * textShader;

	PD_UI_DissCard(BulletWorld * _world, Player * _player);
	PD_UI_DissCard(BulletWorld * _world, PD_Character * _enemy);
	~PD_UI_DissCard();
};