#pragma once

#include <Player.h>

#include <node/NodeChild.h>
#include <node/NodeUpdatable.h>

#include <Scene.h>
#include <BulletDebugDrawer.h>
#include <BulletWorld.h>
#include <NodeBulletBody.h>

#include <Joystick.h>
#include <NodeBulletBody.h>
#include <NodeUI.h>
#include <MousePerspectiveCamera.h>
#include <Log.h>

#include <PD_ResourceManager.h>

#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <PointLight.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentMVP.h>

#include <MousePerspectiveCamera.h>

#include <RenderOptions.h>


#include <sweet/Input.h>

Player::Player(glm::vec3  sPos, BulletWorld * _bulletWorld, MousePerspectiveCamera * _playerCamera) : 
	playerCamera(_playerCamera),
	NodeBulletBody(_bulletWorld),
	keyboard(&Keyboard::getInstance()),
	mouse(&Mouse::getInstance()),
	joystick(nullptr)
{
	startPos = sPos;

	// player set-up
	this->setColliderAsCapsule(1, 1.5f);
	this->createRigidBody(1);
	this->body->setDamping(0.9, 0.8);
	this->body->setFriction(1);
	this->body->setAngularFactor(btVector3(0,1,0));


};

void Player::update(Step * _step){
	float playerSpeed = 3.f;
	float mass = 1;

	//mouseCam->parents.at(0)->translate(player->getWorldPos() + glm::vec3(0, 0, player->parents.at(0)->getScaleVector().z*1.25f), false);
	//mouseCam->lookAtOffset = glm::vec3(0, 0, -player->parents.at(0)->getScaleVector().z*0.25f);
	
	// get direction vectors
	glm::vec3 forward = playerCamera->forwardVectorRotated;
	glm::vec3 right = playerCamera->rightVectorRotated;
	
	// remove y portion of direction vectors to avoid flying
	forward.y = 0;
	right.y = 0;

	// normalize direction vectors for consistent motion regardless of viewing angle
	forward = glm::normalize(forward);
	right = glm::normalize(right);
	
	glm::vec3 movement(0);
	if (keyboard->keyDown(GLFW_KEY_W)){
		movement += forward;
	}if (keyboard->keyDown(GLFW_KEY_S)){
		movement -= forward;
	}if (keyboard->keyDown(GLFW_KEY_A)){
		movement -= right;
	}if (keyboard->keyDown(GLFW_KEY_D)){
		movement += right;
	}if (keyboard->keyDown(GLFW_KEY_SPACE)){
		movement += glm::vec3(0,10,0);
	}


	if(joystick != nullptr){
		movement += forward * -joystick->getAxis(joystick->axisLeftY);
		movement += right * joystick->getAxis(joystick->axisLeftX);
			
		// move camera by directly moving mouse
		float x2 = joystick->getAxis(joystick->axisRightX)*100;
		float y2 = -joystick->getAxis(joystick->axisRightY)*100;
		mouse->translate(glm::vec2(x2, y2));
	}
	
	float movementMag = glm::length(movement);
	if(movementMag > 0){
		//movement = movement/movementMag * playerSpeed * mass;
		movement = movement/movementMag * playerSpeed * mass;
		this->body->activate(true);
		this->body->applyCentralImpulse(btVector3(movement.x, movement.y, movement.z));
		this->body->applyDamping(btScalar(0));
	}
	else if(movementMag <= 0){
		this->body->activate(true);
		this->body->applyDamping(btScalar(0.025 * playerSpeed * mass));
	}




	// mouse interaction with world objects
	
	float range = 1000;
	glm::vec3 pos = playerCamera->childTransform->getWorldPos();
	btVector3 start(pos.x, pos.y, pos.z);
	btVector3 dir(playerCamera->forwardVectorRotated.x, playerCamera->forwardVectorRotated.y, playerCamera->forwardVectorRotated.z);
	btVector3 end = start + dir*range;

	
	//std::cout << dir.x() << "\t" << dir.y() << "\t" << dir.z() << std::endl;
	//std::cout << start.x() << "\t" << start.y() << "\t" << start.z() << std::endl;

	btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
	this->world->world->rayTest(start, end, RayCallback);
	//Log::info(std::to_string(RayCallback.hasHit()));
	if(RayCallback.hasHit()){
		NodeBulletBody * me = static_cast<NodeBulletBody *>(RayCallback.m_collisionObject->getUserPointer());
			
		if(me != nullptr){
			if(mouse->leftJustPressed()){
				/*me->body->activate(true);
				me->body->applyImpulse(dir*-10, me->body->getWorldTransform().getOrigin());*/
			}

			NodeUI * ui = dynamic_cast<NodeUI *>(me);
			if(ui != nullptr){
				ui->setUpdateState(true);
			}
		}
	}
}