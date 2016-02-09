#pragma once

#include <PD_Scene_Main.h>
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

#include <PD_PhraseGenerator_Incidental.h>

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

Colour PD_Scene_Main::wipeColour(glm::ivec3(125/255.f,200/255.f,50/255.f));

PD_Scene_Main::PD_Scene_Main(PD_Game * _game) :
	Scene(_game),
	toonShader(new ComponentShaderBase(false)),
	uiLayer(0,0,0,0),
	characterShader(new ComponentShaderBase(false)),
	emoteShader(new ComponentShaderBase(false)),
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
	transitionTarget(1.f),
	currentRoom(nullptr),
	currentHousePosition(0)
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

	emoteShader->addComponent(new ShaderComponentMVP(emoteShader));
	emoteShader->addComponent(new ShaderComponentTexture(emoteShader));
	emoteShader->addComponent(new ShaderComponentDepthOffset(emoteShader));
	emoteShader->compileShader();

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
	crosshairIndicator->invalidateLayout();
	for(auto & v : crosshairIndicator->background->mesh->vertices){
		v.x -= 0.5f;
		v.y -= 0.5f;
	}crosshairIndicator->background->mesh->dirty = true;
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
		player->enable();

		// replace the crosshair texture with the item texture
		Texture * itemTex = uiInventory->getSelected()->mesh->textures.at(0);
		crosshairIndicator->background->mesh->replaceTextures(itemTex);
		crosshairIndicator->setWidth(itemTex->width);
		crosshairIndicator->setHeight(itemTex->height);
		crosshairIndicator->invalidateLayout();
		// TODO: update the UI to indicate the selected item to the player
	});

	uiDialogue = new PD_UI_Dialogue(uiLayer.world, uiBubble);
	uiLayer.addChild(uiDialogue);
	uiDialogue->setRationalHeight(1.f, &uiLayer);
	uiDialogue->setRationalWidth(1.f, &uiLayer);
	uiDialogue->eventManager.addEventListener("end", [this](sweet::Event * _event){
		player->enable();
	});

	uiYellingContest = new PD_UI_YellingContest(uiLayer.world, PD_ResourceManager::scenario->getFont("FIGHT-FONT")->font, uiBubble->textShader, uiLayer.shader);
	uiLayer.addChild(uiYellingContest);
	uiYellingContest->setRationalHeight(1.f, &uiLayer);
	uiYellingContest->setRationalWidth(1.f, &uiLayer);
	uiYellingContest->eventManager.addEventListener("complete", [this](sweet::Event * _event){
		uiYellingContest->disable();
		player->enable();
	});
	uiYellingContest->eventManager.addEventListener("interject", [this](sweet::Event * _event){
		player->shakeIntensity = 0.3f;
		if(!_event->getIntData("success")){
			player->shakeTimeout->restart();
		}
	});
	uiYellingContest->eventManager.addEventListener("insult", [this](sweet::Event * _event){
		player->shakeIntensity = 0.3f;
		if(!_event->getIntData("success")){
			player->shakeTimeout->restart();
		}
	});
	uiYellingContest->eventManager.addEventListener("miss", [this](sweet::Event * _event){
		player->shakeIntensity = 0.1f;
		player->shakeTimeout->restart();
	});

	uiMap = new PD_UI_Map(uiLayer.world, PD_ResourceManager::scenario->getFont("FONT")->font, uiBubble->textShader);
	uiLayer.addChild(uiMap);
	uiMap->setRationalHeight(1.f, &uiLayer);
	uiMap->setRationalWidth(1.f, &uiLayer);
	uiMap->disable();

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


	std::map<std::string, std::function<bool(sweet::Event *)>> * ff;
	
	// Set the scenario condition implentations pointer
	PD_ResourceManager::scenario->conditionImplementations = PD_ResourceManager::conditionImplementations;

	// Setup conditions

	// TODO - Actually implement this
	(*PD_ResourceManager::conditionImplementations)["checkState"] = [this](sweet::Event * _event){
		//Check if the character is in a certain state.
		// CHARACTER character
		// CHARACTER_STATE state

		return false;
	};

	// setup event listeners
	PD_ResourceManager::scenario->eventManager.addEventListener("changeState", [](sweet::Event * _event){
		std::stringstream characterName;
		characterName << (int)glm::round(_event->getFloatData("Character"));
		std::stringstream stateName;
		stateName << (int)glm::round(_event->getFloatData("State"));
		std::cout << characterName.str() << "'s state changed to " << stateName.str() << std::endl;

		PD_Listing * listing = PD_Listing::listingsById[_event->getStringData("scenario")];
		Person * character = listing->characters[characterName.str()];
		if(character == nullptr){
			Log::warn("Character not found in state change event");
		}else{
			character->state = &character->definition->states.at(stateName.str());
			character->pr->setAnimation(character->state->animation);
		}
	});

	// Called when going through a door
	PD_ResourceManager::scenario->eventManager.addEventListener("reset", [_game, this](sweet::Event * _event){
		
		PD_ResourceManager::scenario->getAudio("doorOpen")->sound->play();
		
		player->disable();
		transition = 0.f;
		transitionTarget = 1.f;

		screenSurfaceShader->bindShader();
		wipeColour = Colour::getRandomFromHsvMean(glm::ivec3(300, 67, 61), glm::ivec3(30, 25, 25));
		
		Timeout * t = new Timeout(1.f, [this, _game](sweet::Event * _event){
			std::stringstream ss;
			ss << "COMBINED_TEST_" << sweet::lastTimestamp;
			//_game->scenes[ss.str()] = new PD_Scene_Main(dynamic_cast<PD_Game *>(_game));
			//_game->switchScene(ss.str(), false); // TODO: fix memory issues so that this can be true

			navigate(glm::ivec2(1,0)); // TODO: replace this with actual navigation vector

			PD_ResourceManager::scenario->getAudio("doorClose")->sound->play();
		});
		t->start();
		childTransform->addChild(t, false);

		//PD_ResourceManager::scenario->eventManager.listeners.clear();

		
		GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "reverse");
		checkForGlError(false);
		if(test != -1){
			glUniform1i(test, 0);
			checkForGlError(false);
		}test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "xMult");
		checkForGlError(false);
		if(test != -1){
			glUniform1f(test, 1);
			checkForGlError(false);
		}
	});


	PD_ResourceManager::scenario->eventManager.addEventListener("changeDISSStat", [this](sweet::Event * _event){
		// Trigger
		// Modifies the DISS stats of the player. The stat can be chose from Defense, Insight, Strength, Sass. Delta is the amount to change the stat by, and can be positive or negative.
		// STRING stat
		// INT delta

		std::string stat = _event->getStringData("stat");
		int delta = _event->getIntData("delta", -1);

		if(stat == "" || delta == -1) {
			ST_LOG_ERROR_V("Missing field in trigger changeDISSStat")
		}

		std::transform(stat.begin(), stat.end(), stat.begin(), ::tolower);
		if(stat == "strength") {
			player->strenth += delta;	
		}else if(stat == "defense") {
			player->defense += delta;		
		}else if(stat == "insight") {
			player->insight += delta;		
		}else if(stat == "sass") {
			player->sass += delta;		
		}else {
			ST_LOG_ERROR_V("Invalid argument provided for argument 'stat' in trigger changeDISSStat");
		}
	});

	PD_ResourceManager::scenario->eventManager.addEventListener("changerOwnership", [](sweet::Event * _event){
		// Trigger
		// Takes an item from a character and gives it to another character. If the previous owner does not actually have the item, it should do nothing.
		// CHARACTER newOwner
		// ITEM item
		// CHARACTER prevOwner
	
		std::string ownerCharId = _event->getStringData("newOwner");
		std::string itemId = _event->getStringData("newOwner");
		std::string prevOwnerCharId = _event->getStringData("prevOwner");

		if(ownerCharId == "" || itemId == "" || prevOwnerCharId == "") {
			ST_LOG_ERROR_V("Missing field in trigger changerOwnership")
		}
	});

	PD_ResourceManager::scenario->eventManager.addEventListener("emote", [](sweet::Event * _event){
		std::string charId = _event->getStringData("character");
		std::string emote = _event->getStringData("emote");
		float duration = _event->getFloatData("duration");
		std::string scenario = _event->getStringData("scenario");
		PD_Listing::listingsById[scenario]->characters[charId]->pr->setEmote(emote, duration);
	});

	// build house
	pickScenarios();
	bundleScenarios();
	buildHouse();

	// move the player to the entrance of the first room
	navigate(glm::ivec2(0,0), false);
}


