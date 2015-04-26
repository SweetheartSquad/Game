#pragma once

#include <ResourceManager.h>
#include <SoundManager.h>
#include <Texture.h>

class PD_ResourceManager : public ResourceManager{
public:
	static Texture * startSplash;
	static Texture * endSplash;
	static Texture * endSplashBad;
	static Texture * crosshair;
	static SoundManager * music;
	static SoundManager * sfx;
	static SoundManager * miscSounds;
	static SoundManager * bumperSfx;
	static SoundManager * enemySfx;
	static Texture * donutTop;
	static Texture * donutBot;

	static void init();
};