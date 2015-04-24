#pragma once

#include <PD_Game.h>
#include <PD_TestScene.h>

PD_Game::PD_Game() :
	Game(true)
{
	currentSceneKey = "1";
	currentScene = new PD_TestScene(this);
	scenes.insert(std::pair<std::string, Scene * >(currentSceneKey, currentScene));
}

PD_Game::~PD_Game(){

}