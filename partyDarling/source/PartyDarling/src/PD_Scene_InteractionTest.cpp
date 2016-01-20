#pragma once

#include <PD_Scene_InteractionTest.h>
#include <PD_ResourceManager.h>
#include <PD_Game.h>
#include <PD_Assets.h>
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

PD_Scene_InteractionTest::PD_Scene_InteractionTest(Game * _game) :
	Scene(_game),
	uiLayer(0,0,0,0),
	shader(new ComponentShaderBase(false)),
	characterShader(new ComponentShaderBase(false)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr),
	currentHoverTarget(nullptr)
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
	

	uiBubble = new PD_UI_Bubble(uiLayer.world);
	uiLayer.addChild(uiBubble);

	uiBubble->addOption("test", nullptr);

	uiInventory = new PD_UI_Inventory(uiLayer.world);
	uiLayer.addChild(uiInventory);
	uiInventory->eventManager.addEventListener("itemSelected", [this](sweet::Event * _event){
		uiInventory->close();

		// replace the crosshair texture with the item texture
		crosshairIndicator->background->mesh->replaceTextures(uiInventory->getSelected()->mesh->textures.at(0));
		// TODO: update the UI to indicate the selected item to the player
	});

	uiDialogue = new PD_UI_Dialogue(uiLayer.world, uiBubble);
	uiLayer.addChild(uiDialogue);
	uiDialogue->setRationalHeight(1.f, &uiLayer);
	uiDialogue->setRationalWidth(1.f, &uiLayer);

	PersonRenderer * testCharacter = dynamic_cast<AssetCharacter*>(PD_ResourceManager::scenario->getAsset("character", "1"))->getCharacter(bulletWorld, characterShader);
	childTransform->addChild(testCharacter);
	testCharacter->setShader(characterShader, true);
	testCharacter->unload();
	testCharacter->load();
	testCharacter->firstParent()->scale(0.005f);
}

PD_Scene_InteractionTest::~PD_Scene_InteractionTest(){
	deleteChildTransform();
}

void PD_Scene_InteractionTest::update(Step * _step){
	bulletWorld->update(_step);





	// mouse interaction with world objects
	
	float range = 10;
	glm::vec3 pos = activeCamera->childTransform->getWorldPos();
	btVector3 start(pos.x, pos.y, pos.z);
	btVector3 dir(activeCamera->forwardVectorRotated.x, activeCamera->forwardVectorRotated.y, activeCamera->forwardVectorRotated.z);
	btVector3 end = start + dir*range;
	btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
	bulletWorld->world->rayTest(start, end, RayCallback);
	if(RayCallback.hasHit()){
		NodeBulletBody * me = static_cast<NodeBulletBody *>(RayCallback.m_collisionObject->getUserPointer());
			
		if(me != nullptr){
			PD_Item * item = dynamic_cast<PD_Item *>(me);
			if(item != nullptr){
				if(item->actuallyHovered(glm::vec3(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ()))){
					// hover over item
					if(item != currentHoverTarget){
						// if we aren't already looking at the item,
						// clear out the bubble UI and add the relevant options
						uiBubble->clear();
						if(item->definition->collectable){
							uiBubble->addOption("pickup", [this, item](sweet::Event * _event){
								// remove the item from the scene
								Transform * toDelete = item->firstParent();
								toDelete->firstParent()->removeChild(toDelete);
								toDelete->removeChild(item);
								delete toDelete;
								delete item->shape;
								item->shape = nullptr;
								bulletWorld->world->removeRigidBody(item->body);
								item->body = nullptr;

								// pickup the item
								uiInventory->pickupItem(item);
							});
						}else{
							uiBubble->addOption("interact", [](sweet::Event * _event){
								std::cout << "hey gj you interacted" << std::endl;
							});
						}
					}
				}else{
					// we hovered over an item, but it wasn't pixel-perfect
					me = item = nullptr;
				}
			}else{
				Person * person = dynamic_cast<Person *>(me);
				if(person != nullptr){
					// hover over person
					if(person != currentHoverTarget){
						// if we aren't already looking at the person,
						// clear out the bubble UI and add the relevant options
						uiBubble->clear();
						uiBubble->addOption("talk to ", [this, person](sweet::Event * _event){
							uiDialogue->startEvent(PD_ResourceManager::scenario->getConversation(person->state->conversation)->conversation);
							std::cout << "hey gj you talked" << std::endl;
						});
						uiBubble->addOption("yell at ", [](sweet::Event * _event){
							std::cout << "hey gj you yelled" << std::endl;
						});
						uiBubble->placeOptions();
						uiBubble->reorderChildren();
					}
				}
			}
		}

		/*NodeUI * ui = dynamic_cast<NodeUI *>(me);
		if(ui != nullptr){
			ui->setUpdateState(true);
		}*/
		

		currentHoverTarget = me;
	}else{
		currentHoverTarget = nullptr;
	}
	if(currentHoverTarget == nullptr){
		uiBubble->clear();
	}

	if(keyboard->keyJustDown(GLFW_KEY_D)){

		// dropping an item

		if(PD_Item * item = uiInventory->removeSelected()){
			// put the item back into the scene
			childTransform->addChild(item);
			item->addToWorld();
			
			// figure out where to put the item
			glm::vec3 targetPos = activeCamera->getWorldPos() + activeCamera->forwardVectorRotated * 3.f;
			targetPos.y = 0; // always put stuff on the ground
			item->setTranslationPhysical(targetPos, false);
			// rotate the item to face the camera
			item->rotatePhysical(activeCamera->yaw - 90,0,1,0, false);
		}

		// replace the crosshair itme texture with the actual crosshair texture
		crosshairIndicator->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	}
	
	// inventory toggle
	if(keyboard->keyJustDown(GLFW_KEY_TAB)){
		if(uiInventory->isVisible()){
			uiInventory->close();
		}else{
			uiInventory->open();
		}
	}

	// bubble testing controls
	if(keyboard->keyJustDown(GLFW_KEY_V)){
		uiBubble->next();
	}if(keyboard->keyJustDown(GLFW_KEY_B)){
		uiBubble->prev();
	}if(keyboard->keyJustDown(GLFW_KEY_N)){
		uiBubble->addOption("test", nullptr);
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

void PD_Scene_InteractionTest::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_Scene_InteractionTest::load(){
	Scene::load();	
	uiLayer.load();
}

void PD_Scene_InteractionTest::unload(){
	uiLayer.unload();
	Scene::unload();	
}