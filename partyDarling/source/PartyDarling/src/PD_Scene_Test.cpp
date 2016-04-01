#pragma once

#include <PD_Scene_Test.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <Resource.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>
#include <shader/ShaderComponentIndexedTexture.h>
#include <shader/ShaderComponentDepthOffset.h>
#include <shader/ShaderComponentHsv.h>
#include <shader/ComponentShaderText.h>
#include <shader/ShaderComponentToon.h>
#include <PD_ShaderComponentSpecialToon.h>

#include <PD_PhraseGenerator_Incidental.h>

#include <MousePerspectiveCamera.h>
#include <MeshFactory.h>
#include <Timeout.h>

#include <RenderOptions.h>
#include <json\json.h>
#include <RampTexture.h>

#include <sweet/Input.h>
#include <PD_FurnitureParser.h>
#include <PD_Furniture.h>
#include <PD_Prop.h>
#include <PointLight.h>

PD_Scene_Test::PD_Scene_Test(PD_Game * _game) :
	Scene(_game),
	uiLayer(new UILayer(0,0,0,0)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr)
{

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);

	// add crosshair
	VerticalLinearLayout * l = new VerticalLinearLayout(uiLayer->world);
	l->setRationalHeight(1.f, uiLayer);
	l->setRationalWidth(1.f, uiLayer);
	l->horizontalAlignment = kCENTER;
	l->verticalAlignment = kMIDDLE;

	crosshairIndicator = new NodeUI(uiLayer->world);
	crosshairIndicator->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	crosshairIndicator->setWidth(16);
	crosshairIndicator->setHeight(16);
	crosshairIndicator->invalidateLayout();
	for(auto & v : crosshairIndicator->background->mesh->vertices){
		v.x -= 0.5f;
		v.y -= 0.5f;
	}crosshairIndicator->background->mesh->dirty = true;
	crosshairIndicator->background->mesh->setScaleMode(GL_NEAREST);
	uiLayer->addChild(l);
	l->addChild(crosshairIndicator);

	ComponentShaderBase * baseShader = new ComponentShaderBase(true);
	baseShader->addComponent(new ShaderComponentMVP(baseShader));
	//baseShader->addComponent(new ShaderComponentDiffuse(baseShader));
	baseShader->addComponent(new ShaderComponentTexture(baseShader));
	baseShader->compileShader();

	// add the player to the scene
	player = new Player(bulletWorld);
	childTransform->addChild(player);
	cameras.push_back(player->playerCamera);
	activeCamera = player->playerCamera;
	childTransform->addChild(player->playerCamera);
	player->playerCamera->firstParent()->translate(0, 20, 0);
	player->enable();

	// create floor/ceiling as static bullet planes
	BulletMeshEntity * floor = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh(100.f, 100.f), baseShader);
	floor->setColliderAsStaticPlane(0, 1, 0, 0);
	floor->createRigidBody(0);
	floor->body->setFriction(1);
	childTransform->addChild(floor);
	floor->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	floor->body->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(180.f)));
	floor->mesh->setScaleMode(GL_NEAREST);
	//floor->translatePhysical(glm::vec3(0, -0.1f, 0));

	for(Vertex &v : floor->mesh->vertices){
		v.red = 0.5f;
		v.green = 0.5f;
		v.blue = 0.5f;
	}

	int x = 0.f;
	int padding = 2.f;
	for(auto d : PD_ResourceManager::furnitureDefinitions){
		PD_Furniture * f = new PD_Furniture(bulletWorld, d, baseShader);
		x += f->boundingBox.width * 0.5f;
		childTransform->addChild(f);
		f->translatePhysical(glm::vec3(x, 0, 0));
		f->realign();
		x += f->boundingBox.width * 0.5f + padding;

		for(auto l : f->lights){
			lights.push_back(l);
		}
	}
	player->childTransform->translate(0, 0, 10.f);
	
	x = 0.f;
	for(auto d : PD_ResourceManager::propDefinitions){
		PD_Prop * p = new PD_Prop(bulletWorld, d, baseShader);
		x += p->boundingBox.width * 0.5f;
		childTransform->addChild(p);
		p->translatePhysical(glm::vec3(x, 0, -20.f));
		p->realign();
		x += p->boundingBox.width * 0.5f + padding;
	}
}


PD_Scene_Test::~PD_Scene_Test(){
	
	deleteChildTransform();
	delete uiLayer;
	delete bulletWorld;
}

void PD_Scene_Test::update(Step * _step){
	// party lights!
	bulletWorld->update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_1)){
		cycleCamera();
	}
	if(keyboard->keyJustUp(GLFW_KEY_2)){
		//Transform::drawTransforms = !Transform::drawTransforms;
		if(debugDrawer != nullptr){
			bulletWorld->world->setDebugDrawer(nullptr);
			childTransform->removeChild(debugDrawer);
			delete debugDrawer;
			debugDrawer = nullptr;
			uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
		}else{
			debugDrawer = new BulletDebugDrawer(bulletWorld->world);
			childTransform->addChild(debugDrawer, false);
			debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
			bulletWorld->world->setDebugDrawer(debugDrawer);
			uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
		}
	}

	
	if(keyboard->keyDown(GLFW_KEY_UP)){
		activeCamera->firstParent()->translate(activeCamera->forwardVectorRotated * 0.03f);
	}if(keyboard->keyDown(GLFW_KEY_DOWN)){
		activeCamera->firstParent()->translate(activeCamera->forwardVectorRotated * -0.03f);
	}if(keyboard->keyDown(GLFW_KEY_LEFT)){
		activeCamera->firstParent()->translate(activeCamera->rightVectorRotated * -0.03f);
	}if(keyboard->keyDown(GLFW_KEY_RIGHT)){
		activeCamera->firstParent()->translate(activeCamera->rightVectorRotated * 0.03f);
	}

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);
	uiLayer->update(_step);
}

void PD_Scene_Test::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){

	_renderOptions->setClearColour(0.5f,0.5f,0.5f,1.f);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);

	uiLayer->render(_matrixStack, _renderOptions);
}

void PD_Scene_Test::load(){
	Scene::load();	
	uiLayer->load();
}

void PD_Scene_Test::unload(){
	uiLayer->unload();
	Scene::unload();	
}