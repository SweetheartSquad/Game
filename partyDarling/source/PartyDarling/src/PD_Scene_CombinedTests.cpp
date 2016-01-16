#pragma once

#include <PD_Scene_CombinedTests.h>
#include <PD_ResourceManager.h>
#include <PD_Game.h>
#include <PD_Assets.h>
#include <Resource.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>
#include <shader/ShaderComponentIndexedTexture.h>
#include <shader/ShaderComponentDepthOffset.h>
#include <shader/ComponentShaderText.h>

#include <NumberUtils.h>
#include <StringUtils.h>
#include <TextureUtils.h>
#include <sweet/UI.h>

#include <MousePerspectiveCamera.h>
#include <MeshFactory.h>
#include <Timeout.h>

#include <RenderOptions.h>
#include <json\json.h>

#include <sweet/Input.h>
#include <PD_FurnitureParser.h>
#include <PD_Furniture.h>
#include <PointLight.h>

PD_Scene_CombinedTests::PD_Scene_CombinedTests(Game * _game) :
	Scene(_game),
	uiLayer(0,0,0,0),
	shader(new ComponentShaderBase(false)),
	characterShader(new ComponentShaderBase(false)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr),
	currentHoverTarget(nullptr),
	selectedItem(nullptr)
{
	shader->addComponent(new ShaderComponentMVP(shader));
	shader->addComponent(new ShaderComponentTexture(shader, 0));
	shader->addComponent(new ShaderComponentDiffuse(shader));
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

	for(unsigned long int i = 0; i < 50; ++i){
		std::map<std::string, Asset *>::iterator itemDef = PD_ResourceManager::scenario->assets["item"].begin();
		if(itemDef != PD_ResourceManager::scenario->assets["item"].end()){
			std::advance(itemDef, sweet::NumberUtils::randomInt(0, PD_ResourceManager::scenario->assets["item"].size()-1));
			PD_Item * item = dynamic_cast<AssetItem *>(itemDef->second)->getItem(bulletWorld, shader);
			item->addToWorld();
			childTransform->addChild(item);
	
			item->setTranslationPhysical(sweet::NumberUtils::randomFloat(-50, 50), 2, sweet::NumberUtils::randomFloat(-50, 50));
			item->rotatePhysical(45,0,1,0,false);
		}
	}
	

	uiBubble = new PD_UI_Bubble(uiLayer.world);
	uiLayer.addChild(uiBubble);

	uiBubble->addOption("test", nullptr);

	uiInventory = new PD_UI_Inventory(uiLayer.world);
	uiLayer.addChild(uiInventory);
	uiInventory->eventManager.addEventListener("itemSelected", [this](sweet::Event * _event){
		uiInventory->close();
		uiLayer.removeMouseIndicator();

		// replace the crosshair texture with the item texture
		crosshairIndicator->background->mesh->replaceTextures(uiInventory->getSelected()->mesh->textures.at(0));
		// TODO: update the UI to indicate the selected item to the player
	});

	uiDialogue = new PD_UI_Dialogue(uiLayer.world, uiBubble);
	uiLayer.addChild(uiDialogue);
	uiDialogue->setRationalHeight(1.f, &uiLayer);
	uiDialogue->setRationalWidth(1.f, &uiLayer);
	uiDialogue->eventManager.addEventListener("end", [this](sweet::Event * _event){
		player->enable();
	});

	uiYellingContest = new PD_UI_YellingContest(uiLayer.world, PD_ResourceManager::scenario->defaultFont->font, uiBubble->textShader, uiLayer.shader);
	uiLayer.addChild(uiYellingContest);
	uiYellingContest->setRationalHeight(1.f, &uiLayer);
	uiYellingContest->setRationalWidth(1.f, &uiLayer);
	uiYellingContest->eventManager.addEventListener("complete", [this](sweet::Event * _event){
		uiYellingContest->disable();
		player->enable();
	});

	PersonRenderer * testCharacter = new PersonRenderer(bulletWorld);
	childTransform->addChild(testCharacter);
	testCharacter->setShader(characterShader, true);
	testCharacter->unload();
	testCharacter->load();
	testCharacter->firstParent()->scale(0.001f);
	testCharacter->firstParent()->translate(0, 2, 0);
	testCharacter->butt->setTranslationPhysical(0, 2, 0, true);
	testCharacter->state = &dynamic_cast<AssetCharacter *>(PD_ResourceManager::scenario->getAsset("character", "1"))->states.at(0);





	// add a ground to the scene
	BulletMeshEntity * bulletGround = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh(), shader);
	bulletGround->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletGround->createRigidBody(0);
	childTransform->addChild(bulletGround);
	bulletGround->meshTransform->scale(1000,1000,1000);
	bulletGround->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletGround->body->translate(btVector3(0, 0, 0));
	bulletGround->body->setFriction(1);
	bulletGround->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GREY")->texture);

	// add the player to the scene
	player = new Player(bulletWorld);
	childTransform->addChild(player);
	cameras.push_back(player->playerCamera);
	activeCamera = player->playerCamera;
	childTransform->addChild(player->playerCamera);
	player->playerCamera->firstParent()->translate(0, 5, 0);

	std::vector<PD_FurnitureDefinition*> * definitions = PD_FurnitureParser::parseFurnitureDefinitions();
	PD_FurnitureComponentContainer * components = PD_FurnitureParser::parseFurnitureComponents();
	
	sweet::NumberUtils::seed(3425577115);

	// Add some generated furniture
	for(unsigned long int i = 0; i < 50; ++i) {
		int randIdx = sweet::NumberUtils::randomInt(0, definitions->size() - 1);
		auto furn = new PD_Furniture(bulletWorld, shader, definitions->at(randIdx), components);
		furn->meshTransform->scale(0.15f, 0.15f, 0.15f);
		furn->freezeTransformation();
		furn->setColliderAsBoundingBox();
		furn->createRigidBody(1.0f);
		furn->setTranslationPhysical(i * 10.0f, furn->mesh->calcBoundingBox().height * 0.5f, 0.f);
		childTransform->addChild(furn);
	}

	PointLight * light2 = new PointLight(glm::vec3(5,5,5), 0.02f, 0.001f, -1);
	childTransform->addChild(light2);
	light2->firstParent()->translate(0.f, 20.f, 0.f);
	lights.push_back(light2);
}

