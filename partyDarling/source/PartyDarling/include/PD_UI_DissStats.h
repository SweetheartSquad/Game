#pragma once

#include <sweet/UI.h>

class PD_UI_DissCard;
class Player;
class PD_Character;
class Timeout;

/**
* Events:
*	introComplete
*   outroComplete
*   changeDissStatComplete
*/
class PD_UI_DissStats : public NodeUI{
private:
public:
	// diss battle intro stuff
	Timeout * dissBattleStartTimeout;

	HorizontalLinearLayout * levelUpContainer;
	NodeUI * levelUp;

	Player * player;
	PD_Character * dissEnemy;
	PD_UI_DissCard * playerCard, * enemyCard;
	NodeUI * vs;

	float prevXP;
	float wonXP;

	Timeout * dissBattleXPGainTimeout;
	Timeout * dissBattleXPPause;
	Timeout * dissBattleLevelUpTimeout;
	Timeout * dissBattleChangeStatTimeout;

	PD_UI_DissStats(BulletWorld * _bulletWorld, Player * _player, Shader * _shader);
	~PD_UI_DissStats();
	
	void playIntro(PD_Character * _enemy);
	void playOutro(float _wonXP);
	void playChangeDissStat();
	
};