#pragma once

#include <PD_Scene_RoomGenerationTest.h>
#include <PD_ResourceManager.h>
#include <PD_Game.h>
#include <Resource.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>
#include <shader/ShaderComponentIndexedTexture.h>
#include <shader/ShaderComponentDepthOffset.h>

#include <NumberUtils.h>
#include <StringUtils.h>
#include <TextureUtils.h>
#include <sweet/UI.h>

#include <MousePerspectiveCamera.h>
#include <MeshFactory.h>

#include <RenderOptions.h>
#include <json\json.h>

#include <sweet/Input.h>

#include <Room.h>
#include <RoomBuilder.h>
#include <RoomObject.h>
#include <PD_TilemapGenerator.h>

#include <PD_Assets.h>

PD_Scene_RoomGenerationTest::PD_Scene_RoomGenerationTest(Game * _game) :
	Scene(_game),
	uiLayer(0,0,0,0),
	shader(new ComponentShaderBase(false)),
	characterShader(new ComponentShaderBase(false)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr)
{
	shader->addComponent(new ShaderComponentMVP(shader));
	shader->addComponent(new ShaderComponentTexture(shader, 0));
	//shader->addComponent(new ShaderComponentDiffuse(shader));
	shader->compileShader();

	characterShader->addComponent(new ShaderComponentMVP(characterShader));
	characterShader->addComponent(new ShaderComponentIndexedTexture(characterShader));
	characterShader->addComponent(new ShaderComponentDepthOffset(characterShader));
	characterShader->compileShader();

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);


	// remove initial camera
	childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();

	//Set up debug camera
	MousePerspectiveCamera * debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	childTransform->addChild(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;
	activeCamera = debugCam;

	uiLayer.addMouseIndicator();

	// add crosshair
	VerticalLinearLayout * l = new VerticalLinearLayout(uiLayer.world);
	l->setRationalHeight(1.f);
	l->setRationalWidth(1.f);
	l->horizontalAlignment = kCENTER;
	l->verticalAlignment = kMIDDLE;

	crosshairIndicator = new NodeUI(uiLayer.world);
	crosshairIndicator->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	crosshairIndicator->setWidth(16);
	crosshairIndicator->setHeight(16);
	crosshairIndicator->background->mesh->setScaleMode(GL_NEAREST);
	uiLayer.addChild(l);
	l->addChild(crosshairIndicator);
	/*
	PersonRenderer * testCharacter = new PersonRenderer(bulletWorld);
	childTransform->addChild(testCharacter);
	testCharacter->setShader(characterShader, true);
	testCharacter->unload();
	testCharacter->load();
	testCharacter->firstParent()->scale(0.005f);
	*/
	
	BulletMeshEntity * bulletGround = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh(), shader);
	bulletGround->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletGround->createRigidBody(0);
	childTransform->addChild(bulletGround);
	bulletGround->meshTransform->scale(1000,1000,1000);
	bulletGround->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletGround->body->translate(btVector3(0, -1, 0));
	bulletGround->body->setFriction(1);
	bulletGround->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GREY")->texture);
	
	room = RoomBuilder(dynamic_cast<AssetRoom *>(PD_ResourceManager::scenario->getAsset("room","1")), bulletWorld, shader, characterShader).getRoom();
	childTransform->addChild(room);
	room->setShader(shader, true);
	childTransform->addChild(room->tilemapSprite);

	std::vector<RoomObject *> components = room->getAllComponents();
	for(unsigned int i = 0; i < components.size(); ++i){
		childTransform->addChild(components.at(i));
	}

}

PD_Scene_RoomGenerationTest::~PD_Scene_RoomGenerationTest(){
	deleteChildTransform();
}

void PD_Scene_RoomGenerationTest::update(Step * _step){
	bulletWorld->update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_E)){
		printHierarchy();
	}

	if(keyboard->keyJustDown(GLFW_KEY_F12)){
		game->toggleFullScreen();
	}

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
	
	
	float speed = 1;
	MousePerspectiveCamera * cam = dynamic_cast<MousePerspectiveCamera *>(activeCamera);
	if(cam != nullptr){
		speed = cam->speed;
	}
	// camera controls
	if (keyboard->keyDown(GLFW_KEY_UP)){
		activeCamera->parents.at(0)->translate((activeCamera->forwardVectorRotated) * speed);
	}
	if (keyboard->keyDown(GLFW_KEY_DOWN)){
		activeCamera->parents.at(0)->translate((activeCamera->forwardVectorRotated) * -speed);
	}
	if (keyboard->keyDown(GLFW_KEY_LEFT)){
		activeCamera->parents.at(0)->translate((activeCamera->rightVectorRotated) * -speed);
	}
	if (keyboard->keyDown(GLFW_KEY_RIGHT)){
		activeCamera->parents.at(0)->translate((activeCamera->rightVectorRotated) * speed);
	}

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);
	uiLayer.update(_step);
}

void PD_Scene_RoomGenerationTest::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	
	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);

}

void PD_Scene_RoomGenerationTest::load(){
	Scene::load();	
	uiLayer.load();
}

void PD_Scene_RoomGenerationTest::unload(){
	uiLayer.unload();
	Scene::unload();	
}