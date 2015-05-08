#pragma once

#include <PD_TestScene.h>
#include <PD_Game.h>
#include <PD_ResourceManager.h>
#include <PD_Player.h>
#include <PD_ContactListener.h>

#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <DirectionalLight.h>
#include <PointLight.h>
#include <Material.h>

#include <shader\BaseComponentShader.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentPhong.h>
#include <shader\ShaderComponentBlinn.h>
#include <shader\ShaderComponentShadow.h>
#include <shader\ShaderComponentHsv.h>

#include <Box2DWorld.h>
#include <Box2DMeshEntity.h>
#include <Box2DDebugDrawer.h>

#include <MousePerspectiveCamera.h>
#include <FollowCamera.h>

#include <Sound.h>
#include <libzplay.h>

#include <System.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>
#include <MatrixStack.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <NumberUtils.h>
#include <RenderOptions.h>
#include <shader\ShaderComponentText.h>
#include <StringUtils.h>
#include <CharacterUtils.h>


PD_TestScene::PD_TestScene(Game * _game) :
	Scene(_game),
	shader(new BaseComponentShader(true)),
	textShader(new BaseComponentShader(true)),
	hsvComponent(new ShaderComponentHsv(shader, 0, 1, 1)),
	box2dWorld(new Box2DWorld(b2Vec2(0, 0))),
	box2dDebugDrawer(nullptr),
	debugDrawer(nullptr),
	player(nullptr),
	screenSurfaceShader(new Shader("../assets/RenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	phongMat(new Material(15.0, glm::vec3(1.f, 1.f, 1.f), true)),
	sceneHeight(150),
	sceneWidth(50),
	firstPerson(true),
	joy(new JoystickManager()),
	uiLayer(0,0,0,0)
{

	shader->addComponent(new ShaderComponentTexture(shader));
	shader->addComponent(new ShaderComponentDiffuse(shader));
	shader->addComponent(hsvComponent);
	//shader->addComponent(new ShaderComponentPhong(shader));
	//shader->addComponent(new ShaderComponentBlinn(shader));
	//shader->addComponent(new ShaderComponentShadow(shader));

	shader->compileShader();


	//Set up cameras
	Transform * t = new Transform();
	mouseCam = new MousePerspectiveCamera();
	t->addChild(mouseCam);
	cameras.push_back(mouseCam);
	mouseCam->farClip = 1000.f;
	mouseCam->nearClip = 0.001f;
	mouseCam->parent->rotate(90, 0, 1, 0, kWORLD);
	mouseCam->parent->translate(5.0f, 1.5f, 22.5f);
	mouseCam->yaw = 90.0f;
	mouseCam->pitch = -10.0f;
	mouseCam->speed = 1;

	debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	t = new Transform();
	t->addChild(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->parent->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parent->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;

	gameCam = new FollowCamera(15, glm::vec3(0, 0, 0), 0, 0);
	cameras.push_back(gameCam);
	t = new Transform();
	t->addChild(gameCam);
	gameCam->farClip = 1000.f;
	gameCam->parent->rotate(90, 0, 1, 0, kWORLD);
	gameCam->parent->translate(5.0f, 1.5f, 22.5f);
	gameCam->minimumZoom = 22.5f;
	gameCam->yaw = 90.0f;
	gameCam->pitch = -10.0f;

	activeCamera = mouseCam;
	
	float _size = 3;
	std::vector<Box2DMeshEntity *> boundaries;
	MeshInterface * boundaryMesh = MeshFactory::getCubeMesh();
	boundaries.push_back(new Box2DMeshEntity(box2dWorld, boundaryMesh, b2_staticBody));
	boundaries.push_back(new Box2DMeshEntity(box2dWorld, boundaryMesh, b2_staticBody));
	boundaries.push_back(new Box2DMeshEntity(box2dWorld, boundaryMesh, b2_staticBody));
	boundaries.push_back(new Box2DMeshEntity(box2dWorld, boundaryMesh, b2_staticBody));
	
	for(auto b : boundaries){
		childTransform->addChild(b);
		b->setShader(shader, true);
		b->mesh->pushMaterial(phongMat);
	}

	boundaries.at(0)->parent->scale(_size, sceneHeight*0.5f + _size*2.f, _size * 4.f);
	boundaries.at(1)->parent->scale(_size, sceneHeight*0.5f + _size*2.f, _size * 4.f);
	boundaries.at(2)->parent->scale(sceneWidth*0.5f + _size*2.f, _size, _size * 4.f);
	boundaries.at(3)->parent->scale(sceneWidth*0.5f + _size*2.f, _size, _size * 4.f);

	boundaries.at(0)->setTranslationPhysical(sceneWidth+_size, sceneHeight*0.5f, 0);
	boundaries.at(1)->setTranslationPhysical(-_size, sceneHeight*0.5f, 0);
	boundaries.at(2)->setTranslationPhysical(sceneWidth*0.5f, sceneHeight+_size, 0);
	boundaries.at(3)->setTranslationPhysical(sceneWidth*0.5f, -_size, 0);
	
	// important that this is done after scaling
	b2Filter sf;
	for(auto b : boundaries){
		box2dWorld->addToWorld(b);
		b->body->GetFixtureList()->SetFilterData(sf);
	}
	boundaries.at(3)->body->GetFixtureList()->SetFilterData(sf);
	boundaries.at(3)->body->GetFixtureList()->SetFriction(1);
	boundaries.at(3)->body->GetFixtureList()->SetRestitution(0);

	MeshEntity * ground = new MeshEntity(MeshFactory::getPlaneMesh());
	childTransform->addChild(ground);
	ground->parent->translate(sceneWidth/2.f, sceneHeight/2.f, -2.f);
	ground->parent->scale(sceneWidth/2.f, sceneHeight/2.f, 1);
	ground->setShader(shader, true);

	/*MeshEntity * ceiling = new MeshEntity(MeshFactory::getPlaneMesh());
	ceiling->transform->translate(sceneWidth/2.f, sceneHeight/2.f, _size * 4.f);
	ceiling->transform->scale(sceneWidth, sceneHeight, 1);
	ceiling->setShader(shader, true);
	addChild(ceiling);*/


	//lights.push_back(new DirectionalLight(glm::vec3(1,0,0), glm::vec3(1,1,1), 0));
	
	player = new PD_Player(box2dWorld);
	childTransform->addChild(player);
	player->setShader(shader, true);
	gameCam->addTarget(player, 1);

	player->setTranslationPhysical(sceneWidth / 2.f, sceneHeight / 8.f, 0, false);
	player->body->SetLinearDamping(2.5f);
	player->body->SetAngularDamping(2.5f);

	//intialize key light
	PointLight * keyLight = new PointLight(glm::vec3(1.f, 1.f, 1.f), 0.01f, 0.01f, -10.f);
	//Set it as the key light so it casts shadows
	//keyLight->isKeyLight = true;
	//Add it to the scene
	lights.push_back(keyLight);
	player->childTransform->addChild(keyLight);
	keyLight->parent->translate(0,0,0.2f);
	
	mouseCam->upVectorLocal = glm::vec3(0, 0, 1);
	mouseCam->forwardVectorLocal = glm::vec3(1, 0, 0);
	mouseCam->rightVectorLocal = glm::vec3(0, -1, 0);

	PD_ContactListener * cl = new PD_ContactListener(this);
	box2dWorld->b2world->SetContactListener(cl);
	
	crosshair = new Sprite();
	uiLayer.childTransform->addChild(crosshair);
	crosshair->mesh->pushTexture2D(PD_ResourceManager::crosshair);
	crosshair->parent->scale(8,8,1);
	crosshair->setShader(uiLayer.shader, true);

	playerIndicator = new Sprite();
	uiLayer.childTransform->addChild(playerIndicator);
	playerIndicator->mesh->pushTexture2D(PD_ResourceManager::crosshair);
	playerIndicator->parent->scale(8,8,1);
	playerIndicator->setShader(uiLayer.shader, true);

	mouseIndicator = new Sprite();
	uiLayer.childTransform->addChild(mouseIndicator);
	mouseIndicator->mesh->pushTexture2D(PD_ResourceManager::cursor);
	mouseIndicator->parent->scale(32 * 10 * 0.5, 32 * 10 * 0.5, 1);
	mouseIndicator->mesh->scaleModeMag = GL_NEAREST;
	mouseIndicator->mesh->scaleModeMin = GL_NEAREST;

	for(unsigned long int i = 0; i < mouseIndicator->mesh->vertices.size(); ++i){
		mouseIndicator->mesh->vertices[i].x -= 1;
		mouseIndicator->mesh->vertices[i].y -= 1;
	}
	mouseIndicator->mesh->dirty = true;
	mouseIndicator->setShader(uiLayer.shader, true);

	screenSurface->scaleModeMag = GL_NEAREST;
	screenSurface->scaleModeMin = GL_NEAREST;

	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadphase = new btDbvtBroadphase(); // how the world loops through the possible collisions?
	solver = new btSequentialImpulseConstraintSolver();
	bulletWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	bulletWorld->setGravity(btVector3(0, -20, 0));


	btTransform bt;
	bt.setIdentity();
	bt.setOrigin(btVector3(0,0,0));
	btStaticPlaneShape * plane = new btStaticPlaneShape(btVector3(0,1,0), 0);
	btMotionState * motion = new btDefaultMotionState(bt);
	btRigidBody::btRigidBodyConstructionInfo info(0, motion, plane);
	btRigidBody * body = new btRigidBody(info);
	bulletWorld->addRigidBody(body);
	bodies.push_back(body);
	bodies2.push_back(new MeshEntity(MeshFactory::getPlaneMesh()));
	childTransform->addChild(bodies2.back());
	bodies2.back()->setShader(shader, true);
	bodies2.back()->parent->scale(50, 50, 50);
	bodies2.back()->parent->rotate(90, 1, 0, 0, kOBJECT);
	bodies2.back()->freezeTransformation();
	
	textShader->addComponent(new ShaderComponentText(textShader));
	textShader->compileShader();

	//children.clear();

	font = new Font("../assets/arial.ttf", 100, false);
	label = new Label(font, textShader);
	label->setText("The");	
	childTransform->addChild(label);

	/*for(unsigned long int i = 0; i < 1000; ++i){
		MeshEntity * me = new MeshEntity(MeshFactory::getCubeMesh());
		me->setShader(shader, true);
		childTransform->addChild(me);
	}*/
}

void PD_TestScene::addThing(){
	MeshInterface * me;
	if(bodies.size() == 1){
		me = MeshFactory::getCubeMesh();
		bodies2.push_back(new MeshEntity(me));
		childTransform->addChild(bodies2.back());

		bodies2.back()->parent->scale(3,3,3);
		bodies2.back()->freezeTransformation();
	}else{
		me = bodies2.back()->mesh;
		bodies2.push_back(new MeshEntity(me));
		childTransform->addChild(bodies2.back());
	}
	bodies2.back()->setShader(shader, true);

	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(std::rand() % 30, 100, std::rand() % 30));
	btBoxShape * shape = new btBoxShape(btVector3(3,3,3));
	btMotionState * motion = new btDefaultMotionState(t);
	btVector3 inertia(0,0,0);
	float mass = 1;
	if(mass != 0){
		shape->calculateLocalInertia(mass, inertia);
	}
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, shape, inertia);
	btRigidBody * body = new btRigidBody(info);
	bulletWorld->addRigidBody(body);
	bodies.push_back(body);
}

PD_TestScene::~PD_TestScene(){
	shader->safeDelete();
	//delete phongMat;
	delete box2dWorld;

	screenSurface->safeDelete();
	//screenSurfaceShader->safeDelete();
	screenFBO->safeDelete();
	delete joy;

	delete bulletWorld;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete collisionConfig;

	delete font;
}

void PD_TestScene::update(Step * _step){
	// handle inputs

	joy->update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_BACKSPACE)){
		if(label->getText().size() > 0){
			label->setText(label->getText().substr(0, label->getText().size() - 1));
		}
	}
	if(keyboard->justReleasedKeys.size() > 0){
		std::string acc = "";
		for(auto k : keyboard->justReleasedKeys){
			if(CharacterUtils::isSymbolLetterDigit(k.second)){
				acc += k.second;
			}
		}
		if(acc != ""){
			label->appendText(acc);
		}
	}

	if(keyboard->keyJustUp(GLFW_KEY_F11)){
		game->toggleFullScreen();
	}
	
	if(keyboard->keyJustUp(GLFW_KEY_F)){
		firstPerson = !firstPerson;
	}
	
	if(keyboard->keyJustUp(GLFW_KEY_G)){
		addThing();
	}
	if(keyboard->keyJustUp(GLFW_KEY_R)){
		static_cast<ShaderComponentText *>(textShader->getComponentAt(0))->setColor(glm::vec3(1, 0.1, 0.2));
	}
	if(keyboard->keyJustUp(GLFW_KEY_B)){
		static_cast<ShaderComponentText *>(textShader->getComponentAt(0))->setColor(glm::vec3(0.2, 0.1, 1));
	}

	// camera controls
	if (keyboard->keyDown(GLFW_KEY_UP)){
		activeCamera->parent->translate((activeCamera->forwardVectorRotated) * static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}
	if (keyboard->keyDown(GLFW_KEY_DOWN)){
		activeCamera->parent->translate((activeCamera->forwardVectorRotated) * -static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}
	if (keyboard->keyDown(GLFW_KEY_LEFT)){
		activeCamera->parent->translate((activeCamera->rightVectorRotated) * -static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}
	if (keyboard->keyDown(GLFW_KEY_RIGHT)){
		activeCamera->parent->translate((activeCamera->rightVectorRotated) * static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}

	if(firstPerson){
		float playerSpeed = 2.5f;
		float mass = player->body->GetMass();
		float angle = atan2(mouseCam->forwardVectorRotated.y, mouseCam->forwardVectorRotated.x);

		if(activeCamera != mouseCam){
			angle = glm::radians(90.f);
		}

		mouseCam->parent->translate(player->getWorldPos() + glm::vec3(0, 0, player->parent->getScaleVector().z*1.25f), false);
		mouseCam->lookAtOffset = glm::vec3(0, 0, -player->parent->getScaleVector().z*0.25f);
		
		
		if (keyboard->keyDown(GLFW_KEY_W)){
			player->applyLinearImpulseUp(playerSpeed * mass * sin(angle));
			player->applyLinearImpulseRight(playerSpeed * mass * cos(angle));
		}
		if (keyboard->keyDown(GLFW_KEY_S)){
			player->applyLinearImpulseDown(playerSpeed * mass * sin(angle));
			player->applyLinearImpulseLeft(playerSpeed * mass * cos(angle));
		}
		if (keyboard->keyDown(GLFW_KEY_A)){
			player->applyLinearImpulseUp(playerSpeed * mass * cos(angle));
			player->applyLinearImpulseLeft(playerSpeed * mass * sin(angle));
		}
		if (keyboard->keyDown(GLFW_KEY_D)){
			player->applyLinearImpulseDown(playerSpeed * mass * cos(angle));
			player->applyLinearImpulseRight(playerSpeed * mass * sin(angle));
		}
		
		// correct joystick controls for first-person
		Joystick * one = joy->joysticks[0];
		if(one != nullptr){
			float x = playerSpeed * mass * cos(angle) * -one->getAxis(Joystick::xbox_axes::kLY) +
				playerSpeed * mass * sin(angle) * one->getAxis(Joystick::xbox_axes::kLX);
			float y = playerSpeed * mass * sin(angle) * -one->getAxis(Joystick::xbox_axes::kLY) +
				playerSpeed * mass * cos(angle) * -one->getAxis(Joystick::xbox_axes::kLX);

			player->applyLinearImpulseUp(y);
			player->applyLinearImpulseRight(x);

			
			float x2 = one->getAxis(Joystick::xbox_axes::kRX)*100;
			float y2 = one->getAxis(Joystick::xbox_axes::kRY)*100;
			mouse->translate(glm::vec2(x2, y2));
		}
	}

	// debug controls
	if(keyboard->keyJustDown(GLFW_KEY_1)){
		if(activeCamera == gameCam){
			activeCamera = mouseCam;
		}else if(activeCamera == mouseCam){
			activeCamera = debugCam;
		}else{
			activeCamera = gameCam;
		}
	}
	if(keyboard->keyJustUp(GLFW_KEY_2)){
		if(box2dDebugDrawer != nullptr){
			box2dWorld->b2world->SetDebugDraw(nullptr);
			childTransform->removeChild(box2dDebugDrawer->parent);
			delete box2dDebugDrawer->parent;
			box2dDebugDrawer = nullptr;
		}else{
			box2dDebugDrawer = new Box2DDebugDrawer(box2dWorld);
			box2dWorld->b2world->SetDebugDraw(box2dDebugDrawer);
			box2dDebugDrawer->drawing = true;
			//drawer->AppendFlags(b2Draw::e_aabbBit);
			box2dDebugDrawer->AppendFlags(b2Draw::e_shapeBit);
			box2dDebugDrawer->AppendFlags(b2Draw::e_centerOfMassBit);
			box2dDebugDrawer->AppendFlags(b2Draw::e_jointBit);
			//drawer->AppendFlags(b2Draw::e_pairBit);
			childTransform->addChild(box2dDebugDrawer);
		}

		if(debugDrawer != nullptr){
			bulletWorld->setDebugDrawer(nullptr);
			childTransform->removeChild(debugDrawer->parent);
			delete debugDrawer->parent;
			debugDrawer = nullptr;
		}else{
			debugDrawer = new BulletDebugDrawer(bulletWorld);
			childTransform->addChild(debugDrawer);
			debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
			bulletWorld->setDebugDrawer(debugDrawer);
		}
	}
	
	// update scene and physics
	box2dWorld->update(_step);
	bulletWorld->stepSimulation(_step->deltaTime);
	Scene::update(_step);

	// manually snap the cubes to the bullet boxes (won't be needed when we have a proper class for bullet physics objects)
	for(unsigned long int i = 0; i < bodies.size(); ++i){
		btTransform t = bodies.at(i)->getWorldTransform();
		btVector3 v = t.getOrigin();
		btQuaternion q = t.getRotation();
		bodies2.at(i)->parent->translate(v.x(), v.y(), v.z(), false);
		bodies2.at(i)->parent->setOrientation(glm::quat(q.w(), q.x(), q.y(), q.z()));
	}

	// update ui stuff
	glm::uvec2 sd = vox::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	uiLayer.update(_step);

	glm::vec3 sp = activeCamera->worldToScreen(player->getWorldPos(), sd);
	if(sp.z < 0){
		sp.z = activeCamera->farClip * 2;
	}
	playerIndicator->parent->translate(sp, false);
	crosshair->parent->translate(sd.x/2.f, sd.y/2.f, 0, false);
	mouseIndicator->parent->translate(sd.x - mouse->mouseX(), sd.y - mouse->mouseY(), 0, false);
}

void PD_TestScene::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	clear();
	
	float scale = 1;
	game->setViewport(0, 0, game->viewPortWidth * 1 / scale, game->viewPortHeight * 1 / scale);

	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);

	//Bind frameBuffer
	screenFBO->bindFrameBuffer();
	//render the scene to the buffer
	Scene::render(_matrixStack, _renderOptions);
	game->setViewport(0, 0, game->viewPortWidth*scale, game->viewPortHeight*scale);

	//Render the buffer to the render surface
	screenSurface->render(screenFBO->getTextureId());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_TestScene::load(){
	Scene::load();	

	screenSurface->load();
	screenFBO->load();
	uiLayer.load();
}

void PD_TestScene::unload(){
	uiLayer.unload();
	screenFBO->unload();
	screenSurface->unload();

	Scene::unload();	
}