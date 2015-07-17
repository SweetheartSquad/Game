#pragma once

#include <PD_FirstPersonController.h>

#include <Keyboard.h>
#include <Mouse.h>
#include <Joystick.h>
#include <NodeBulletBody.h>
#include <PerspectiveCamera.h>

PD_FirstPersonController::PD_FirstPersonController(NodeBulletBody * _targetEntity, PerspectiveCamera * _targetCamera) :
	targetEntity(_targetEntity),
	targetCamera(_targetCamera),
	keyboard(&Keyboard::getInstance()),
	mouse(&Mouse::getInstance()),
	joystick(nullptr)
{

}

void PD_FirstPersonController::update(Step * _step){
	float playerSpeed = 0.1f;
	float mass = 1;//target->body->get();

	//mouseCam->parents.at(0)->translate(player->getWorldPos() + glm::vec3(0, 0, player->parents.at(0)->getScaleVector().z*1.25f), false);
	//mouseCam->lookAtOffset = glm::vec3(0, 0, -player->parents.at(0)->getScaleVector().z*0.25f);
	
	glm::vec3 forward = targetCamera->forwardVectorRotated;
	glm::vec3 right = targetCamera->rightVectorRotated;
	glm::vec3 movement(0);
	if (keyboard->keyDown(GLFW_KEY_W)){
		movement += forward;
	}
	if (keyboard->keyDown(GLFW_KEY_S)){
		movement -= forward;
	}
	if (keyboard->keyDown(GLFW_KEY_A)){
		movement -= right;
	}
	if (keyboard->keyDown(GLFW_KEY_D)){
		movement += right;
	}

	if(joystick != nullptr){
		movement += forward * -joystick->getAxis(joystick->axisLeftY);
		movement += right * joystick->getAxis(joystick->axisLeftX);
			
		// move camera by directly moving mouse
		float x2 = joystick->getAxis(joystick->axisRightX)*100;
		float y2 = -joystick->getAxis(joystick->axisRightY)*100;
		mouse->translate(glm::vec2(x2, y2));
	}

	movement *= playerSpeed * mass;

	if(movement.x != 0 || movement.y != 0 || movement.z != 0){
		targetEntity->body->activate(true);
		targetEntity->body->applyCentralImpulse(btVector3(movement.x, movement.y, movement.z));
	}

	targetCamera->parents.at(0)->translate(targetEntity->getWorldPos() + glm::vec3(0, 0.6f, 0), false);
	targetCamera->parents.at(0)->setOrientation(targetEntity->parents.at(0)->getOrientationQuat());
}