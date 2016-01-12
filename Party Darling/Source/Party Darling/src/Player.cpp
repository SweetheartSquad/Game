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

#include <Animation.h>
#include <Tween.h>

#include <OpenALSound.h>

#include <NumberUtils.h>

Player::Player(BulletWorld * _bulletWorld, MousePerspectiveCamera * _playerCamera) : 
	playerCamera(_playerCamera),
	NodeBulletBody(_bulletWorld),
	keyboard(&Keyboard::getInstance()),
	mouse(&Mouse::getInstance()),
	joystick(nullptr)
{
	//init movement vars
	playerSpeed = 0.1f;
	mass = 1;
	initSpeed = 2.0f;
	sprintSpeed = 5.0f;

	// player set-up
	playerRad = 1.f;
	playerHeight = 1.5f;
	this->setColliderAsCapsule(playerRad, playerHeight);
	this->createRigidBody(1);
	this->body->setFriction(1);
	this->body->setAngularFactor(btVector3(0,1,0));
	this->body->setLinearFactor(btVector3(1,0.9,1));
	this->maxVelocity = btVector3(-1,-1,-1);
	
	//Set how much camera drags behind mouse movement
	playerCamera->interpolation = 0.8f;

	//Head Bobble Animation
	headBobble = new Animation<float>(&bobbleVal);
	headBobble->loopType = Animation<float>::LoopType::kLOOP;
	headBobbleTween1 = new Tween<float>(0.15f,-0.005f,Easing::kEASE_IN_OUT_CUBIC);
	headBobbleTween2 = new Tween<float>(0.15f,0.005f,Easing::kEASE_IN_OUT_CUBIC);
	headBobble->startValue= 0.f;
	headBobble->tweens.push_back(headBobbleTween1);
	headBobble->tweens.push_back(headBobbleTween2);
	headBobble->hasStart = true;

	currentBobbleTween = 0;
	lastBobbleTween = 0;
	tweenBobbleChange = false;

	//Default Interpolation, controls how extremem head bobble is
	bobbleInterpolation = 0.f;

	lastYVel = 0;

	footSteps = new OpenAL_SoundSimple("assets/audio/crunchyStep.ogg", false, true, "sfx");
	jumpSound = new OpenAL_SoundSimple("assets/audio/jump.ogg", false, true, "sfx");
	landSound = new OpenAL_SoundSimple("assets/audio/fall.ogg", false, true, "sfx");
	/*easeIntoBobble = new Animation<float>(&easeIntoBobbleVal);
	easeIntoBobbleTween1 = new Tween<float>(0.25f,0.05f,Easing::kEASE_IN_CUBIC);
	easeIntoBobble->startValue=0.f;
	easeIntoBobble->tweens.push_back(easeIntoBobbleTween1);
	easeIntoBobble->hasStart = true;

	easeOutOfBobble = new Animation<float>(&easeOutOfBobbleVal);
	easeOutOfBobbleTween1 = new Tween<float>(0.25f,-0.05f,Easing::kEASE_OUT_CUBIC);
	easeOutOfBobble->startValue=0.f;
	easeOutOfBobble->tweens.push_back(easeOutOfBobbleTween1);
	easeOutOfBobble->hasStart = true;*/

	
};

