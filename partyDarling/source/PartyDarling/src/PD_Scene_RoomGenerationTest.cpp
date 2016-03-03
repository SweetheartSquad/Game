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
	uiLayer(new UILayer(0,0,0,0)),
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
	uiLayer->resize(0,sd.x,0,sd.y);


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
	activeCamera = debugCam;

	uiLayer->addMouseIndicator();

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
	crosshairIndicator->background->mesh->setScaleMode(GL_NEAREST);
	uiLayer->addChild(l);
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
	
	/*
	room = RoomBuilder(dynamic_cast<AssetRoom *>(PD_ResourceManager::scenario->getAsset("room","1")), bulletWorld, shader, characterShader).getRoom();
	childTransform->addChild(room);
	room->setShader(shader, true);
	childTransform->addChild(room->tilemapSprite);

	std::vector<RoomObject *> components = room->getAllComponents();
	for(unsigned int i = 0; i < components.size(); ++i){
		childTransform->addChild(components.at(i));
	}
	*/

	BulletMeshEntity * a1 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	a1->setColliderAsBoundingBox();
	a1->createRigidBody(0);
	BulletMeshEntity * a2 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	a2->setColliderAsBoundingBox();
	a2->createRigidBody(0);

	BulletMeshEntity * b1 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	b1->setColliderAsBoundingBox();
	b1->createRigidBody(0);
	b1->translatePhysical(glm::vec3(0, 0, 10.f));
	b1->realign();
	BulletMeshEntity * b2 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	b2->setColliderAsBoundingBox();
	b2->createRigidBody(0);
	b2->translatePhysical(glm::vec3(0, 0, 10.f));
	b2->realign();

	BulletMeshEntity * c1 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	c1->setColliderAsBoundingBox();
	c1->createRigidBody(0);
	c1->translatePhysical(glm::vec3(0, 0, 20.f));
	c1->realign();
	BulletMeshEntity * c2 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	c2->meshTransform->scale(0.5f);
	c2->freezeTransformation();
	c2->setColliderAsBoundingBox();
	c2->createRigidBody(0);
	c2->translatePhysical(glm::vec3(0, 0, 20.f));
	c2->realign();

	BulletMeshEntity * d1 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	d1->setColliderAsBoundingBox();
	d1->createRigidBody(0);
	d1->translatePhysical(glm::vec3(0, 0, 30.f));
	d1->realign();
	BulletMeshEntity * d2 = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh(), shader);
	d2->setColliderAsBoundingBox();
	d2->createRigidBody(0);
	d2->translatePhysical(glm::vec3(0, 0, 30.f));
	d2->realign();


	//  Against
	glm::mat4 mmA1 = a1->meshTransform->getCumulativeModelMatrix();
	a2->translatePhysical(glm::vec3(-a2->mesh->calcBoundingBox().width, 0, 0));
	a2->realign();
	glm::mat4 mmA2 = a2->meshTransform->getCumulativeModelMatrix();
	bool a = a1->mesh->calcBoundingBox().intersects(getLocalBoundingBoxVertices(a2->mesh->calcBoundingBox().getVertices(), mmA2, mmA1));

	// Corner intersection
	
	glm::mat4 mmB1 = b1->meshTransform->getCumulativeModelMatrix();
	float bT = -b2->mesh->calcBoundingBox().width * 0.5;
	b2->translatePhysical(glm::vec3(bT, 0, bT));
	b2->realign();
	glm::mat4 mmB2 = b2->meshTransform->getCumulativeModelMatrix();
	sweet::Box blah = sweet::Box::bound(getLocalBoundingBoxVertices(b2->mesh->calcBoundingBox().getVertices(), mmB2, mmB1));
	bool b = b1->mesh->calcBoundingBox().intersects(getLocalBoundingBoxVertices(b2->mesh->calcBoundingBox().getVertices(), mmB2, mmB1));
	
	// Totaly inside
	glm::mat4 mmC1 = c1->meshTransform->getCumulativeModelMatrix();
	glm::mat4 mmC2 = c2->meshTransform->getCumulativeModelMatrix();
	bool c = c1->mesh->calcBoundingBox().intersects(getLocalBoundingBoxVertices(c2->mesh->calcBoundingBox().getVertices(), mmC2, mmC1));

	// No collision
	glm::mat4 mmD1 = d1->meshTransform->getCumulativeModelMatrix();
	d2->translatePhysical(glm::vec3(-d2->mesh->calcBoundingBox().width * 1.5, 0, 0));
	d2->realign();
	glm::mat4 mmD2 = d2->meshTransform->getCumulativeModelMatrix();
	bool d = d1->mesh->calcBoundingBox().intersects(getLocalBoundingBoxVertices(d2->mesh->calcBoundingBox().getVertices(), mmD2, mmD1));

	std::stringstream s;
	s << "Against Edge = " << a << " Corner Intersection = " << b << " Completely Inside = " << c << " Completely Left = " << d;

	Log::info(s.str());

	childTransform->addChild(a1);
	childTransform->addChild(a2);
	childTransform->addChild(b1);
	childTransform->addChild(b2);
	childTransform->addChild(c1);
	childTransform->addChild(c2);
	childTransform->addChild(d1);
	childTransform->addChild(d2);
}

PD_Scene_RoomGenerationTest::~PD_Scene_RoomGenerationTest(){
	deleteChildTransform();
	delete uiLayer;
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
			uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
		}else{
			debugDrawer = new BulletDebugDrawer(bulletWorld->world);
			childTransform->addChild(debugDrawer, false);
			debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
			bulletWorld->world->setDebugDrawer(debugDrawer);
			uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
		}
	}
	

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);
	uiLayer->update(_step);
}

void PD_Scene_RoomGenerationTest::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	
	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer->render(_matrixStack, _renderOptions);

}

void PD_Scene_RoomGenerationTest::load(){
	Scene::load();	
	uiLayer->load();
}

void PD_Scene_RoomGenerationTest::unload(){
	uiLayer->unload();
	Scene::unload();	
}

std::vector<glm::vec3> PD_Scene_RoomGenerationTest::getLocalBoundingBoxVertices(std::vector<glm::vec3> _verts, glm::mat4 _mmA, glm::mat4 _mmB){

	glm::mat4 immB = glm::inverse(_mmB);
	glm::mat4 m = immB * _mmA;

	std::vector<glm::vec4> transformed; 
	for(auto v : _verts){
		transformed.push_back(m * glm::vec4(v, 1));
	}

	std::vector<glm::vec3> vertices; 
	for(auto t : transformed){
		vertices.push_back(glm::vec3(t.x, t.y, t.z));
	}

	return vertices;
}