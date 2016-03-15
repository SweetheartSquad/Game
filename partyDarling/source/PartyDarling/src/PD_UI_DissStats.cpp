#pragma once

#include <PD_UI_DissStats.h>

#include <Player.h>
#include <PD_Character.h>
#include <PD_UI_DissCard.h>
#include <Sprite.h>
#include <Easing.h>
#include <PD_ResourceManager.h>
#include <PD_DissStats.h>

#define LEVEL_UP_DURATION 2
#define XP_GAIN_PAUSE 1
#define CHANGE_STAT_DURATION 1

PD_UI_DissStats::PD_UI_DissStats(BulletWorld* _bulletWorld, Player * _player, Shader * _shader) :
	NodeUI(_bulletWorld),
	player(_player),
	dissEnemy(nullptr),
	prevXP(0.f),
	wonXP(0.f)
{
	background->setVisible(false);

	HorizontalLinearLayout * dissBattleCards = new HorizontalLinearLayout(_bulletWorld);
	addChild(dissBattleCards);
	dissBattleCards->setRationalHeight(1.f, this);
	dissBattleCards->setRationalWidth(1.f, this);
	dissBattleCards->setMarginLeft(0.3f);
	dissBattleCards->horizontalAlignment = kCENTER;
	dissBattleCards->verticalAlignment = kMIDDLE;

	playerCard = new PD_UI_DissCard(_bulletWorld, player);
	dissBattleCards->addChild(playerCard);
	playerCard->setRationalHeight(0.3f, dissBattleCards);
	playerCard->setSquareWidth(1.4f);

	vs = new NodeUI(_bulletWorld);
	dissBattleCards->addChild(vs);
	vs->setRationalWidth(0.3f, dissBattleCards);
	vs->setSquareHeight(1.f);
	vs->background->mesh->setScaleMode(GL_NEAREST);
	vs->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-VS")->texture);

	enemyCard = new PD_UI_DissCard(_bulletWorld);
	dissBattleCards->addChild(enemyCard);
	enemyCard->setRationalHeight(0.3f, dissBattleCards);
	enemyCard->setSquareWidth(1.4f);

	levelUpContainer = new HorizontalLinearLayout(_bulletWorld);
	addChild(levelUpContainer);
	levelUpContainer->setRationalWidth(1.f, this);
	levelUpContainer->setRationalHeight(1.f, this);
	levelUpContainer->horizontalAlignment = kCENTER;
	levelUpContainer->verticalAlignment = kMIDDLE;
	levelUpContainer->setVisible(false);

	levelUp = new NodeUI(_bulletWorld);
	levelUpContainer->addChild(levelUp);
	levelUp->setRationalHeight(1.f, levelUpContainer);
	levelUp->setSquareWidth(1.f);
	levelUp->background->mesh->setScaleMode(GL_NEAREST);
	levelUp->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-LEVEL-UP")->texture);

	dissBattleStartTimeout = new Timeout(3, [this](sweet::Event * _event){
		setVisible(false);
		eventManager->triggerEvent("introComplete");
	});
	dissBattleStartTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		if(p < 0.25){
			playerCard->firstParent()->scale(Easing::easeOutBounce(p, 0, 1, 0.33), false);
			vs->firstParent()->scale(0, false);
			enemyCard->firstParent()->scale(0, false);
			if(!playerCard->isVisible()){
				playerCard->setVisible(true);
				vs->setVisible(false);
				enemyCard->setVisible(false);
				PD_ResourceManager::scenario->getAudio("DISS-BATTLE-INTRO")->sound->play();
			}
		}else if(p < 0.5){
			playerCard->firstParent()->scale(1, false);
			vs->firstParent()->scale(0, false);
			enemyCard->firstParent()->scale(Easing::easeOutBounce(p-0.25, 0, 1, 0.33), false);
			if(!enemyCard->isVisible()){
				playerCard->setVisible(true);
				vs->setVisible(false);
				enemyCard->setVisible(true);
				PD_ResourceManager::scenario->getAudio("DISS-BATTLE-INTRO")->sound->play();
			}
		}else if(p < 0.75){
			playerCard->firstParent()->scale(1, false);
			vs->firstParent()->scale(Easing::easeOutBounce(p-0.5, 0, 1, 0.33), false);
			enemyCard->firstParent()->scale(1, false);
			if(!vs->isVisible()){
				playerCard->setVisible(true);
				vs->setVisible(true);
				enemyCard->setVisible(true);
				PD_ResourceManager::scenario->getAudio("DISS-BATTLE-INTRO")->sound->play();
			}
		}else{
			playerCard->firstParent()->scale(1, false);
			vs->firstParent()->scale(1, false);
			enemyCard->firstParent()->scale(1, false);
			playerCard->setVisible(true);
			vs->setVisible(true);
			enemyCard->setVisible(true);
		}
	});
	childTransform->addChild(dissBattleStartTimeout, false);

	dissBattleXPGainTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		if(player->experience >= 100){
			PD_ResourceManager::scenario->getAudio("LEVEL_UP")->sound->play();

			// LEVEL UP
			player->dissStats->setLastStats(); // so we get accurate increments

			player->dissStats->incrementDefense();
			player->dissStats->incrementInsight();
			player->dissStats->incrementSass();
			player->dissStats->incrementStrength();
			playerCard->updateStats(false);
			playerCard->animateNewStats(0.f);
			invalidateLayout();
			autoResize();
			++player->level;
			playerCard->setLevel(player->level);

			levelUpContainer->setVisible(true);
			levelUp->setRationalHeight(0.f, levelUpContainer);
			levelUp->setSquareWidth(1.f);
			invalidateLayout();

			dissBattleLevelUpTimeout->restart();
		}else{
			// NORMAL
			dissBattleXPPause->restart();
		}
	});

	dissBattleXPGainTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");

		player->experience = prevXP + p * wonXP;
	});
	childTransform->addChild(dissBattleXPGainTimeout, false);

	dissBattleXPPause = new Timeout(1.f, [this](sweet::Event * _event){
		setVisible(false);
		eventManager->triggerEvent("outroComplete");
	});
	childTransform->addChild(dissBattleXPPause, false);

	dissBattleLevelUpTimeout = new Timeout(LEVEL_UP_DURATION, [this](sweet::Event * _event){
		// end and this
		levelUpContainer->setVisible(false);
		player->experience = 0.f; // just in case
		setVisible(false);

		dissEnemy = nullptr;
		playerCard->updateStats(); // actually hide stats star
		eventManager->triggerEvent("outroComplete");
	});
	dissBattleLevelUpTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		if(p <= 0.1f){
			player->experience = 100 * (1-p/0.1f);
		}

		float size;
		if(p <= 0.5f){
			size = Easing::easeOutBounce(p * LEVEL_UP_DURATION, 0, 1, LEVEL_UP_DURATION * 0.5f);
			levelUp->setRationalHeight(size, levelUpContainer);
		}else if(p >= 0.8f){
			size = Easing::easeInCubic((p - 0.8f) * LEVEL_UP_DURATION, 1, -1, LEVEL_UP_DURATION * 0.2f);
			levelUp->setRationalHeight(size, levelUpContainer);
		}

		if(LEVEL_UP_DURATION * p <= CHANGE_STAT_DURATION){
			float statP = LEVEL_UP_DURATION * p / CHANGE_STAT_DURATION;
			playerCard->animateNewStats(statP);
		}

		invalidateLayout();
	});
	childTransform->addChild(dissBattleLevelUpTimeout, false);

	dissBattleChangeStatTimeout = new Timeout(1.5f, [this](sweet::Event * _event){
		// end and this
		enemyCard->setVisible(true);
		vs->setVisible(true);
		setVisible(false);
		playerCard->updateStats(); // actually hide stats star

		eventManager->triggerEvent("changeDissStatComplete");
	});
	dissBattleChangeStatTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");

		if(1.5f * p <= CHANGE_STAT_DURATION){
			float statP = 1.5f * p / CHANGE_STAT_DURATION;
			playerCard->animateNewStats(statP);
		}
	});
	childTransform->addChild(dissBattleChangeStatTimeout, false);
}