PD_Scene_CombinedTests::~PD_Scene_CombinedTests(){
	deleteChildTransform();
}

void PD_Scene_CombinedTests::update(Step * _step){
	bulletWorld->update(_step);





	// mouse interaction with world objects
	if(player->isEnabled()){
		float range = 4;
		glm::vec3 pos = activeCamera->childTransform->getWorldPos();
		btVector3 start(pos.x, pos.y, pos.z);
		btVector3 dir(activeCamera->forwardVectorRotated.x, activeCamera->forwardVectorRotated.y, activeCamera->forwardVectorRotated.z);
		btVector3 end = start + dir*range;
		btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
		bulletWorld->world->rayTest(start, end, RayCallback);
		
		
		NodeBulletBody * lastHoverTarget = currentHoverTarget;
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
					PersonButt * butt = dynamic_cast<PersonButt *>(me);
					if(butt != nullptr){
						// hover over person
						if(butt != currentHoverTarget){
							// if we aren't already looking at the person,
							// clear out the bubble UI and add the relevant options
							uiBubble->clear();
							uiBubble->addOption("talk to ", [this, butt](sweet::Event * _event){
								uiDialogue->startEvent(PD_ResourceManager::scenario->getConversation(butt->person->state->conversation)->conversation);
								player->disable();
							});
							uiBubble->addOption("yell at ", [this](sweet::Event * _event){
								uiYellingContest->startNewFight();
								uiBubble->clear();
								player->disable();
								// TODO: pass in the character that's fighting here
							});
							// if we have an item, also add the "use on" option
							if(uiInventory->getSelected() != nullptr){
								uiBubble->addOption("use item on character", [this](sweet::Event * _event){
									uiBubble->clear();
									player->disable();
									// TODO: pass in the character that's fighting here
								});
							}
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
		if((lastHoverTarget != currentHoverTarget) && currentHoverTarget == nullptr || selectedItem != uiInventory->getSelected()){
			uiBubble->clear();
			selectedItem = uiInventory->getSelected();
			if(uiInventory->getSelected() != nullptr){
				uiBubble->addOption("use item on self", [this](sweet::Event * _event){
					//uiBubble->clear();
					//player->disable();
					// TODO: actually trigger item interaction
				});
				uiBubble->addOption("drop item", [this](sweet::Event * _event){
					//uiBubble->clear();

					// dropping an item
					if(PD_Item * item = uiInventory->removeSelected()){
						// put the item back into the scene
						childTransform->addChild(item);
						item->addToWorld();
			
						// figure out where to put the item
						glm::vec3 targetPos = activeCamera->getWorldPos() + activeCamera->forwardVectorRotated * 3.f;
						targetPos.y = 2; // always put stuff on the ground
						item->setTranslationPhysical(targetPos, false);
						// rotate the item to face the camera
						item->rotatePhysical(activeCamera->yaw - 90,0,1,0, false);
					}

					// replace the crosshair itme texture with the actual crosshair texture
					crosshairIndicator->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);


				});
			}
		}
	}
	
	// inventory toggle
	if(keyboard->keyJustDown(GLFW_KEY_TAB)){
		if(uiInventory->isVisible()){
			uiInventory->close();
			uiLayer.removeMouseIndicator();
		}else{
			uiInventory->open();
			uiLayer.addMouseIndicator();
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

	// debug controls
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
	
	

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);
	uiLayer.update(_step);
}

void PD_Scene_CombinedTests::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_Scene_CombinedTests::load(){
	Scene::load();	
	uiLayer.load();
}

void PD_Scene_CombinedTests::unload(){
	uiLayer.unload();
	Scene::unload();	
}