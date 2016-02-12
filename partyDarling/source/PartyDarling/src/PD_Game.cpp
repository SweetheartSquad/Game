#pragma once

#include <PD_Game.h>
#include <PD_Scene_Main.h>
#include <PD_Scene_RoomGenerationTest.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_MainMenu.h>
#include <PD_Scene_LoadingScreen.h>
#include <PD_Scene_Animation.h>

#include <NumberUtils.h>
#include <OpenAlSound.h>
#include <AutoMusic.h>
#include <RenderOptions.h>

sweet::ShuffleVector<unsigned long> PD_Game::bgmTrackIdx;
bool PD_Game::staticInit(){
	bgmTrackIdx.push(1);
	bgmTrackIdx.push(2);
	return true;
}
bool PD_Game::staticInitialized = staticInit();

void PD_Game::addSplashes(){
	//Game::addSplashes(); // add default splashes
}

PD_Game::PD_Game() :
	Game("menu", new PD_Scene_Animation(this), true),
	bgmTrack(nullptr),
	fightTrack(nullptr)
{
	printFPS = false;

<<<<<<< HEAD
=======
	scenes["loadingScreen"] = new PD_Scene_LoadingScreen(this);
>>>>>>> origin/master
	//scenes["game"] = new PD_Scene_Main(this);

	//playBGM();
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
	ss << "BGM" << bgmTrackIdx.pop();

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

void PD_Game::showLoading(std::string _message, float _percentage){
	PD_Scene_LoadingScreen * s = dynamic_cast<PD_Scene_LoadingScreen *>(scenes["loadingScreen"]);
	s->loadingMessage->setText(_message);
	s->uiLayer.invalidateLayout();
	s->update(nullptr);
	draw(s);
}