void PD_Scene_Main::pickScenarios(){
	// grab the current main plot scenario
	// these go in order


	// pick an omar scenario
	// first is always the tutorial/intro
	// middle are random from the a given set
	// last is always the final


	// pick side scenarios
	// random from static shuffle vector

	// TODO: all of this
	
	activeScenarios.push_back(new Scenario("assets/scenario-external-1.json"));
	activeScenarios.push_back(new Scenario("assets/scenario-external-2.json"));

	// set event managers on selected scenarios as children of the global scenario
	for(auto s : activeScenarios){
		PD_ResourceManager::scenario->eventManager.addChildManager(&s->eventManager);
	}
}

void PD_Scene_Main::bundleScenarios(){
	// find matching assets in scenarios and merge them
	// TODO: all of this
}

void PD_Scene_Main::buildHouse(){
	glm::ivec2 startPos(0);

	// build all of the rooms contained in the selected scenarios
	for(auto s : activeScenarios){
		
		// create a listing for this scenario
		PD_Listing * listing = new PD_Listing(s);

		// build the rooms in this scenario
		for(auto rd : s->assets.at("room")){
			Room * room = RoomBuilder(dynamic_cast<AssetRoom *>(rd.second), bulletWorld, toonShader, characterShader, emoteShader).getRoom();
			
			// setup the first parents, but don't actually add anything to the scene yet
			Transform * t = new Transform();
			t->addChild(room, false);
			for(unsigned int i = 0; i < room->components.size(); ++i){
				t = new Transform();
				t->addChild(room->components.at(i), false);
			}

			// run the physics simulation for a few seconds to let things settle
			Log::info("Letting the bodies hit the floor...");
			Step s;
			s.setDeltaTime(10);
			unsigned long int i = bulletWorld->maxSubSteps;
			bulletWorld->maxSubSteps = 10000;
			bulletWorld->update(&s);
			bulletWorld->maxSubSteps = i;
			Log::info("The bodies have finished hitting the floor.");

			// remove the physics bodies (we'll put them back in as needed)
			room->removePhysics();


			// save the room for later access
			PD_Listing::listings.at(room->definition->scenario)->addRoom(room);

			houseGrid[std::make_pair(startPos.x, startPos.y)] = room;
			startPos.x += 1;
		}
	}

	// update the map
	uiMap->buildMap(houseGrid);
}

