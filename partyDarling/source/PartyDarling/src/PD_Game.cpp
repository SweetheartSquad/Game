#pragma once

#include <PD_Game.h>
#include <PD_TestScene.h>
#include <PD_Scene_YellingContestTest.h>
#include <PD_Scene_FurnitureTest.h>
#include <PD_Scene_InteractionTest.h>
#include <PD_Scene_CombinedTests.h>
#include <PD_Scene_RoomGenerationTest.h>
#include <PD_Nav.h>
#include <PD_ResourceManager.h>

#include <NumberUtils.h>
#include <OpenAlSound.h>
#include <AutoMusic.h>
#include <PD_Scene_MainMenu.h>


void PD_Game::addSplashes(){
	//Game::addSplashes(); // add default splashes
}

PD_Game::PD_Game() :
	Game("test", new PD_Scene_MainMenu(this), true),
	bgmTrack(nullptr),
	fightTrack(nullptr)
{
	printFPS = false;

	scenes["game"] = new PD_Scene_CombinedTests(this);

	playBGM();
}

PD_Game::~PD_Game(){
	delete bgmTrack;
	delete fightTrack;
}

void PD_Game::update(Step * _step){
	if(fightTrack != nullptr){
		fightTrack->update(_step);
	}

	Game::update(_step);
}

void PD_Game::playBGM(){
	if(bgmTrack != nullptr){
		bgmTrack->stop();
	}if(fightTrack != nullptr){
		delete fightTrack;
		fightTrack = nullptr;
	}
	std::stringstream ss;
	ss << "BGM" << sweet::NumberUtils::randomInt(1, 2);

	bgmTrack = PD_ResourceManager::scenario->getAudio(ss.str())->sound;
	bgmTrack->play(true);
}

void PD_Game::playFight(){
	if(bgmTrack != nullptr){
		bgmTrack->stop();
	}if(fightTrack != nullptr){
		delete fightTrack;
		fightTrack = nullptr;
	}
	// TODO: implement generative song here
	fightTrack = new AutoDrums(PD_ResourceManager::scenario->getAudio("PLAYER_JUMP")->sound, PD_ResourceManager::scenario->getAudio("PLAYER_FALL")->sound, PD_ResourceManager::scenario->getAudio("PLAYER_FOOTSTEP")->sound);
	fightTrack->bpm = 128;
	fightTrack->generate();
}