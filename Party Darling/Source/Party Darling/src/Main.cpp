#pragma once

#include <Vox.h>
#include <node\Node.h>
#include <typeinfo>
#include <Texture.h>
#include <Game.h>
#include <PD_ResourceManager.h>

#include <PD_Game.h>

int main(void){
	vox::initialize("Vox");
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
}