PD_UI_DissStats::~PD_UI_DissStats(){
}

void PD_UI_DissStats::playIntro(PD_Character * _enemy){
	setVisible(true);
	dissBattleStartTimeout->restart();
	enemyCard->setEnemy(_enemy);

	playerCard->setVisible(false);
	vs->setVisible(false);
	enemyCard->setVisible(false);

	playerCard->childTransform->translate(glm::vec3(-playerCard->getWidth(true,false)*0.5f, -playerCard->getHeight(true,false)*0.5f, 0), false);
	vs->childTransform->translate(glm::vec3(-vs->getWidth(true,false)*0.5f, -vs->getHeight(true,false)*0.5f, 0), false);
	enemyCard->childTransform->translate(glm::vec3(-enemyCard->getWidth(true,false)*0.5f, -enemyCard->getHeight(true,false)*0.5f, 0), false);
}

void PD_UI_DissStats::playOutro(float _wonXP){
	prevXP = player->experience;
	wonXP = _wonXP;
	setVisible(true);
	if(wonXP > 0){
		PD_ResourceManager::scenario->getAudio("XP_UP")->sound->play();
	}
	dissBattleXPGainTimeout->restart();
}

void PD_UI_DissStats::playChangeDissStat(){
	setVisible(true);
	enemyCard->setVisible(false);
	vs->setVisible(false);
	playerCard->updateStats(false);
	playerCard->animateNewStats(0.f);

	dissBattleChangeStatTimeout->restart();
}