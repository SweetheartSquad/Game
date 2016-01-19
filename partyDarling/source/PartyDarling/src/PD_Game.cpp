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


void PD_Game::addSplashes(){
	//Game::addSplashes(); // add default splashes
}

PD_Game::PD_Game() :
	Game("test", new PD_Scene_CombinedTests(this), true),
	bgmTrack(nullptr)
{
	printFPS = false;
}

PD_Game::~PD_Game(){

}

void PD_Game::playBGM(){
	if(bgmTrack != nullptr){
		bgmTrack->stop();
	}
	std::stringstream ss;
	ss << "BGM" << sweet::NumberUtils::randomInt(1, 2);

	bgmTrack = PD_ResourceManager::scenario->getAudio(ss.str())->sound;
	bgmTrack->play(true);
}

void PD_Game::playFight(){
	// TODO: implement generative song here
}