void PD_Scene_Main::navigate(glm::ivec2 _movement, bool _relative){
	// transition
	screenSurfaceShader->bindShader();
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "reverse");
	checkForGlError(false);
	if(test != -1){
		glUniform1i(test, 1);
		checkForGlError(false);
	}test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "xMult");
	checkForGlError(false);
	if(test != -1){
		glUniform1f(test, 1);
		checkForGlError(false);
	}
	transition = 0.f;
	transitionTarget = 1.f;
	player->enable();
	
	// clear out the old room's stuff
	if(currentRoom != nullptr){
		for(unsigned int i = 0; i < currentRoom->components.size(); ++i){
			childTransform->removeChild(currentRoom->components.at(i)->firstParent());
		}
		childTransform->removeChild(currentRoom->firstParent());
		currentRoom->removePhysics();
	}


	// update position within house
	if(_relative){
		currentHousePosition += _movement;
	}else{
		currentHousePosition = _movement;
	}
	uiMap->updateMap(currentHousePosition);

	// get the room for the new position
	auto key = std::make_pair(currentHousePosition.x, currentHousePosition.y);
	if(houseGrid.count(key) != 1){
		Log::error("Room not found.");
	}
	currentRoom = houseGrid.at(key);

	// put the room into the scene/physics world
	currentRoom->addPhysics();
	childTransform->addChild(currentRoom->firstParent(), false);
	for(unsigned int i = 0; i < currentRoom->components.size(); ++i){
		childTransform->addChild(currentRoom->components.at(i)->firstParent(), false);
	}


	

	// make sure the door is up-to-date, and then place the player in front of it
	currentRoom->door->realign();
	glm::quat o = currentRoom->door->childTransform->getOrientationQuat();
	btVector3 p = currentRoom->door->body->getWorldTransform().getOrigin();
	glm::vec3 p2(0,0,3);
	p2 = o * p2;
	p2 += glm::vec3(p.x(), p.y(), p.z());

	player->translatePhysical(p2, false);

	Log::info("Navigated to room \"" + currentRoom->definition->name + "\"");

}