void Player::update(Step * _step){
	
	//get player velocity
	btVector3 curVelocity = this->body->getLinearVelocity();
	float xVelocity = curVelocity[0];
	float zVelocity = curVelocity[2];

	float pitchRand = sweet::NumberUtils::randomFloat(0.75f,1.75f);

	//get player position
	btVector3 b = this->body->getWorldTransform().getOrigin();
	
	

	currentBobbleTween = headBobble->currentTween;
	if(currentBobbleTween != lastBobbleTween && currentBobbleTween == 1)
	{
		tweenBobbleChange = true;
	}
	else{
		tweenBobbleChange = false;
	}

	//restrict how player can rotate head upward and downward around x-axis
	if(playerCamera->pitch > 80){
		playerCamera->pitch = 80;
	}
	else if(playerCamera->pitch < -80){
		playerCamera->pitch = -80;
	}

	float rayRange = playerHeight * 1.25f;
	btVector3 rayEnd = b + btVector3(0,-1,0)*rayRange;
	btCollisionWorld::ClosestRayResultCallback GroundRayCallback(b, rayEnd);
	world->world->rayTest(b, rayEnd, GroundRayCallback);
	if(GroundRayCallback.hasHit()){
		//std::cout << "HIT" << std::endl;
		isGrounded = true;
	}else{
		isGrounded = false;
	}

	currentYVel = curVelocity.y();
	if (currentYVel > 0 && lastYVel < 0 && isGrounded){
		landSound->play();
	}
	lastYVel = currentYVel;

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
	
	//create movement vector
	glm::vec3 movement(0);
	if (keyboard->keyDown(GLFW_KEY_W)){
		movement += forward;
	}if (keyboard->keyDown(GLFW_KEY_S)){
		movement -= forward;
	}if (keyboard->keyDown(GLFW_KEY_A)){
		movement -= right;
	}if (keyboard->keyDown(GLFW_KEY_D)){
		movement += right;
	}if (keyboard->keyJustDown(GLFW_KEY_SPACE)){
		if(isGrounded){
			movement += glm::vec3(0,100,0);
			jumpSound->play();
		}
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

	//If player is moving
	if(movementMag > 0){

		if(tweenBobbleChange){
			footSteps->setPitch(pitchRand);
			footSteps->play();
		}
		//set movement
		movement = movement/movementMag * playerSpeed * mass;

		//set initial walking speed so that there is no ease in
		float initXSpeed = (movement/movementMag)[0]*initSpeed;
		float initZSpeed = (movement/movementMag)[2]*initSpeed;

		//set initial running speed
		float sprintXSpeed = (movement/movementMag)[0]*sprintSpeed;
		float sprintZSpeed = (movement/movementMag)[2]*sprintSpeed;

		this->body->activate(true);

		//Shift key for sprint
		if(keyboard->keyDown(GLFW_KEY_LEFT_SHIFT)){
			if(bobbleInterpolation<2){
				bobbleInterpolation += 0.1f;
			}
			this->body->applyCentralImpulse(btVector3(sprintXSpeed+movement.x, movement.y*50, sprintZSpeed+movement.z));
			
		}
		else{
			if(bobbleInterpolation<1){
				bobbleInterpolation += 0.1f;
			}
			this->body->applyCentralImpulse(btVector3(initXSpeed+movement.x, movement.y*50, initZSpeed+movement.z));
		}
		
	}

	//If the player is not moving
	else if(movementMag <= 0){
		//float slideVal = 10.0f;
		this->body->activate(true);
		//slow down body by applying force in opposite direction of its velocity
		this->body->applyCentralImpulse(btVector3(xVelocity*-0.2,0,zVelocity*-0.2));
		if(bobbleInterpolation > 0){
			bobbleInterpolation -= 0.1f;
		}
		else{
			bobbleInterpolation = 0.f;
		}
		/*float curVel = std::max(abs(xVelocity), abs(zVelocity));
		if(curVel > slideVal){
			if(xVelocity > zVelocity){
				this -> maxVelocity.setX(slideVal/xVelocity*maxVelocity.x());
				this -> maxVelocity.setZ(slideVal/xVelocity*maxVelocity.z());
			}
			else if(zVelocity > xVelocity){
				this -> maxVelocity.setX(slideVal/zVelocity*maxVelocity.x());
				this -> maxVelocity.setZ(slideVal/zVelocity*maxVelocity.z());
			}
			else{
				this -> maxVelocity = btVector3(slideVal,-1,slideVal);
			}
		}*/
	}

	// If the player isnt moving vertically
	if(abs(curVelocity[1]) <= 0.01f){
		headBobble->update(_step);
	}
	playerCamera->firstParent()->translate(b.x(), bobbleVal*bobbleInterpolation+b.y(), b.z(), false);


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

	lastBobbleTween = currentBobbleTween;
	NodeBulletBody::update(_step);
}


glm::vec3 Player::getPlayerPosition(){
	btVector3 playerPos = this->body->getWorldTransform().getOrigin();
	return glm::vec3(playerPos.x(), playerPos.y(), playerPos.z());
}
glm::vec3 Player::getPlayerLinearVelocity(){
	btVector3 playerVel = this->body->getLinearVelocity(); 
	return glm::vec3(playerVel.x(), playerVel.y(),playerVel.z());
}