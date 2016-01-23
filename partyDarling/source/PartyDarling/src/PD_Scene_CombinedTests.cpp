#pragma once

#include <PD_Scene_CombinedTests.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <Resource.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>
#include <shader/ShaderComponentIndexedTexture.h>
#include <shader/ShaderComponentDepthOffset.h>
#include <shader/ComponentShaderText.h>
#include <shader/ShaderComponentToon.h>
#include <PD_ShaderComponentSpecialToon.h>

#include <NumberUtils.h>
#include <StringUtils.h>
#include <TextureUtils.h>
#include <sweet/UI.h>

#include <MousePerspectiveCamera.h>
#include <MeshFactory.h>
#include <Timeout.h>

#include <RenderOptions.h>
#include <json\json.h>
#include <RampTexture.h>

#include <sweet/Input.h>
#include <PD_FurnitureParser.h>
#include <PD_Furniture.h>
#include <PointLight.h>

#include <Room.h>
#include <RoomBuilder.h>
#include <RenderSurface.h>

Colour PD_Scene_CombinedTests::wipeColour(glm::ivec3(0));

PD_Scene_CombinedTests::PD_Scene_CombinedTests(PD_Game * _game) :
	Scene(_game),
	toonShader(new ComponentShaderBase(false)),
	uiLayer(0,0,0,0),
	characterShader(new ComponentShaderBase(false)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr),
	selectedItem(nullptr),
	screenSurfaceShader(new Shader("assets/RenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	currentHoverTarget(nullptr),
	lightStart(0.3f),
	lightEnd(1.f),
	lightIntensity(1.f),
	transition(0.f),
	transitionTarget(1.f)
{
	toonRamp = new RampTexture(lightStart, lightEnd, 4);
	toonShader->addComponent(new ShaderComponentMVP(toonShader));
	toonShader->addComponent(new PD_ShaderComponentSpecialToon(toonShader, toonRamp, true));
	//toonShader->addComponent(new ShaderComponentDiffuse(toonShader));
	toonShader->addComponent(new ShaderComponentTexture(toonShader, 0));
	toonShader->compileShader();


	characterShader->addComponent(new ShaderComponentMVP(characterShader));
	characterShader->addComponent(new PD_ShaderComponentSpecialToon(characterShader, toonRamp, true));
	//characterShader->addComponent(new ShaderComponentDiffuse(characterShader));
	characterShader->addComponent(new ShaderComponentIndexedTexture(characterShader));
	characterShader->addComponent(new ShaderComponentDepthOffset(characterShader));
	characterShader->compileShader();

	screenSurfaceShader->referenceCount++;
	screenFBO->referenceCount++;
	screenSurface->referenceCount++;

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);


	// remove initial camera
	/*childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();*/

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

	/*for(unsigned long int i = 0; i < 50; ++i){
		std::map<std::string, Asset *>::iterator itemDef = PD_ResourceManager::scenario->assets["item"].begin();
		if(itemDef != PD_ResourceManager::scenario->assets["item"].end()){
			std::advance(itemDef, sweet::NumberUtils::randomInt(0, PD_ResourceManager::scenario->assets["item"].size()-1));
			PD_Item * item = dynamic_cast<AssetItem *>(itemDef->second)->getItem(bulletWorld, shader);
			item->addToWorld();
			childTransform->addChild(item);
	
			item->setTranslationPhysical(sweet::NumberUtils::randomFloat(-50, 50), 2, sweet::NumberUtils::randomFloat(-50, 50));
			item->rotatePhysical(45,0,1,0,false);
		}
	}*/
	

	uiBubble = new PD_UI_Bubble(uiLayer.world);
	uiLayer.addChild(uiBubble);

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


	// add the player to the scene
	player = new Player(bulletWorld);
	childTransform->addChild(player);
	cameras.push_back(player->playerCamera);
	activeCamera = player->playerCamera;
	childTransform->addChild(player->playerCamera);
	player->playerCamera->firstParent()->translate(0, 5, 0);

	playerLight = new PointLight(glm::vec3(lightIntensity), 0.0f, 0.003f, -1);
	player->playerCamera->childTransform->addChild(playerLight);
	playerLight->firstParent()->translate(0.f, 1.f, 0.f);
	lights.push_back(playerLight);


	// pick a random room to load
	std::stringstream ss;
	ss << sweet::NumberUtils::randomInt(1, 4);
	Room * room = RoomBuilder(dynamic_cast<AssetRoom *>(PD_ResourceManager::scenario->getAsset("room",ss.str())), bulletWorld, toonShader, characterShader).getRoom();
	childTransform->addChild(room);

	std::vector<RoomObject *> components = room->getAllComponents();
	for(unsigned int i = 0; i < components.size(); ++i){
		childTransform->addChild(components.at(i));
	}



	// setup event listeners
	PD_ResourceManager::scenario->eventManager.addEventListener("changeState", [](sweet::Event * _event){
		std::stringstream characterName;
		characterName << (int)glm::round(_event->getFloatData("Character"));
		std::stringstream stateName;
		stateName << (int)glm::round(_event->getFloatData("State"));
		std::cout << characterName.str() << "'s state changed to " << stateName.str() << std::endl;

		Scenario * scenario = PD_ResourceManager::scenario;
		PD_Listing * listing = PD_Listing::listings[PD_ResourceManager::scenario];
		Person * character = listing->characters[characterName.str()];
		character->state = &character->definition->states.at(stateName.str());
	});

	PD_ResourceManager::scenario->eventManager.addEventListener("reset", [_game, this](sweet::Event * _event){
		player->disable();
		transitionTarget = 0;
		
		screenSurfaceShader->bindShader();
		wipeColour = Colour::getRandomFromHsvRange(glm::ivec3(0, 90, 90), glm::ivec3(360, 100, 100));
		
		Timeout * t = new Timeout(1.f, [_game](sweet::Event * _event){
			std::stringstream ss;
			ss << "COMBINED_TEST_" << sweet::lastTimestamp;
			_game->scenes[ss.str()] = new PD_Scene_CombinedTests(dynamic_cast<PD_Game *>(_game));
			_game->switchScene(ss.str(), false); // TODO: fix memory issues so that this can be true
		});
		t->start();
		childTransform->addChild(t, false);

		PD_ResourceManager::scenario->eventManager.listeners.clear();
	});
}

PD_Scene_CombinedTests::~PD_Scene_CombinedTests(){
	deleteChildTransform();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
	screenSurface->decrementAndDelete();
}

void PD_Scene_CombinedTests::update(Step * _step){
	// party lights!
	float a = playerLight->getAttenuation();
	float newa = fmod(_step->time, 142.f/300.f)*0.01f+0.01f;
	playerLight->setAttenuation(newa);
	if(newa < a){
		lightStart = glm::vec3(sweet::NumberUtils::randomFloat(0.3f, 0.5f),sweet::NumberUtils::randomFloat(0.3f, 0.5f),sweet::NumberUtils::randomFloat(0.3f, 0.5f));
		lightEnd = glm::vec3(sweet::NumberUtils::randomFloat(0.9f, 1.5f),sweet::NumberUtils::randomFloat(0.9f, 1.5f),sweet::NumberUtils::randomFloat(0.9f, 1.5f));
		lightIntensity = sweet::NumberUtils::randomFloat(1.f, 1.25f);
	}
	toonRamp->setRamp(
		toonRamp->start + (lightStart - toonRamp->start) * 0.1f,
		toonRamp->end + (lightEnd - toonRamp->end) * 0.1f,
		4);
	toonRamp->bufferData();
	playerLight->setIntensities(playerLight->getIntensities() + (glm::vec3(lightIntensity) - playerLight->getIntensities() * 0.1f));


	// screen surface update
	float transitionDelta = transitionTarget - transition;
	if(glm::abs(transitionDelta) >= FLT_EPSILON){
		transition += transitionDelta * 0.1f;
		if(transition + FLT_EPSILON >= 1.f){
			transition = 1.f;
		}
		screenSurfaceShader->bindShader();
		GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "transition");
		checkForGlError(0,__FILE__,__LINE__);
		if(test != -1){
			glUniform1f(test, transition);
			checkForGlError(0,__FILE__,__LINE__);
		}
	}
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "wipeColour");
	checkForGlError(0,__FILE__,__LINE__);
	if(test != -1){
		glUniform3f(test, wipeColour.r/255.f, wipeColour.g/255.f, wipeColour.b/255.f);
		checkForGlError(0,__FILE__,__LINE__);
	}



	PD_ResourceManager::scenario->eventManager.update(_step);

	bulletWorld->update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_R)){
		PD_ResourceManager::scenario->eventManager.triggerEvent("reset");
	}

	if(keyboard->keyJustDown(GLFW_KEY_L)){
		screenSurfaceShader->unload();
		screenSurfaceShader->loadFromFile(screenSurfaceShader->vertSource, screenSurfaceShader->fragSource);
		screenSurfaceShader->load();
	}

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
								uiBubble->addOption("Pickup " + item->definition->name, [this, item](sweet::Event * _event){
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

									// run item pickup triggers
									item->triggerPickup();
								});
							}else{
								uiBubble->addOption("Use " + item->definition->name, [item](sweet::Event * _event){
									std::cout << "hey gj you interacted" << std::endl;

									// run item interact triggers
									item->triggerInteract();
								});
							}
						}
					}else{
						// we hovered over an item, but it wasn't pixel-perfect
						me = item = nullptr;
					}
				}else{
					Person * person = dynamic_cast<Person*>(me);
					if(person != nullptr){
						// hover over person
						if(person != currentHoverTarget){
							// if we aren't already looking at the person,
							// clear out the bubble UI and add the relevant options
							uiBubble->clear();
							uiBubble->addOption("Talk to " + person->definition->name, [this, person](sweet::Event * _event){
								uiDialogue->startEvent(PD_ResourceManager::scenario->getConversation(person->state->conversation)->conversation);
								player->disable();
							});
							uiBubble->addOption("Yell at " + person->definition->name, [this](sweet::Event * _event){
								uiYellingContest->startNewFight();
								uiBubble->clear();
								player->disable();
								// TODO: pass in the character that's fighting here
							});
							// if we have an item, also add the "use on" option
							if(uiInventory->getSelected() != nullptr){
								uiBubble->addOption("Use " + uiInventory->getSelected()->definition->name + " on " + person->definition->name, [this](sweet::Event * _event){
									uiBubble->clear();
									player->disable();
									// TODO: pass in the character that's interacting with the item here
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
				uiBubble->addOption("Use " + uiInventory->getSelected()->definition->name, [this](sweet::Event * _event){
					//uiBubble->clear();
					//player->disable();
					// TODO: actually trigger item interaction
				});
				uiBubble->addOption("Drop " + uiInventory->getSelected()->definition->name, [this](sweet::Event * _event){
					//uiBubble->clear();

					// dropping an item
					if(PD_Item * item = uiInventory->removeSelected()){
						// put the item back into the scene
						childTransform->addChild(item);
						item->addToWorld();
			
						// figure out where to put the item
						glm::vec3 targetPos = activeCamera->getWorldPos() + activeCamera->forwardVectorRotated * 3.f;
						targetPos.y = ITEM_POS_Y; // always put stuff on the ground
						item->translatePhysical(targetPos, false);
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
	if(keyboard->keyJustDown(GLFW_KEY_P)){
		dynamic_cast<PD_Game*>(game)->playBGM();
	}if(keyboard->keyJustDown(GLFW_KEY_O)){
		dynamic_cast<PD_Game*>(game)->playFight();
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
	uiLayer.resize(0,sd.x,0,sd.y);
	uiLayer.update(_step);
}

void PD_Scene_CombinedTests::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
	screenFBO->bindFrameBuffer();
	
	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();

	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
	
	screenSurface->render(screenFBO->getTextureId());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PD_Scene_CombinedTests::load(){
	Scene::load();	
	uiLayer.load();
	screenSurface->load();
	screenSurfaceShader->load();
	screenFBO->load();
}

void PD_Scene_CombinedTests::unload(){
	uiLayer.unload();
	Scene::unload();	
	screenSurface->unload();
	screenSurfaceShader->unload();
	screenFBO->unload();
}