PD_Scene_Main::~PD_Scene_Main(){
	deleteChildTransform();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
	screenSurface->decrementAndDelete();
}

void PD_Scene_Main::update(Step * _step){
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
	screenSurfaceShader->bindShader();
	float transitionDelta = transitionTarget - transition;
	if(glm::abs(transitionDelta) >= FLT_EPSILON){
		transition += transitionDelta * 0.2f;
		if(transition >= 0.999f){
			transition = 1.f;
		}else if(transition <= 0.001f){
			transition = 0.f;
		}
		checkForGlError(false);
		GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "transition");
		checkForGlError(false);
		if(test != -1){
			glUniform1f(test, transition);
			checkForGlError(false);
		}
	}
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "wipeColour");
	checkForGlError(false);
	if(test != -1){
		glUniform3f(test, wipeColour.r/255.f, wipeColour.g/255.f, wipeColour.b/255.f);
		checkForGlError(false);
	}



	PD_ResourceManager::scenario->eventManager.update(_step);

	bulletWorld->update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_ESCAPE)){
		game->switchScene("menu", false);
		game->scenes["game"] = this;
	}

	if(keyboard->keyJustDown(GLFW_KEY_R)){
		PD_ResourceManager::scenario->eventManager.triggerEvent("reset");
	}

	if(keyboard->keyJustDown(GLFW_KEY_X)){
		uiYellingContest->eventManager.triggerEvent("interject");
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

									currentRoom->removeComponent(item);

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

									uiBubble->clear();
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
								std::string c = person->state->conversation;
								if(c == "NO_CONVO"){
									// incidental conversation
									Json::Value dialogue;
									dialogue["text"].append(incidentalPhraseGenerator.getLine());
									dialogue["speaker"] = person->definition->id;
									Json::Value root;
									root["dialogue"] = Json::Value();
									root["dialogue"].append(dialogue);

									Conversation * tempConvo = new Conversation(root, person->definition->scenario);
									uiDialogue->startEvent(tempConvo, true);
									player->disable();
								}else{
									// start a proper conversation
									uiDialogue->startEvent(person->definition->scenario->getConversation(c)->conversation, false);
									player->disable();
								}
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

						currentRoom->addComponent(item);
					}

					// replace the crosshair item texture with the actual crosshair texture
					crosshairIndicator->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
					crosshairIndicator->setWidth(16);
					crosshairIndicator->setHeight(16);
					crosshairIndicator->invalidateLayout();


				});
			}
		}
	}
	
	// inventory toggle
	if(keyboard->keyJustDown(GLFW_KEY_TAB)){
		if(uiInventory->isVisible()){
			uiInventory->close();
			uiLayer.removeMouseIndicator();
			player->enable();
		}else{
			uiInventory->open();
			uiLayer.addMouseIndicator();
			player->disable();
		}
	}
	
	// map toggle
	if(keyboard->keyJustDown(GLFW_KEY_M)){
		if(uiMap->isEnabled()){
			if(uiMap->isDetailed()){
				uiMap->setDetailed(false);
				uiMap->disable();
				player->enable();
				uiLayer.removeMouseIndicator();
			}else{
				uiMap->setDetailed(true);
				player->disable();
				uiLayer.addMouseIndicator();
			}
		}else{
			uiMap->enable();
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

void PD_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
		

	FrameBufferInterface::pushFbo(screenFBO);

	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
	
	FrameBufferInterface::popFbo();


	screenSurface->render(screenFBO->getTextureId());
}

void PD_Scene_Main::load(){
	Scene::load();	
	uiLayer.load();
	screenSurface->load();
	screenSurfaceShader->load();
	screenFBO->load();
}

void PD_Scene_Main::unload(){
	uiLayer.unload();
	screenSurface->unload();
	screenSurfaceShader->unload();
	screenFBO->unload();
	Scene::unload();	
}