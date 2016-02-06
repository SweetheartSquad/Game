#pragma once

#include <Player.h>
#include <PD_ResourceManager.h>

#include <OpenALSound.h>
#include <NumberUtils.h>
#include <Timeout.h>

Player::Player(BulletWorld * _bulletWorld) : 
	BulletFirstPersonController(_bulletWorld, 0.25f, 1.5f, 1.f),
	NodeBulletBody(_bulletWorld),
	shakeIntensity(0.3f),
	// speed
	playerSpeed(0.3f),
	sprintSpeed(2.f),
	jumpSpeed(5.f),
	// collider
	mass(1.f),
	sass(0),
	strenth(0),
	defense(0),
	insight(0)
{
	// override sounds
	footSteps = PD_ResourceManager::scenario->getAudio("PLAYER_FOOTSTEP")->sound;
	jumpSound = PD_ResourceManager::scenario->getAudio("PLAYER_JUMP")->sound;
	landSound = PD_ResourceManager::scenario->getAudio("PLAYER_FALL")->sound;

	// camera shake stuff
	shakeTimeout = new Timeout(0.5f, [this](sweet::Event * _event){
		camOffset = glm::vec3(0);
	});
	shakeTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		camOffset = sweet::NumberUtils::randomVec3(glm::vec3(-shakeIntensity), glm::vec3(shakeIntensity)) * Easing::easeOutCirc(_event->getFloatData("progress"), 1.f, -1.f, 1.f);
	});
	childTransform->addChild(shakeTimeout);
};