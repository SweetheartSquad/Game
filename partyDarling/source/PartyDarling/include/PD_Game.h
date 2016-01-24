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

	enum BOX2D_CATEGORY{
		kBOUNDARY = 0x0001,
		kPLAYER = 0x0002,
		kBUMPER = 0x0004,
		kENEMY = 0x0008,
		kDEAD_ZONE = 0x00010,
		kMONSTER = 0x00020,
		kCAT7 = 0x00040,
		kCAT8 = 0x00080,
		kCAT9 = 0x00100,
		kCAT10 = 0x00200,
		kCAT11 = 0x00400,
		kCAT12 = 0x00800
	};


	OpenAL_Sound * bgmTrack;
	AutoDrums * fightTrack;

	// picks a random BGM track and plays it, replacing the current track
	void playBGM();

	// generates a fight track and plays it, replacing the current track
	void playFight();

	
	virtual void update(Step * _step) override;
};