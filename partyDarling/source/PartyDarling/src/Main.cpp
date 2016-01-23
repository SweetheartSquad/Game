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

int main(void){

	//  Against
	sweet::Box a1 = sweet::Box(0, 0, 0, 1, 1, 1);
	sweet::Box a2 = sweet::Box(-1, 0, 0, 1, 1, 1);
	bool a = a1.intersects(a2);

	// Corner intersection
	sweet::Box b1 = sweet::Box(0, 0, 0, 1, 1, 1);
	sweet::Box b2 = sweet::Box(0.5, 0, 0.5, 1, 1, 1);
	bool b = b1.intersects(b2);

	// Totaly inside
	sweet::Box c1 = sweet::Box(0, 0, 0, 1, 1, 1);
	sweet::Box c2 = sweet::Box(0.25, 0, 0.25, 0.5, 0.5, 0.5);
	bool c = c1.intersects(c2);

	// No collision
	sweet::Box d1 = sweet::Box(0, 0, 0, 1, 1, 1);
	sweet::Box d2 = sweet::Box(-1.5, 0, 0, 1, 1, 1);
	bool d = d1.intersects(d2);
	
	std::stringstream s;
	s << "Against Edge = " << a << " Corner Intersection = " << b << " Completely Inside = " << c << " Completely Left = " << d;

	//Log::info(s.str());

#ifdef _DEBUG
	_CrtMemState s1;
	_CrtMemCheckpoint( &s1 );
#endif

	Log::THROW_ON_ERROR = true;

	sweet::NumberUtils::seed(time(nullptr));

	sweet::initialize("Party, Darling? -test prepping");
	PD_ResourceManager::init();
	PD_ResourceManager::load();
	PD_Game * game = new PD_Game();
	game->init();
	while (game->isRunning){
		game->performGameLoop();
	}
	
	delete game;
	game = nullptr;
	PD_ResourceManager::destruct();
#ifdef _DEBUG
	std::cout << "Final node count: " << Node::nodes.size() << std::endl;

	for(auto n : Node::nodes){
		std::cout << typeid(*n).name() << " " << n << std::endl;
	}
#endif

	sweet::destruct();
	
#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince(&s1);
#endif
}