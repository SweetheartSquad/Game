#pragma once

#include <PD_Game.h>
#include <PD_TestScene.h>
#include <PD_Scene_YellingContestTest.h>
#include <PD_Scene_FurnitureTest.h>

PD_Game::PD_Game() :
	Game("test", new PD_Scene_YellingContestTest(this), true)
{
}

PD_Game::~PD_Game(){

}