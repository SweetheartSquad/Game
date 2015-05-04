#pragma once

#include <PD_Game.h>
#include <PD_TestScene.h>

PD_Game::PD_Game() :
	Game(true, std::pair<std::string, Scene *>("test", new PD_TestScene(this)))
{
}

PD_Game::~PD_Game(){

}