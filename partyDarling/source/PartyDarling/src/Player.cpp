#pragma once

#include <Player.h>
#include <PD_ResourceManager.h>

#include <OpenALSound.h>
#include <NumberUtils.h>
#include <Timeout.h>

#include <Mouse.h>
#include <Keyboard.h>

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
};

Player::~Player(){
	delete shakeTimeout;
}

void Player::update(Step * _step){
	shakeTimeout->update(_step);
	BulletFirstPersonController::update(_step);
}

glm::vec3 Player::calculateInputs(Step * _step){
	// get direction vectors
	glm::vec3 forward = playerCamera->forwardVectorRotated;
	glm::vec3 right = playerCamera->rightVectorRotated;
	
	// remove y portion of direction vectors to avoid flying
	forward.y = 0;
	right.y = 0;

	// walking
	glm::vec3 res(0);
	if (keyboard.keyDown(GLFW_KEY_W)){
		res += forward;
	}if (keyboard.keyDown(GLFW_KEY_S)){
		res -= forward;
	}if (keyboard.keyDown(GLFW_KEY_A)){
		res -= right;
	}if (keyboard.keyDown(GLFW_KEY_D)){
		res += right;
	}
	/*if(joystick != nullptr){
		movement += forward * -joystick->getAxis(joystick->axisLeftY);
		movement += right * joystick->getAxis(joystick->axisLeftX);
			
		// move camera by directly moving mouse
		float x2 = joystick->getAxis(joystick->axisRightX)*100;
		float y2 = -joystick->getAxis(joystick->axisRightY)*100;
		mouse->translate(glm::vec2(x2, y2));
	}*/

	// clamp the results in case we're using multiple inputs
	res.x = glm::max(-1.f, glm::min(1.f, res.x));
	res.z = glm::max(-1.f, glm::min(1.f, res.z));
	
	//sprinting
	isSprinting = keyboard.keyDown(GLFW_KEY_LEFT_SHIFT);

	// jumping
	if(isGrounded){
		if (keyboard.keyJustDown(GLFW_KEY_SPACE)){
			res.y = 1.f;
		}
	}
	return res;
}