#pragma once

#include <PD_Game.h>
#include <PD_TestScene.h>
#include <PD_Scene_YellingContestTest.h>
#include <PD_Scene_FurnitureTest.h>
#include <PD_Scene_InteractionTest.h>
#include <PD_Scene_CombinedTests.h>
#include <PD_Scene_RoomGenerationTest.h>
#include <PD_Nav.h>


void PD_Game::addSplashes(){
	//Game::addSplashes(); // add default splashes
}

PD_Game::PD_Game() :
	Game("test", new PD_Scene_RoomGenerationTest(this), true)
{
	printFPS = false;
}

PD_Game::~PD_Game(){

}