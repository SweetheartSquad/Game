#pragma once

#include <Vox.h>
#include <node\Node.h>
#include <typeinfo>
#include <Texture.h>
#include <Game.h>
#include <PD_ResourceManager.h>

#include <PD_Game.h>

// memory leak debugging
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(void){
	_CrtMemState s1;
	_CrtMemCheckpoint( &s1 );

	vox::initialize("Party, Darling? -super test build EX edition");
	PD_ResourceManager::init();
	PD_ResourceManager::load();
	PD_Game * game = new PD_Game();

	//Hide the mouse cursor
	glfwSetInputMode(vox::currentContext, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	while (game->isRunning){
		game->performGameLoop();
	}
	
	delete game;
	game = nullptr;
	PD_ResourceManager::destruct();
#ifdef _DEBUG
	std::cout << "Final node count: " << Node::nodes.size() << std::endl;

	for(auto n : Node::nodes){
		std::cout << typeid(*n).name() << std::endl;
	}
#endif

	vox::destruct();
	
	_CrtMemDumpAllObjectsSince(&s1);
}