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
#include <PD_Scene_MenuOptions.h>

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
	Game("menu", new PD_Scene_MainMenu(this), true),
	bgmTrack(nullptr)
{
	printFPS = false;


	scenes["loadingScreen"] = new PD_Scene_LoadingScreen(this);
	scenes["options"] = new PD_Scene_MenuOptions(this);

	//scenes["game"] = new PD_Scene_Main(this);

	playBGM();
}

PD_Game::~PD_Game(){
	bgmTrack->decrementAndDelete();
}

void PD_Game::update(Step * _step){
	Game::update(_step);
}

void PD_Game::playBGM(){
	if(bgmTrack != nullptr){
		bgmTrack->stop();
		bgmTrack->decrementAndDelete();
	}
	std::stringstream ss;
	ss << "BGM" << bgmTrackIdx.pop();

	bgmTrack = PD_ResourceManager::scenario->getAudio(ss.str())->sound;
	++bgmTrack->referenceCount;
	bgmTrack->play(true);
}

void PD_Game::showLoading(float _percentage){
	PD_Scene_LoadingScreen * s = dynamic_cast<PD_Scene_LoadingScreen *>(scenes["loadingScreen"]);
	s->updateProgress(_percentage);
	s->update(nullptr);
	draw(s);
}