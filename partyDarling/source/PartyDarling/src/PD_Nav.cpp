#pragma once

#include <PD_Nav.h>
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

#include <NodeBulletBody.h>
#include <BulletMeshEntity.h>
#include <Sprite.h>

#include <sweet/Input.h>

#include <Player.h>


PD_Nav::PD_Nav(Game * _game) :
	Scene(_game),
	diffuseShader(new ComponentShaderBase(true)),
	debugDrawer(nullptr),
	uiLayer(0,0,0,0)
{
	diffuseShader->addComponent(new ShaderComponentMVP(diffuseShader));
	diffuseShader->addComponent(new ShaderComponentTexture(diffuseShader));
	diffuseShader->addComponent(new ShaderComponentDiffuse(diffuseShader));
	diffuseShader->compileShader();

	// remove initial camera
	childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();

	//Set up debug camera
	debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	childTransform->addChild(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;


	bulletWorld = new BulletWorld();

	BulletMeshEntity * bulletGround = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh(), diffuseShader);
	bulletGround->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletGround->createRigidBody(0);
	childTransform->addChild(bulletGround);
	bulletGround->meshTransform->scale(1000,1000,1000);
	bulletGround->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletGround->body->translate(btVector3(0, -1, 0));
	bulletGround->body->setFriction(1);
	bulletGround->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GREY")->texture);

	BulletMeshEntity * bulletCube = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), diffuseShader);
	childTransform->addChild(bulletCube);
	bulletCube->meshTransform->scale(10,10,10);
	bulletCube->freezeTransformation();
	bulletCube->setColliderAsBoundingBox();
	bulletCube->createRigidBody(0);
	bulletCube->body->translate(btVector3(20, -1, 0));
	bulletCube->body->setFriction(0);
	bulletCube->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GREY")->texture);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);

	// crosshair
	crosshair = new Sprite();
	uiLayer.childTransform->addChild(crosshair);
	crosshair->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	crosshair->firstParent()->scale(16, 16, 1);
	crosshair->setShader(uiLayer.shader, true);
	crosshair->mesh->scaleModeMag = GL_NEAREST;
	crosshair->mesh->scaleModeMin = GL_NEAREST;




	
	

	playerEntity = new Player(bulletWorld);
	childTransform->addChild(playerEntity);
	cameras.push_back(playerEntity->playerCamera);
	activeCamera = playerEntity->playerCamera;
	childTransform->addChild(playerEntity->playerCamera);
	playerEntity->playerCamera->firstParent()->translate(0, 5, 0);
	

	
	PointLight * light2 = new PointLight(glm::vec3(1,1,1), 0.02f, 0.001f, -1);
	lights.push_back(light2);
	playerEntity->playerCamera->childTransform->addChild(light2);
}

PD_Nav::~PD_Nav(){
	deleteChildTransform();
	diffuseShader->safeDelete();

	delete bulletWorld;
}


void PD_Nav::update(Step * _step){


	glm::vec3 curpos = activeCamera->getWorldPos();
	NodeOpenAL::setListenerVelocity((curpos - lastPos));
	lastPos = curpos;

	NodeOpenAL::setListenerPosition(activeCamera->getWorldPos());
	NodeOpenAL::setListenerOrientation(activeCamera->forwardVectorRotated, activeCamera->upVectorRotated);
	
	
	// debug controls
	if(keyboard->keyJustDown(GLFW_KEY_1)){
		cycleCamera();
	}
	if(keyboard->keyJustUp(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
		if(debugDrawer != nullptr){
			bulletWorld->world->setDebugDrawer(nullptr);
			childTransform->removeChild(debugDrawer);
			delete debugDrawer;
			debugDrawer = nullptr;
			uiLayer.bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
		}else{
			debugDrawer = new BulletDebugDrawer(bulletWorld->world);
			childTransform->addChild(debugDrawer, false);
			debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
			bulletWorld->world->setDebugDrawer(debugDrawer);
			uiLayer.bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
		}
	}
	
	// update scene and physics
	bulletWorld->update(_step);

	Scene::update(_step);

	// update ui stuff
	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	uiLayer.update(_step);

	glm::vec3 sp = activeCamera->worldToScreen(glm::vec3(0,0,0), sd);
	if(sp.z < 0){
		sp.z = activeCamera->farClip * 2;
	}
	crosshair->parents.at(0)->translate(sd.x*0.5f, sd.y*0.5f, 0, false);
}

void PD_Nav::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_Nav::load(){
	Scene::load();	

	uiLayer.load();
}

void PD_Nav::unload(){
	uiLayer.unload();

	Scene::unload();	
}