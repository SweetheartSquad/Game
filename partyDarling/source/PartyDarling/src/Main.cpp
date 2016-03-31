#pragma once

#include <Sweet.h>
#include <node\Node.h>
#include <typeinfo>
#include <Texture.h>
#include <Game.h>
#include <PD_ResourceManager.h>
#include <Log.h>
#include <ctime>
#include <NumberUtils.h>

#include <PD_Game.h>

#ifdef _DEBUG
// memory leak debugging
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef _DEBUG
int main(void){
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);

	_CrtMemState s1;
	_CrtMemCheckpoint( &s1 );
#else
int __stdcall WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show){
#endif

	Log::THROW_ON_ERROR = true;

	// initialize S-Tengine2
	sweet::initialize("Party, Darling? -Beta "
#ifdef _DEBUG
		"Debug"
#else
		"Release"
#endif
		" Build");

	//sweet::NumberUtils::seed(time(nullptr)); // NOTE: this is no longer needed; you can set the seed with "rngSeed" in config.json. If you want a time-based seed, use a seed of -1
	//OpenAL_Sound::masterGain = 0;
#ifdef _DEBUG
	Node::nodeCounting = true; // uncomment this if you're checking for memory leaks and stuff (it's really slow so don't do it if you don't need it)
#endif

	// initialize resources
	PD_ResourceManager * resources = new PD_ResourceManager();

	// create game
	PD_Game * game = new PD_Game();
	game->init();

	// main game loop
	while (game->isRunning){
		game->performGameLoop();
	}

	// memory deallocation
	delete game;
	game = nullptr;
	delete resources;
	resources = nullptr;

	sweet::destruct();
	sweet::printNodes();

#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince(&s1);
#endif
}