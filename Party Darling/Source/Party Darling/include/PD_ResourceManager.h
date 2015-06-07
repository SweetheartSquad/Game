#pragma once

#include <ResourceManager.h>
#include <SoundManager.h>
#include <Texture.h>

#include <OpenALSound.h>

class PD_ResourceManager : public ResourceManager{
public:
	static Texture * crosshair;
	static Texture * cursor;
	static Texture * cheryl;
	static Texture * uvs;
	static Texture * uvs_alt;
	static OpenAL_Sound * scene;
	static OpenAL_Sound * stream;

	static void init();
};