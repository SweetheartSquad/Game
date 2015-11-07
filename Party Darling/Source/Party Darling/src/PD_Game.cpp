#pragma once

#include <PD_Game.h>
#include <PD_TestScene.h>
#include <PD_Scene_YellingContestTest.h>

PD_Game::PD_Game() :
	Game(true, std::pair<std::string, Scene *>("test", new PD_Scene_YellingContestTest(this)), false)
{
}

PD_Game::~PD_Game(){

}