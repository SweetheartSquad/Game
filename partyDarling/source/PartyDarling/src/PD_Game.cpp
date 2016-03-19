#pragma once

#include <PD_Game.h>
#include <PD_Scene_Main.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_MenuMain.h>
#include <PD_Scene_LoadingScreen.h>
#include <PD_Scene_Animation.h>

#include <NumberUtils.h>
#include <OpenAlSound.h>
#include <AutoMusic.h>
#include <RenderOptions.h>
#include <PD_Scene_MenuOptions.h>
#include <RoomBuilder.h>

sweet::ShuffleVector<unsigned long> PD_Game::bgmTrackIdx;
OpenAL_Sound * PD_Game::bgmTrack = nullptr;
ProgressManager * PD_Game::progressManager = nullptr;
bool PD_Game::firstRun = true;

bool PD_Game::staticInit(){
	for(unsigned long int i = 1; i <= 5; ++i){
		bgmTrackIdx.push(i);
	}
	return true;
}
bool PD_Game::staticInitialized = staticInit();

void PD_Game::addSplashes(){
	//Game::addSplashes(); // add default splashes
}

PD_Game::PD_Game() :
	Game("menu", new PD_Scene_MenuMain(this), true)
{
	printFPS = false;

	scenes["loadingScreen"] = new PD_Scene_LoadingScreen(this);
	scenes["options"] = new PD_Scene_MenuOptions(this);

	progressManager = new ProgressManager();
	Log::warn("before RNG:\t" + std::to_string(sweet::NumberUtils::numRandCalls));
	if(sweet::FileUtils::fileExists("data/save.json")){
		PD_Game::progressManager->loadSave();
	}
	Log::warn("start RNG:\t" + std::to_string(sweet::NumberUtils::numRandCalls));

	playBGM();
}

PD_Game::~PD_Game(){
	if(bgmTrack != nullptr){
		bgmTrack->decrementAndDelete();
	}

	for(auto t : RoomBuilder::wallTex){
		t.second->decrementAndDelete();
	}for(auto t : RoomBuilder::ceilTex){
		t.second->decrementAndDelete();
	}for(auto t : RoomBuilder::floorTex){
		t.second->decrementAndDelete();
	}

	delete progressManager;
}

void PD_Game::update(Step * _step){
	if(bgmTrack != nullptr){
		bgmTrack->update(_step);
		if(bgmTrack->source->state != AL_PLAYING){
			playBGM();
		}
	}
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
	bgmTrack->play(false);
}

void PD_Game::showLoading(float _percentage){
	PD_Scene_LoadingScreen * s = dynamic_cast<PD_Scene_LoadingScreen *>(scenes["loadingScreen"]);
	s->updateProgress(_percentage);
	s->update(nullptr);
	draw(s);
}