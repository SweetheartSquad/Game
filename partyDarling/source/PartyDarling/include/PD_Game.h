#pragma once

#include <Game.h>
#include <NumberUtils.h>
class OpenAL_Sound;
class AutoDrums;

class PD_Game : public Game{
private:
	static sweet::ShuffleVector<unsigned long int> bgmTrackIdx;
	static bool staticInit();
	static bool staticInitialized;
public:
	virtual void addSplashes() override;

	PD_Game();
	~PD_Game();


	static OpenAL_Sound * bgmTrack;

	// picks a random BGM track and plays it, replacing the current track
	void playBGM();

	
	virtual void update(Step * _step) override;


	void showLoading(float _percentage);
};