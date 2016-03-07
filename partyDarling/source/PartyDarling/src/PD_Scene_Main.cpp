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
#include <shader/ShaderComponentHsv.h>
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
#include <PD_Door.h>
#include <PD_Prop.h>

#include <IntroRoom.h>
#include <LabRoom.h>
#include <PD_DissStats.h>

#define MAX_SIDE_SCENARIOS 5
#define LEVEL_UP_DURATION 3
#define XP_GAIN_PAUSE 1

Colour PD_Scene_Main::wipeColour(glm::ivec3(125/255.f,200/255.f,50/255.f));

PD_Scene_Main::PD_Scene_Main(PD_Game * _game) :
	Scene(_game),
	panSpeed(20.f),
	panLeft(false),
	panRight(false),
	trackSpeed(0.1f),
	trackLeft(false),
	trackRight(false),
	plotPosition(kBEGINNING),
	toonShader(new ComponentShaderBase(false)),
	screenSurfaceShader(new Shader("assets/RenderSurface", false, false)),
	screenSurface(new RenderSurface(screenSurfaceShader, false)),
	screenFBO(new StandardFrameBuffer(false)),
	uiLayer(new UILayer(0,0,0,0)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr),
	selectedItem(nullptr),
	characterShader(new ComponentShaderBase(false)),
	emoteShader(new ComponentShaderBase(false)),
	currentHoverTarget(nullptr),
	lightStart(0.3f),
	lightEnd(1.f),
	lightIntensity(1.f),
	transition(0.f),
	transitionTarget(1.f),
	currentRoom(nullptr),
	currentHousePosition(0),
	carriedProp(nullptr),
	carriedPropDistance(0)
{
	toonRamp = new RampTexture(lightStart, lightEnd, 4, false);
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
	characterShader->addComponent(new ShaderComponentHsv(characterShader, 0, 1, 1.5f));
	characterShader->compileShader();

	emoteShader->addComponent(new ShaderComponentMVP(emoteShader));
	emoteShader->addComponent(new ShaderComponentTexture(emoteShader));
	emoteShader->addComponent(new ShaderComponentDepthOffset(emoteShader));
	emoteShader->compileShader();

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);


	// remove initial camera
	/*childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();*/

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


	uiFade = new PD_UI_Fade(uiLayer->world);
	uiLayer->addChild(uiFade);
	uiFade->setRationalHeight(1.f, uiLayer);
	uiFade->setRationalWidth(1.f, uiLayer);
	
	uiBubble = new PD_UI_Bubble(uiLayer->world);
	uiMap = new PD_UI_Map(uiLayer->world, PD_ResourceManager::scenario->getFont("FONT")->font, uiBubble->textShader);
	uiLayer->addChild(uiMap);
	uiMap->setRationalHeight(1.f, uiLayer);
	uiMap->setRationalWidth(1.f, uiLayer);
	uiMap->enable();
	

	uiBubble->setRationalWidth(1.f, uiLayer);
	uiBubble->setRationalHeight(0.25f, uiLayer);
	uiLayer->addChild(uiBubble);

	uiInventory = new PD_UI_Inventory(uiLayer->world, uiBubble->textShader);
	uiLayer->addChild(uiInventory);
	uiInventory->setRationalHeight(1.f, uiLayer);
	uiInventory->setRationalWidth(1.f, uiLayer);
	uiInventory->eventManager->addEventListener("itemSelected", [this](sweet::Event * _event){
		uiInventory->disable();
		uiBubble->enable();
		uiLayer->removeMouseIndicator();
		player->enable();

		// replace the crosshair texture with the item texture
		Texture * itemTex = uiInventory->getSelected()->mesh->textures.at(0);
		crosshairIndicator->background->mesh->replaceTextures(itemTex);
		crosshairIndicator->setWidth(itemTex->width);
		crosshairIndicator->setHeight(itemTex->height);
		crosshairIndicator->invalidateLayout();
		// TODO: update the UI to indicate the selected item to the player
	});

	uiDialogue = new PD_UI_Dialogue(uiLayer->world, uiBubble);
	uiLayer->addChild(uiDialogue);
	uiDialogue->setRationalHeight(1.f, uiLayer);
	uiDialogue->setRationalWidth(1.f, uiLayer);
	uiDialogue->eventManager->addEventListener("end", [this](sweet::Event * _event){
		// Handle case where a diss battle is the last trigger in a dialogue
		if(!uiDissBattle->isEnabled()){
			player->enable();
			currentHoverTarget = nullptr;
			updateSelection();
		}
	});
	
	// add the player to the scene
	player = new Player(bulletWorld);
	childTransform->addChild(player);
	cameras.push_back(player->playerCamera);
	activeCamera = player->playerCamera;
	childTransform->addChild(player->playerCamera);
	player->playerCamera->firstParent()->translate(0, 5, 0);

	
	uiDissBattle = new PD_UI_DissBattle(uiLayer->world, player, PD_ResourceManager::scenario->getFont("FIGHT-FONT")->font, uiBubble->textShader, uiLayer->shader);
	uiLayer->addChild(uiDissBattle);
	uiDissBattle->setRationalHeight(1.f, uiLayer);
	uiDissBattle->setRationalWidth(1.f, uiLayer);

	uiDissBattle->eventManager->addEventListener("complete", [this](sweet::Event * _event){
		uiDissBattle->disable();
		dissBattleStartLayout->setVisible(true);
		dissBattleXPGainTimeout->restart();
		player->wonLastDissBattle = _event->getIntData("win");
	});
	uiDissBattle->eventManager->addEventListener("interject", [this](sweet::Event * _event){
		player->shakeIntensity = 0.3f;
		if(!_event->getIntData("success")){
			player->shakeTimeout->restart();
		}
	});
	uiDissBattle->eventManager->addEventListener("insult", [this](sweet::Event * _event){
		player->shakeIntensity = 0.3f;
		if(!_event->getIntData("success")){
			player->shakeTimeout->restart();
		}
	});
	uiDissBattle->eventManager->addEventListener("miss", [this](sweet::Event * _event){
		player->shakeIntensity = 0.1f;
		player->shakeTimeout->restart();
	});

	playerLight = new PointLight(glm::vec3(lightIntensity), 0.0f, 0.003f, -1);
	player->playerCamera->childTransform->addChild(playerLight);
	playerLight->firstParent()->translate(0.f, 1.f, 0.f);
	lights.push_back(playerLight);

	// Set the scenario condition implentations pointer
	PD_ResourceManager::scenario->conditionImplementations = PD_ResourceManager::conditionImplementations;

	// Setup conditions
	(*PD_ResourceManager::conditionImplementations)["checkState"] = [this](sweet::Event * _event){
		//Check if the character is in a certain state.
		// CHARACTER character
		// CHARACTER_STATE state
		std::string charId	 = _event->getStringData("character");
		std::string state	 = _event->getStringData("state");
		std::string scenario = _event->getStringData("scenario");

		if(charId == "" || state == "") {
			ST_LOG_ERROR_V("Field missing in condition checkState");
		}

		return PD_Listing::listingsById[scenario]->characters[charId]->state->id == state;
	};

	(*PD_ResourceManager::conditionImplementations)["checkInventory"] = [this](sweet::Event * _event){
		//Check if the character has a certain item
		// CHARACTER character
		// CHARACTER_STATE state
		std::string charId	 = _event->getStringData("owner");
		std::string item	 = _event->getStringData("item");
		std::string scenario = _event->getStringData("scenario");

		if(charId == "" || item == "") {
			ST_LOG_ERROR_V("Field missing in condition checkState");
			ST_LOG_ERROR_V("Field missing in condition checkState");
		}

		if(charId == PLAYER_ID) {
			for(auto i : uiInventory->items) {
				// Check scenario id since item ids are scoped to scenarios
				if(i->definition->id == item && i->definition->scenario->id == scenario) {
					return true;
				}
			}
		}else {
			for(auto c : PD_Listing::listingsById[scenario]->characters) {
				for(auto it : c.second->items) {
					if(it == item) {
						return true;
					}
				}
			}
		}
		return false;
	};

	(*PD_ResourceManager::conditionImplementations)["checkInt"] = [this](sweet::Event * _event){
		// check an integer variable that has been set
		// STRING name
		// INT desiredValue
		
		std::string name = _event->getStringData("name");
		int desiredValue = _event->getIntData("desiredValue", INT_MAX);
		Scenario * scenario = PD_Listing::listingsById[_event->getStringData("scenario")]->scenario;

		if(name == "" || desiredValue == INT_MAX) {
			ST_LOG_ERROR("Missing argument in condition checkInt");
		}

		int curVal = scenario->variables->getIntData(name, INT_MAX);
		
		return curVal == desiredValue;
	};

	(*PD_ResourceManager::conditionImplementations)["checkString"] = [this](sweet::Event * _event){
		// check an integer variable that has been set
		// STRING name
		// STRING desiredValue
		
		std::string name = _event->getStringData("name");
		std::string desiredValue = _event->getStringData("desiredValue", "NO_VALUE");
		Scenario * scenario = PD_Listing::listingsById[_event->getStringData("scenario")]->scenario;

		if(name == "" || desiredValue == "NO_VALUE") {
			ST_LOG_ERROR("Missing argument in condition checkString");
		}
		std::string curVal = scenario->variables->getStringData(name, "NO_VALUE_");
		return curVal == desiredValue;
	};

	(*PD_ResourceManager::conditionImplementations)["wonLastDissBattle"] = [this](sweet::Event * _event){
		//checks if the last diss battle was won. If no diss battles have occurred than false is returned
		return player->wonLastDissBattle;
	};

	// setup event listeners
	PD_ResourceManager::scenario->eventManager->addEventListener("changeState", [](sweet::Event * _event){
		std::string characterName = _event->getStringData("Character");
		std::string stateName = _event->getStringData("State");
		std::cout << characterName << "'s state changed to " << stateName << std::endl;

		PD_Listing * listing = PD_Listing::listingsById[_event->getStringData("scenario")];
		PD_Character * character = listing->characters[characterName];
		if(character == nullptr){
			Log::warn("Character not found in state change event");
		}else{
			character->state = &character->definition->states.at(stateName);
			character->pr->setAnimation(character->state->animation);
		}
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("setInt", [](sweet::Event * _event){
		// change/create a local int variable for a specific scenario
		// STRING name 
		// INT value
		std::string name = _event->getStringData("name");
		int value = _event->getIntData("value", INT_MAX);
		Scenario * scenario = PD_Listing::listingsById[_event->getStringData("scenario")]->scenario;

		if(name == "" || value == INT_MAX) {
			ST_LOG_ERROR("Missing argument in trigger setInt");
		}
		scenario->variables->setIntData(name, value);
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("setString", [](sweet::Event * _event){
		// change/create a local string variable for a specific scenario
		// STRING name 
		// STRING value
		std::string name = _event->getStringData("name");
		std::string value = _event->getStringData("value", "NO_VALUE");
		Scenario * scenario = PD_Listing::listingsById[_event->getStringData("scenario")]->scenario;

		if(name == "" || value == "NO_VALUE") {
			ST_LOG_ERROR("Missing argument in trigger setString");
		}
		scenario->variables->setStringData(name, value);
	});

	// Called when going through a door
	PD_ResourceManager::scenario->eventManager->addEventListener("navigate", [_game, this](sweet::Event * _event){
		glm::ivec2 navigation(_event->getIntData("x"), _event->getIntData("y"));

		player->disable();
		uiBubble->disable();

		PD_ResourceManager::scenario->getAudio("doorOpen")->sound->play();
		
		transition = 0.f;
		transitionTarget = 1.f;

		screenSurfaceShader->bindShader();
		wipeColour = Colour::getRandomFromHsvMean(glm::ivec3(300, 67, 61), glm::ivec3(30, 25, 25));
		
		Timeout * t = new Timeout(1.f, [this, _game, navigation](sweet::Event * _event){
			std::stringstream ss;
			ss << "COMBINED_TEST_" << sweet::lastTimestamp;
			//_game->scenes[ss.str()] = new PD_Scene_Main(dynamic_cast<PD_Game *>(_game));
			//_game->switchScene(ss.str(), false); // TODO: fix memory issues so that this can be true

			navigate(navigation); // TODO: replace this with actual navigation vector

			PD_ResourceManager::scenario->getAudio("doorClose")->sound->play();
			player->enable();
			uiBubble->enable();
		});
		t->start();
		childTransform->addChild(t, false);

		//PD_ResourceManager::scenario->eventManager->listeners.clear();

		
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

	
	// fades
	PD_ResourceManager::scenario->eventManager->addEventListener("fadeIn", [_game, this](sweet::Event * _event){
		uiFade->fadeIn(glm::uvec3(
			_event->getFloatData("r"),
			_event->getFloatData("g"),
			_event->getFloatData("b")
		),
		_event->getFloatData("length") / 1000.f);
	});
	PD_ResourceManager::scenario->eventManager->addEventListener("fadeOut", [_game, this](sweet::Event * _event){
		uiFade->fadeOut(glm::uvec3(
			_event->getFloatData("r"),
			_event->getFloatData("g"),
			_event->getFloatData("b")
		),
		_event->getFloatData("length") / 1000.f);
	});

	
	PD_ResourceManager::scenario->eventManager->addEventListener("locked", [_game, this](sweet::Event * _event){
		// Triggered when the player tries to open a locked door
		uiBubble->options.front()->label->setText("This door is locked.");
		PD_ResourceManager::scenario->getAudio("doorLocked")->sound->play();
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("changeDISSStat", [this](sweet::Event * _event){
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
			player->dissStats->incrementDefense(delta);	
		}else if(stat == "defense") {
			player->dissStats->incrementDefense(delta);		
		}else if(stat == "insight") {
			player->dissStats->incrementInsight(delta);		
		}else if(stat == "sass") {
			player->dissStats->incrementSass(delta);		
		}else {
			ST_LOG_ERROR_V("Invalid argument provided for argument 'stat' in trigger changeDISSStat");
		}
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("changeOwnership", [this](sweet::Event * _event){
		// Trigger
		// Takes an item from a character and gives it to another character. If the previous owner does not actually have the item, it should do nothing.
		// CHARACTER newOwner
		// ITEM item
		// CHARACTER prevOwner
		std::string ownerCharId = _event->getStringData("newOwner");
		std::string itemId = _event->getStringData("item");
		std::string prevOwnerCharId = _event->getStringData("prevOwner");
		std::string scenarioId = _event->getStringData("scenario");

		if(ownerCharId == "" || itemId == "" || prevOwnerCharId == "") {
			ST_LOG_ERROR_V("Missing field in trigger changerOwnership")
		}
		auto listing = PD_Listing::listingsById[scenarioId];
		PD_Item * item = listing->items[itemId];
		
		if(ownerCharId == prevOwnerCharId) {
			ST_LOG_ERROR_V("Invalid arguments is trigger changeOwnership - owner == prevOwner");
		}

		bool prevOwnerHasItem = false;
		for(auto it : listing->characters[prevOwnerCharId]->items) {
			if(it == item->definition->id) {
				prevOwnerHasItem = true;
			}
		}

		if(prevOwnerHasItem){
			if(ownerCharId == "0") {
				uiInventory->items.push_back(item);	
			}else if (prevOwnerCharId == "0") {
				for(unsigned long int i = 0; i < uiInventory->items.size(); ++i) {
					if(uiInventory->items[i]->definition->id == item->definition->id) {
						uiInventory->items.erase(uiInventory->items.begin() + i);
						break;
					}
				}
			}
			if(ownerCharId != "0") {
				listing->characters[ownerCharId]->items.push_back(item->definition->id);
			}
			if(prevOwnerCharId != "0") {
				for(unsigned long int i = 0; i < listing->characters[prevOwnerCharId]->items.size(); ++i) {
					if(listing->characters[prevOwnerCharId]->items[i] == item->definition->id) {
						listing->characters[prevOwnerCharId]->items.erase(listing->characters[prevOwnerCharId]->items.begin() + i);
						break;
					}
				}
			}
		}else {
			ST_LOG_WARN_V("Nothing occured in trigger changeOwnership - prevOwner did not have item");
		}
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("emote", [](sweet::Event * _event){
		std::string charId = _event->getStringData("character");
		std::string emote = _event->getStringData("emote");
		float duration = _event->getFloatData("duration", 9999);
		std::string scenario = _event->getStringData("scenario");
		
		if(charId == "" || emote == "" || abs(duration - 9999) <= FLT_EPSILON) {
			ST_LOG_ERROR_V("Missing field in trigger emote");
		}

		PD_Listing::listingsById[scenario]->characters[charId]->pr->setEmote(emote, duration);
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("unlockRoom", [](sweet::Event * _event){
		//Unlock the chosen room. If it is already unlocked, nothing will happen.
		// ROOM room = room to unlock
		std::string room = _event->getStringData("room");
		std::string scenario = _event->getStringData("scenario");

		if(room == "") {
			ST_LOG_ERROR_V("Missing field on trigger unlockRoom");
		}

		PD_Listing::listingsById[scenario]->rooms[room]->locked = false;
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("unlockLab", [this](sweet::Event * _event){
		// unlocks the lab room for the run
		labRoom->locked = false;
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("triggerDissBattle", [this](sweet::Event * _event){
		// Launch a diss battle with the selected character. 
		// playerInterjectInit is a boolean. If true, the player interjects first, if fasle, the player insults first.
		
		// CHARACTER oponent = character the player is fighting
		// INT(BOOLEAN) playerInterjectInit = true = player interects first, false = player insults first
		std::string opponent = _event->getStringData("opponent");
		bool interjectingFirst = static_cast<bool>(_event->getIntData("playerInterjectInit"));
		std::string scenario = _event->getStringData("scenario");

		if(opponent == "") {
			ST_LOG_ERROR_V("Missing field on trigger triggerDissBattle");
		}
		
		// TODO - Configure addtional data once the diss battle is set up for it
		
		PD_Character * enemy = PD_Listing::listingsById[scenario]->characters[opponent];
		uiDialogue->setVisible(false);
		uiBubble->disable();
		triggerDissBattle(enemy);
		uiDissBattle->eventManager->addEventListener("complete", [this](sweet::Event * _event){
			if(uiDialogue->hadNextDialogue){
				uiDialogue->setVisible(true);
				uiBubble->enable();
			}
			player->wonLastDissBattle = _event->getIntData("win");
		});
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("hideCharacter", [](sweet::Event * _event){
		// hide a character in a room until they need to appear
		// CHARACTER name
		// (BOOL)INT visibility

		std::string character = _event->getStringData("name");
		int visible = _event->getIntData("visibility", -1);
		std::string scenario = _event->getStringData("scenario");

		if(character == "" || visible == -1) {
			ST_LOG_ERROR_V("Missing field on trigger hideCharacter");
		}

		PD_Character * person = PD_Listing::listingsById[scenario]->characters[character];
		
		if(static_cast<bool>(visible)) {
			person->enable();
		}else {
			person->disable();
		}
	});

	// Load the save file
	loadSave();

	// build house
	pickScenarios();
	bundleScenarios();
	placeRooms(buildRooms());

	// move the player to the entrance of the first room
	// to do this, we set the currentHousePosition to be one unit outside the grid
	// and then navigate into it
	glm::ivec2 introHousePosition = currentHousePosition;
	if(currentHousePosition.x == 0){
		// west
		currentHousePosition.x -= 1;
		player->playerCamera->yaw += 90;
		navigate(glm::ivec2(1,0));
	}else if(currentHousePosition.x == houseSize-1){
		// east
		currentHousePosition.x += 1;
		player->playerCamera->yaw -= 90;
		navigate(glm::ivec2(-1,0));
	}else if(currentHousePosition.y == 0){
		// north
		currentHousePosition.y -= 1;
		//player->playerCamera->yaw += 0;
		navigate(glm::ivec2(0,1));
	}else{
		// south
		currentHousePosition.y += 1;
		player->playerCamera->yaw += 180;
		navigate(glm::ivec2(0,-1));
	}
	


	dissBattleStartLayout = new NodeUI(uiLayer->world);
	uiLayer->addChild(dissBattleStartLayout);
	dissBattleStartLayout->setRationalHeight(1.f, uiLayer);
	dissBattleStartLayout->setRationalWidth(1.f, uiLayer);
	dissBattleStartLayout->background->setVisible(false);
	dissBattleStartLayout->setVisible(false);

	HorizontalLinearLayout * dissBattleCards = new HorizontalLinearLayout(uiLayer->world);
	dissBattleStartLayout->addChild(dissBattleCards);
	dissBattleCards->setRationalHeight(1.f, dissBattleStartLayout);
	dissBattleCards->setRationalWidth(1.f, dissBattleStartLayout);
	dissBattleCards->setMarginLeft(0.3f);
	dissBattleCards->horizontalAlignment = kCENTER;
	dissBattleCards->verticalAlignment = kMIDDLE;
	
	playerCard = new PD_UI_DissCard(uiLayer->world, player);
	dissBattleCards->addChild(playerCard);
	playerCard->setRationalHeight(0.3f, dissBattleCards);
	playerCard->setSquareWidth(1.4f);

	vs = new NodeUI(uiLayer->world);
	dissBattleCards->addChild(vs);
	vs->setRationalWidth(0.3f, dissBattleCards);
	vs->setSquareHeight(1.f);
	vs->background->mesh->setScaleMode(GL_NEAREST);
	vs->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-VS")->texture);

	enemyCard = new PD_UI_DissCard(uiLayer->world);
	dissBattleCards->addChild(enemyCard);
	enemyCard->setRationalHeight(0.3f, dissBattleCards);
	enemyCard->setSquareWidth(1.4f);

	HorizontalLinearLayout * levelUpContainer = new HorizontalLinearLayout(uiLayer->world);
	dissBattleStartLayout->addChild(levelUpContainer);
	levelUpContainer->setRationalWidth(1.f, dissBattleStartLayout);
	levelUpContainer->setRationalHeight(1.f, dissBattleStartLayout);
	levelUpContainer->horizontalAlignment = kCENTER;
	levelUpContainer->verticalAlignment = kMIDDLE;

	levelUp = new NodeUI(uiLayer->world);
	levelUpContainer->addChild(levelUp);
	levelUp->setRationalHeight(1.f, levelUpContainer);
	levelUp->setSquareWidth(1.f);
	levelUp->background->mesh->setScaleMode(GL_NEAREST);
	levelUp->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-LEVEL-UP")->texture);
	levelUp->setVisible(false);

	dissBattleStartTimeout = new Timeout(3, [this](sweet::Event * _event){
		dissBattleStartLayout->setVisible(false);
		uiDissBattle->startNewFight(dissEnemy);
	});
	dissBattleStartTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		if(p < 0.25){
			playerCard->firstParent()->scale(Easing::easeOutBounce(p, 0, 1, 0.33), false);
			vs->firstParent()->scale(0, false);
			enemyCard->firstParent()->scale(0, false);
			if(!playerCard->isVisible()){
				playerCard->setVisible(true);
				vs->setVisible(false);
				enemyCard->setVisible(false);
				PD_ResourceManager::scenario->getAudio("DISS-BATTLE-INTRO")->sound->play();
			}
		}else if(p < 0.5){
			playerCard->firstParent()->scale(1, false);
			vs->firstParent()->scale(0, false);
			enemyCard->firstParent()->scale(Easing::easeOutBounce(p-0.25, 0, 1, 0.33), false);
			if(!enemyCard->isVisible()){
				playerCard->setVisible(true);
				vs->setVisible(false);
				enemyCard->setVisible(true);
				PD_ResourceManager::scenario->getAudio("DISS-BATTLE-INTRO")->sound->play();
			}
		}else if(p < 0.75){
			playerCard->firstParent()->scale(1, false);
			vs->firstParent()->scale(Easing::easeOutBounce(p-0.5, 0, 1, 0.33), false);
			enemyCard->firstParent()->scale(1, false);
			if(!vs->isVisible()){
				playerCard->setVisible(true);
				vs->setVisible(true);
				enemyCard->setVisible(true);
				PD_ResourceManager::scenario->getAudio("DISS-BATTLE-INTRO")->sound->play();
			}
		}else{
			playerCard->firstParent()->scale(1, false);
			vs->firstParent()->scale(1, false);
			enemyCard->firstParent()->scale(1, false);
			playerCard->setVisible(true);
			vs->setVisible(true);
			enemyCard->setVisible(true);
		}
	});
	childTransform->addChild(dissBattleStartTimeout, false);

	dissBattleXPGainTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		if(player->experience >= 100 * player->level){
			// LEVEL UP
			player->dissStats->incrementDefense();
			player->dissStats->incrementInsight();
			player->dissStats->incrementSass();
			player->dissStats->incrementStrength();
			playerCard->updateStats();
			++player->level;

			levelUp->setVisible(true);
			levelUp->firstParent()->scale(0, false);
			
			dissBattleLevelUpTimeout->restart();
		}else{
			// NORMAL
			dissBattleXPPause->restart();
		}
		
	});

	dissBattleXPGainTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		
		player->experience = uiDissBattle->prevXP + p * uiDissBattle->wonXP;
	});
	childTransform->addChild(dissBattleXPGainTimeout, false);

	dissBattleXPPause = new Timeout(1.f, [this](sweet::Event * _event){
		// end I hate this
		dissBattleStartLayout->setVisible(false);
		if(!uiDialogue->hadNextDialogue){
			player->enable();
			currentHoverTarget = nullptr;
			updateSelection();
		}
	});
	childTransform->addChild(dissBattleXPPause, false);

	dissBattleLevelUpTimeout = new Timeout(LEVEL_UP_DURATION, [this](sweet::Event * _event){
		// end and this
		levelUp->setVisible(false);
		player->experience = 0.f; // just in case
		dissBattleStartLayout->setVisible(false);
		if(!uiDialogue->hadNextDialogue){
			player->enable();
			currentHoverTarget = nullptr;
			updateSelection();
		}
	});
	dissBattleLevelUpTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		if(p <= 0.2f){
			player->experience = 100 * (1-p/0.2f);
		}

		levelUp->firstParent()->scale(Easing::easeOutBounce(p * LEVEL_UP_DURATION, 0, 1, LEVEL_UP_DURATION * 0.5f), false);
	});
	childTransform->addChild(dissBattleLevelUpTimeout, false);
}


void PD_Scene_Main::pickScenarios(){
	activeScenarios.clear();

	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = sweet::FileUtils::readFile("assets/scenarios.json");
	bool parsingSuccessful = reader.parse(jsonLoaded, root);
	if(!parsingSuccessful) {
		ST_LOG_ERROR("Could not load scenarios listing");
	}

	Json::Value scenarioFile;
	sweet::ShuffleVector<Json::Value> allSideDefs;
	sweet::ShuffleVector<Json::Value> allOmarDefs;
	std::vector<Json::Value> allPlotDefs;
	std::vector<Json::Value> allLabDefs;

	for(auto scenarioDef : root) {
		ScenarioType type = static_cast<ScenarioType>(scenarioDef.get("type", 0).asInt());
		switch(type) {
			case kSIDE: 
				allSideDefs.push(scenarioDef);
				break;
			case kOMAR: 
				allOmarDefs.push(scenarioDef);
				break;
			case kPLOT: 
				allPlotDefs.push_back(scenarioDef);
				break;
			case kLAB: 
				allLabDefs.push_back(scenarioDef);
				break;
			default: 
				ST_LOG_ERROR("Invalid Scenario Type");
				break;
		}
	}
	
	// Sort the plot and lab defs 
	// Each should have five items - each with a different order going from 1 to 5
	// The lab and plot defs mus line up with one another
	std::sort(allPlotDefs.begin(), allPlotDefs.end(), [](Json::Value a, Json::Value b){
		return a["order"] < b["order"];
	});

	std::sort(allLabDefs.begin(), allLabDefs.end(), [](Json::Value a, Json::Value b){
		return a["order"] < b["order"];
	});

	assert(allPlotDefs.size() > 0);
	/* Just for now
	assert(allPlotDefs.size() == 5);
	assert(allLabDefs.size() == 5);
	assert(allOmarDefs.size() >= 3);
	*/
	for(unsigned long int i = 0; i < allPlotDefs.size(); ++i) {
		Json::Value scenariosList;

		// This should add each of the plot scenarios in the right order
		// The plot types go from 1 to 5
		// Since we sorted by order this should just work
		scenariosList.append(allPlotDefs[i]["src"].asString());

		assert(scenariosList.size() > 0);
		
		int numSidePlots = sweet::NumberUtils::randomInt(3, 5);
		for(unsigned long int i = 0; i < numSidePlots; ++i) {
			scenariosList.append(allSideDefs.pop()["src"].asString());			
		}

		if(plotPosition != kBEGINNING && plotPosition != kEND){
			if(allOmarDefs.size() > 0){
				scenariosList.append(allOmarDefs.pop()["src"].asString());
			}
		}

		// Add the lab def last
		// We shouldn't need this check but we'll leave it here until all the scenarios are in
		if(allLabDefs.size() > i){
			scenariosList.append(allLabDefs[i]["src"].asString());
		}

		scenarioFile.append(scenariosList);
	}

	// SAVE senarioFile

	// ****************
	// grab the current main plot scenario
	
	// pick an omar scenario
	
	// first is always the tutorial/intro

	// middle are random from the a given set

	// last is always the final


	// pick side scenarios
	// random from static shuffle vector

	Json::Value currentScenario;
	int i = 1;
	for(auto scenariosList : scenarioFile) {
		if(i == plotPosition){
			currentScenario = scenariosList;
			break;
		}
		++i;
	}
	
	for(auto scenarioDef : currentScenario) {
		activeScenarios.push_back(new PD_Scenario("assets/" + scenarioDef.asString()));
	}

	// TODO: all of this
	//activeScenarios.push_back(new PD_Scenario("assets/scenario-external-1.json"));
	//activeScenarios.push_back(new PD_Scenario("assets/scenario-external-2.json"));
	//activeScenarios.push_back(new PD_Scenario("assets/scenario-intro.json"));
	//activeScenarios.push_back(new Scenario("assets/scenario-external-3.json"));

	// set event managers on selected scenarios as children of the global scenario
	for(auto s : activeScenarios){
		PD_ResourceManager::scenario->eventManager->addChildManager(s->eventManager);
		s->conditionImplementations = PD_ResourceManager::conditionImplementations;
	}
}

void PD_Scene_Main::bundleScenarios(){
	// find matching assets in scenarios and merge them
	// TODO: all of this
}



void PD_Scene_Main::placeRooms(std::vector<Room *> _rooms){
	IntroRoom * introRoom = dynamic_cast<IntroRoom *>(_rooms.back());
	_rooms.pop_back();
	labRoom = dynamic_cast<LabRoom *>(_rooms.back());
	_rooms.pop_back();

	int numRooms = _rooms.size();
	// separate rooms into a list of unlocked and locked
	sweet::ShuffleVector<Room *> unlockedRooms, lockedRooms;
	while(_rooms.size() > 0){
		Room * room = _rooms.back();
		_rooms.pop_back();
		if(room->locked){
			lockedRooms.push(room);
		}else{
			unlockedRooms.push(room);
		}
	}

	// generate a random rectangle which has enough cells for each room
	houseSize = glm::max(2, sweet::NumberUtils::randomInt(sqrt(numRooms), numRooms)) + 2;
	//houseSize.y = glm::max(2, numRooms/houseSize.x);
	while(pow(houseSize,2) < numRooms){
		++houseSize;
	}

	// quadruple the size of the rectangle to allow for empty cells
	houseSize *= 2;

	// generate the starting position as a random spot along an edge
	if(sweet::NumberUtils::randomBool()){
		currentHousePosition.x = sweet::NumberUtils::randomInt(0,houseSize-1);
		currentHousePosition.y = sweet::NumberUtils::randomInt(0,1) * (houseSize-1);
	}else{
		currentHousePosition.x = sweet::NumberUtils::randomInt(0,1) * (houseSize-1);
		currentHousePosition.y = sweet::NumberUtils::randomInt(0,houseSize-1);
	}
	// PLACEMENT
	std::map<std::pair<int,int>, bool> allCells;

	// initialize all cells as empty
	for(int x = 0; x < houseSize; ++x){
		for(int y = 0; y < houseSize; ++y){
			allCells[std::make_pair(x,y)] = true;
		}
	}
	// flag a bunch of cells as used immediately (the grid is big enough for at least numRooms*4) 
	// since we aren't attaching rooms to them, these cells will just be ignored during placement and end up as holes
	// make sure to leave the edge cells open so that we don't block off the entrance and provide a path to anywhere in the grid
	// also make sure to save a list of these cells so that we can force through them if needed
	sweet::ShuffleVector<glm::ivec2> possibleBlockedCellPositions;
	sweet::ShuffleVector<glm::ivec2> blockedPositions;
	for(int x = 1; x < houseSize-1; x += 2){
		for(int y = 1; y < houseSize-1; y += 2){
			possibleBlockedCellPositions.push(glm::ivec2(x,y));
		}
	}

	for(unsigned long int i = 0; i < possibleBlockedCellPositions.size()/3*2; ++i){
		glm::ivec2 pos = possibleBlockedCellPositions.pop();
		blockedPositions.push(pos);
		allCells[std::make_pair(pos.x, pos.y)] = false;
	}
	
	sweet::ShuffleVector<glm::ivec2> openCells;

	// place the starting room in the starting position
	houseGrid[std::make_pair(currentHousePosition.x, currentHousePosition.y)] = introRoom;
	if(currentHousePosition.x == 0){
		introRoom->setEdge(PD_Door::kWEST);
	}else if(currentHousePosition.x == houseSize-1){
		introRoom->setEdge(PD_Door::kEAST);
	}else if(currentHousePosition.y == 0){
		introRoom->setEdge(PD_Door::kNORTH);
	}else{
		introRoom->setEdge(PD_Door::kSOUTH);
	}


	// place the cells adjacent to the starting position into the list of open cells
	openCells.push(getAdjacentCells(currentHousePosition, allCells, houseSize));
	allCells[std::make_pair(currentHousePosition.x, currentHousePosition.y)] = false;

	// place the unlocked rooms by picking a random open cell and a random room,
	// assigning the room to the cell, and storing any open adjacent cells in the list
	while(unlockedRooms.size() > 0){
		glm::ivec2 cell;
		if(openCells.size() > 0){
			cell = openCells.pop(true); // make sure to remove the cell from the shuffle vector
		}else{
			cell = blockedPositions.pop(true); // if we ran out of possible places to go, use one of the pre-blocked cells instead
		}
		allCells[std::make_pair(cell.x, cell.y)] = false;
		Room * room = unlockedRooms.pop(true);
		houseGrid[std::make_pair(cell.x, cell.y)] = room;
		
		openCells.push(getAdjacentCells(cell, allCells, houseSize));
		openCells.clearAvailable();
	}

	// repeat for locked rooms
	while(lockedRooms.size() > 0){
		glm::ivec2 cell;
		if(openCells.size() > 0){
			cell = openCells.pop(true); // make sure to remove the cell from the shuffle vector
		}else if(blockedPositions.size() > 0){
			cell = blockedPositions.pop(true); // if we ran out of possible places to go, use one of the pre-blocked cells instead
		}else{
			Log::error("Room can't be placed!");
		}
		allCells[std::make_pair(cell.x, cell.y)] = false;
		Room * room = lockedRooms.pop(true);
		houseGrid[std::make_pair(cell.x, cell.y)] = room;
		
		openCells.push(getAdjacentCells(cell, allCells, houseSize));
		openCells.clearAvailable();
	}

	// repeat for the lab, ensuring it is the last thing placed
	glm::ivec2 cell;
	if(openCells.size() > 0){
		cell = openCells.pop(true); // make sure to remove the cell from the shuffle vector
	}else if(blockedPositions.size() > 0){
		cell = blockedPositions.pop(true); // if we ran out of possible places to go, use one of the pre-blocked cells instead
	}else{
		Log::error("Room can't be placed!");
	}
	allCells[std::make_pair(cell.x, cell.y)] = false;
	houseGrid[std::make_pair(cell.x, cell.y)] = labRoom;


	
	// loop through all of the rooms and remove doors which don't lead anywhere
	// and save the door positions while we're at it
	float doorSpacing = 1.5f;
	for(auto c : houseGrid){
		int x = c.first.first;
		int y = c.first.second;

		PD_Door::Door_t side;
		btVector3 p;
		Room * room = c.second;
		
		side = PD_Door::kEAST;

		p = room->doors.at(side)->body->getWorldTransform().getOrigin();
		room->doorPositions[side] = glm::vec3(p.x(), p.y(), p.z()) + glm::vec3(-doorSpacing,0,0);
		auto it = houseGrid.find(std::make_pair(x-1,y));
		if(it == houseGrid.end()){
			room->removeComponent(room->doors.at(side));
			delete room->doors.at(side);
			room->doors.erase(side);
		}else{
			room->doors.at(side)->room = it->second;
		}
		
		side = PD_Door::kWEST;
		p = room->doors.at(side)->body->getWorldTransform().getOrigin();
		room->doorPositions[side] = glm::vec3(p.x(), p.y(), p.z()) + glm::vec3(doorSpacing,0,0);
		it = houseGrid.find(std::make_pair(x+1,y));
		if(it == houseGrid.end()){
			room->removeComponent(c.second->doors.at(side));
			delete room->doors.at(side);
			room->doors.erase(side);
		}else{
			room->doors.at(side)->room = it->second;
		}
		
		side = PD_Door::kNORTH;
		p = room->doors.at(side)->body->getWorldTransform().getOrigin();
		room->doorPositions[side] = glm::vec3(p.x(), p.y(), p.z()) + glm::vec3(0,0,doorSpacing);
		it = houseGrid.find(std::make_pair(x,y+1));
		if(it == houseGrid.end()){
			room->removeComponent(room->doors.at(side));
			delete room->doors.at(side);
			room->doors.erase(side);
		}else{
			room->doors.at(side)->room = it->second;
		}
		
		side = PD_Door::kSOUTH;
		p = room->doors.at(side)->body->getWorldTransform().getOrigin();
		room->doorPositions[side] = glm::vec3(p.x(), p.y(), p.z()) + glm::vec3(0,0,-doorSpacing);
		it = houseGrid.find(std::make_pair(x,y-1));
		if(it == houseGrid.end()){
			room->removeComponent(room->doors.at(side));
			delete room->doors.at(side);
			room->doors.erase(side);
		}else{
			room->doors.at(side)->room = it->second;
		}
	}

	// update the map
	uiMap->buildMap(houseGrid);
}

std::vector<glm::ivec2> PD_Scene_Main::getAdjacentCells(glm::ivec2 _pos, std::map<std::pair<int,int>, bool> &_cells, int _maxSize){
	std::vector<glm::ivec2> res;
		
	glm::ivec2 temp = _pos + glm::ivec2(-1,0);
	if(temp.x >= 0 && _cells.at(std::make_pair(temp.x, temp.y))){
		res.push_back(temp);
	}

	temp = _pos + glm::ivec2(1,0);
	if(temp.x < _maxSize && _cells.at(std::make_pair(temp.x, temp.y))){
		res.push_back(temp);
	}

	temp = _pos + glm::ivec2(0,-1);
	if(temp.y >= 0 && _cells.at(std::make_pair(temp.x, temp.y))){
		res.push_back(temp);
	}

	temp = _pos + glm::ivec2(0,1);
	if(temp.y < _maxSize && _cells.at(std::make_pair(temp.x, temp.y))){
		res.push_back(temp);
	}

	return res;
}

std::vector<Room *> PD_Scene_Main::buildRooms(){
	PD_Game * g = dynamic_cast<PD_Game *>(game);
	std::vector<Room *> res;

	// count the total number of rooms so that we can show progress
	unsigned long int numRooms = 0;
	for(auto scenario : activeScenarios){
		numRooms += scenario->assets["room"].size();
	}


	// build all of the rooms contained in the selected scenarios
	unsigned long int progress = 0;
	for(auto scenario : activeScenarios){
		
		// create a listing for this scenario
		PD_Listing * listing = new PD_Listing(scenario);

		// build the rooms in this scenario
		for(auto rd : scenario->assets.at("room")){
			g->showLoading((float)++progress/numRooms);
			Room * room = RoomBuilder(dynamic_cast<AssetRoom *>(rd.second), bulletWorld, toonShader, characterShader, emoteShader).getRoom();
			
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

			// put the room into the shuffle vector
			res.push_back(room);
		}
	}


	// construct static rooms (into room, lab room)
	res.push_back(new LabRoom(bulletWorld, toonShader, characterShader, emoteShader, dynamic_cast<AssetRoom *>(PD_ResourceManager::labScenario->getAsset("room","1"))));
	res.push_back(new IntroRoom(bulletWorld, toonShader, characterShader, emoteShader, dynamic_cast<AssetRoom *>(PD_ResourceManager::introScenario->getAsset("room","1"))));


	return res;
}

void PD_Scene_Main::triggerDissBattle(PD_Character * _enemy) {
	dissEnemy = _enemy;
	uiBubble->clear();
	player->disable();
	dissBattleStartLayout->setVisible(true);
	dissBattleStartTimeout->restart();
	enemyCard->setEnemy(_enemy);

	playerCard->setVisible(false);
	vs->setVisible(false);
	enemyCard->setVisible(false);
	
	playerCard->childTransform->translate(glm::vec3(-playerCard->getWidth(true,false)*0.5f, -playerCard->getHeight(true,false)*0.5f, 0), false);
	vs->childTransform->translate(glm::vec3(-vs->getWidth(true,false)*0.5f, -vs->getHeight(true,false)*0.5f, 0), false);
	enemyCard->childTransform->translate(glm::vec3(-enemyCard->getWidth(true,false)*0.5f, -enemyCard->getHeight(true,false)*0.5f, 0), false);
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
		removeRoom(currentRoom);
	}


	// update position within house
	if(_relative){
		currentHousePosition += _movement;
	}else{
		currentHousePosition = _movement;
	}

	// get the room for the new position
	auto key = std::make_pair(currentHousePosition.x, currentHousePosition.y);
	if(houseGrid.count(key) != 1){
		Log::error("Room not found.");
	}
	currentRoom = houseGrid.at(key);
	ST_LOG_INFO(currentRoom->definition->name);
	// put the room into the scene/physics world
	addRoom(currentRoom);


	PD_Door::Door_t doorToEnter;
	if(_relative){
		if(glm::abs(_movement.x) > glm::abs(_movement.y)){
			// horizontal
			if(_movement.x < 0){
				doorToEnter = PD_Door::kWEST;
			}else{
				doorToEnter = PD_Door::kEAST;
			}
		}else{
			// vertical
			if(_movement.y < 0){
				doorToEnter = PD_Door::kNORTH;
			}else{	
				doorToEnter = PD_Door::kSOUTH;
			}
		}
	}else{
		doorToEnter = currentRoom->doors.begin()->first;
	}

	// make sure the door is up-to-date, and then place the player in front of it
	player->translatePhysical(currentRoom->doorPositions.at(doorToEnter), false);

	// Trigger room entry events (important to do this before map is updated, otherwise we won't get the once-only triggers)
	if(currentRoom->visibility != Room::kENTERED){
		for(auto trigger : currentRoom->definition->triggersOnce) {
			sweet::Event * e = new sweet::Event(trigger);
			PD_ResourceManager::scenario->eventManager->triggerEvent(e);
		}
	}
	currentRoom->definition->triggersOnce.clear();

	for(auto trigger : currentRoom->definition->triggersMulti) {
		sweet::Event * e = new sweet::Event(trigger);
		PD_ResourceManager::scenario->eventManager->triggerEvent(e);
	}

	lights.clear();
	lights.push_back(playerLight);
	
	for(unsigned long int i = 0; i < currentRoom->lights.size(); ++i) {
		currentRoom->lights[i]->lastPos = glm::vec3(99999);
		lights.push_back(currentRoom->lights[i]);
	}
	
	// update map with new position
	uiMap->updateMap(currentHousePosition);
	Log::info("Navigated to room \"" + currentRoom->definition->name + "\"");
}

PD_Scene_Main::~PD_Scene_Main(){
	// clear out the current room's stuff
	if(currentRoom != nullptr){
		removeRoom(currentRoom);
	}
	// put every room into scene/physics world so that they all get deleted along with the scene
	for(auto r : houseGrid){
		addRoom(r.second);
	}

	deleteChildTransform();
	delete uiLayer;
	
	delete screenSurface;
	delete screenSurfaceShader;
	delete screenFBO;

	while(activeScenarios.size() > 0){
		delete activeScenarios.back();
		activeScenarios.pop_back();
	}

	delete bulletWorld;
	delete toonShader;
	delete characterShader;
	delete emoteShader;

	delete toonRamp;
}



void PD_Scene_Main::removeRoom(Room * _room){
	for(unsigned int i = 0; i < _room->components.size(); ++i){
		childTransform->removeChild(_room->components.at(i)->firstParent());
	}
	childTransform->removeChild(_room->firstParent());
	_room->removePhysics();
}
void PD_Scene_Main::addRoom(Room * _room){
	_room->addPhysics();
	if(_room->parents.size() > 0){
		childTransform->addChild(_room->firstParent(), false);
		for(unsigned int i = 0; i < _room->components.size(); ++i){
			childTransform->addChild(_room->components.at(i)->firstParent(), false);
		}
	}else{
		childTransform->addChild(_room);
		for(unsigned int i = 0; i < _room->components.size(); ++i){
			childTransform->addChild(_room->components.at(i));
		}
	}
}



void PD_Scene_Main::update(Step * _step){
	// panning
	if(panLeft){
		player->playerCamera->yaw += panSpeed * _step->getDeltaTime();
	}
	if(panRight){
		player->playerCamera->yaw -= panSpeed * _step->getDeltaTime();
	}

	// tracking
	if(trackLeft){
		player->translatePhysical((player->playerCamera->rightVectorRotated) * -trackSpeed);
	}
	if(trackRight){
		player->translatePhysical((player->playerCamera->rightVectorRotated) * trackSpeed);
	}

	// billboarding
	glm::vec3 camPos = player->playerCamera->childTransform->getWorldPos();
	for(auto & c : currentRoom->characters){
		c->billboard(camPos);
	}
	for(auto & c : currentRoom->items){
		c->billboard(camPos);
	}

	if(keyboard->keyJustDown(GLFW_KEY_1)){
		Texture * tex = getToken();
		tex->load();
		tex->saveImageData("tokenTest.tga");
		uiDissBattle->addLife(tex);
	}
	
	if(keyboard->keyJustDown(GLFW_KEY_3)){
		uiFade->fadeIn(glm::uvec3(255,255,255));
	}
	if(keyboard->keyJustDown(GLFW_KEY_4)){
		uiFade->fadeIn();
	}
	
	if(keyboard->keyJustDown(GLFW_KEY_5)){
		uiFade->fadeOut(glm::uvec3(255,255,255));
	}
	if(keyboard->keyJustDown(GLFW_KEY_6)){
		uiFade->fadeOut();
	}


	// look up at current speaker's face during conversations
	PD_Character * facing = nullptr;
	if(uiDialogue->isVisible()){
		if(uiDialogue->currentSpeaker != nullptr){
			facing = uiDialogue->currentSpeaker;
		}
	}else if(uiDissBattle->isVisible() || dissBattleStartLayout->isVisible()){
		facing = dissEnemy;
	}

	if(facing != nullptr){
		glm::vec3 headPos = facing->pr->head->childTransform->getWorldPos();
		glm::vec3 d = glm::normalize(headPos - camPos);
		
		float pitch = glm::degrees(glm::atan(d.y, sqrt((d.x * d.x) + (d.z * d.z))));
		float yaw = glm::degrees(glm::atan(d.x, d.z)) - 90;
		float pDif = pitch - player->playerCamera->pitch;
		float yDif = yaw - player->playerCamera->yaw;
			
		while(pDif > 180){
			pDif -= 360;
		}while(pDif < -180){
			pDif += 360;
		}
		while(yDif > 180){
			yDif -= 360;
		}while(yDif < -180){
			yDif += 360;
		}
		if(glm::abs(pDif) > FLT_EPSILON){
			player->playerCamera->pitch += pDif*0.05f;
		}
		if(glm::abs(yDif) > FLT_EPSILON){
			player->playerCamera->yaw += yDif*0.05f;
		}
	}


	// party lights!
	float a = playerLight->getAttenuation();
	float newa = fmod(_step->time, 142.f/300.f)*0.01f+0.01f;
	playerLight->setAttenuation(newa);
	if(newa < a){
		lightStart = glm::vec3(sweet::NumberUtils::randomFloat(0.3f, 0.5f),sweet::NumberUtils::randomFloat(0.3f, 0.5f),sweet::NumberUtils::randomFloat(0.3f, 0.5f));
		lightEnd = glm::vec3(sweet::NumberUtils::randomFloat(0.9f, 1.5f),sweet::NumberUtils::randomFloat(0.9f, 1.5f),sweet::NumberUtils::randomFloat(0.9f, 1.5f));
		lightIntensity = sweet::NumberUtils::randomFloat(1.f, 1.25f) * currentRoom->lights.size() > 0 ? 0.05f : 0.5f;
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



	PD_ResourceManager::scenario->eventManager->update(_step);

	bulletWorld->update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_ESCAPE)){
		game->switchScene("menu", false);
		game->scenes["game"] = this;
	}

	if(keyboard->keyJustDown(GLFW_KEY_R)){
		PD_ResourceManager::scenario->eventManager->triggerEvent("reset");
	}

	// navigation testing
	if(keyboard->shift){
		if(keyboard->keyJustDown(GLFW_KEY_UP)){
			navigate(glm::ivec2(0,-1));
		}if(keyboard->keyJustDown(GLFW_KEY_DOWN)){
			navigate(glm::ivec2(0,1));
		}if(keyboard->keyJustDown(GLFW_KEY_LEFT)){
			navigate(glm::ivec2(-1,0));
		}if(keyboard->keyJustDown(GLFW_KEY_RIGHT)){
			navigate(glm::ivec2(1,0));
		}
	}

	// panning
	if(keyboard->keyDown(GLFW_KEY_P)){
		if(keyboard->keyJustDown(GLFW_KEY_LEFT)){
			if(!panLeft){
				panLeft = true;
				if(panRight){
					// switch direction
					panRight = false;
				}else{
					if(!trackLeft && !trackRight){
						// disable player
						uiLayer->setVisible(false);
						player->disable();
					}
				}
			}else{
				panLeft = false;
				if(!trackLeft && !trackRight){
					// enable player
					uiLayer->setVisible(true);
					player->enable();
				}
			}
		}if(keyboard->keyJustDown(GLFW_KEY_RIGHT)){
			if(!panRight){
				panRight = true;
				if(panLeft){
					// switch direction
					panLeft = false;
				}else{
					if(!trackLeft && !trackRight){
						// disable player
						uiLayer->setVisible(false);
						player->disable();
					}
				}
			}else{
				panRight = false;
				if(!trackLeft && !trackRight){
					// enable player
					uiLayer->setVisible(true);
					player->enable();
				}
			}
		}
	}
	
	// tracking
	if(keyboard->keyDown(GLFW_KEY_T)){
		if(keyboard->keyJustDown(GLFW_KEY_LEFT)){
			if(!trackLeft){
				trackLeft = true;
				if(trackRight){
					// switch direction
					trackRight = false;
				}else{
					if(!panLeft && !panRight){
						// disable player
						uiLayer->setVisible(false);
						player->disable();
					}
				}
			}else{
				trackLeft = false;
				if(!panLeft && !panRight){
					// enable player
					uiLayer->setVisible(true);
					player->enable();
				}
			}
		}if(keyboard->keyJustDown(GLFW_KEY_RIGHT)){
			if(!trackRight){
				trackRight = true;
				if(trackLeft){
					// switch direction
					trackLeft = false;
				}else{
					if(!panLeft && !panRight){
						// disable player
						uiLayer->setVisible(false);
						player->disable();
					}
				}
			}else{
				trackRight = false;
				// enable player
				if(!panLeft && !panRight){
					uiLayer->setVisible(true);
					player->enable();
				}
			}
		}
	}

	if(keyboard->keyJustDown(GLFW_KEY_X)){
		uiDissBattle->eventManager->triggerEvent("interject");
	}

	if(keyboard->keyJustDown(GLFW_KEY_L)){
		screenSurfaceShader->unload();
		screenSurfaceShader->loadFromFile(screenSurfaceShader->vertSource, screenSurfaceShader->fragSource);
		screenSurfaceShader->load();
	}

	// mouse interaction with world objects
	updateSelection();


	// prop carrying release and carry
	if(mouse->leftJustReleased()){
		if(carriedProp != nullptr){
			carriedProp->body->setDamping(0,0);
			carriedProp->body->setGravity(bulletWorld->world->getGravity());
		}
		carriedProp = nullptr;
		carriedPropDistance = 0;
	}else if(mouse->leftDown()){
		if(carriedProp != nullptr){
			glm::vec3 targetPos = camPos + player->playerCamera->forwardVectorRotated * carriedPropDistance;
			glm::vec3 d = (targetPos - carriedProp->meshTransform->getWorldPos()) * 0.5f;
			float dl = glm::length(d);
			if(dl > 0.3f){
				d = glm::normalize(d) * 0.3f;
			}
			carriedProp->applyLinearImpulseToCenter(d);
		}
	}
	
	// inventory toggle
	if(keyboard->keyJustDown(GLFW_KEY_TAB)){
		if(uiInventory->isVisible()){
			uiBubble->enable();
			uiInventory->disable();
			uiLayer->removeMouseIndicator();
			player->enable();
		}else{
			uiBubble->disable();
			uiInventory->enable();
			uiLayer->addMouseIndicator();
			player->disable();
		}
	}
	
	// map toggle
	if(keyboard->keyJustDown(GLFW_KEY_M)){
		if(uiMap->isEnabled()){
			uiMap->disable();
		}else{
			uiMap->enable();
		}
	}


	// map compass update
	uiMap->updateCompass(-glm::degrees(atan2(activeCamera->forwardVectorRotated.z, activeCamera->forwardVectorRotated.x)) + 90.f);

#ifdef _DEBUG

	if(keyboard->keyJustUp(GLFW_KEY_W) && keyboard->control) {
		save();
	}

#endif

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

void PD_Scene_Main::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
		

	FrameBufferInterface::pushFbo(screenFBO);

	_renderOptions->setClearColour(0,0,0,0);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	
	FrameBufferInterface::popFbo();


	screenSurface->render(screenFBO->getTextureId());
	uiLayer->render(_matrixStack, _renderOptions);
}

void PD_Scene_Main::load(){
	Scene::load();	
	uiLayer->load();
	screenSurface->load();
	screenSurfaceShader->load();
	screenFBO->load();
}

void PD_Scene_Main::unload(){
	uiLayer->unload();
	screenSurface->unload();
	screenSurfaceShader->unload();
	screenFBO->unload();
	Scene::unload();	
}

Texture * PD_Scene_Main::getToken(){
	// get texture size
	glm::uvec2 sd = sweet::getWindowDimensions();
	sd.x = sd.y = glm::min(sd.x, sd.y);
	sd /= 4;
	glm::vec2 half = glm::vec2(sd)*0.5f;

	// hide the UI
	uiLayer->setVisible(false);

	//re-draw the current frame (swap the buffers a second time to avoid this render actually being visible)
	game->draw(this);
	glfwSwapBuffers(sweet::currentContext);
		
	// allocate enough space for our token and read the center of the newly drawn screen into it
	ProgrammaticTexture * res = new ProgrammaticTexture(nullptr, true);
	res->allocate(sd.x, sd.y, 4);
	glReadPixels(game->viewPortWidth/2 - half.x, game->viewPortHeight/2 - half.y, sd.x, sd.y, GL_RGBA, GL_UNSIGNED_BYTE, res->data);

	// carve out a circle and add a border
	for(signed long int y = 0; y < sd.y; ++y){
		for(signed long int x = 0; x < sd.x; ++x){
			glm::vec2 pos(x,y);
			float d = glm::distance(pos, half);
			if(d >= half.x || d >= half.y){
				sweet::TextureUtils::setPixel(res, x, y, glm::uvec4(0,0,0,0));
			}else if(d >= half.x * 0.9f || d >= half.y * 0.9f){
				sweet::TextureUtils::setPixel(res, x, y, glm::uvec4(237,22,106,255));
			}
		}
	}

	// unhide the UIyel
	uiLayer->setVisible(true);

	return res;
}

void PD_Scene_Main::resetCrosshair() {
	// replace the crosshair item texture with the actual crosshair texture
	crosshairIndicator->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	crosshairIndicator->setWidth(16);
	crosshairIndicator->setHeight(16);
	crosshairIndicator->invalidateLayout();
}

void PD_Scene_Main::save() {
	Json::Value saveOut;
	if(plotPosition != kEND) {
		int pos = static_cast<int>(plotPosition);
		saveOut["plotPosition"] = ++pos;
		saveOut["strength"] = player->dissStats->getStrength();
		saveOut["sass"] = player->dissStats->getSass();
		saveOut["defense"] = player->dissStats->getDefense();
		saveOut["insight"] = player->dissStats->getInsight();
		for(unsigned long int i = 0; i < uiDissBattle->lifeTokens.size(); ++i) {
			std::string fileName = "life_token_" + std::to_string(i) + ".tga";
			uiDissBattle->lifeTokens[i]->saveImageData(fileName);
			saveOut["lifeTokens"].append(fileName);
		}

		std::ofstream saveFile;
		saveFile.open ("data/save.json");
		saveFile << saveOut;
		saveFile.close();
	}else {
		// Delete save file
	}
}

void PD_Scene_Main::loadSave() {
	if(sweet::FileUtils::fileExists("data/save.json")){
		std::string saveJson = sweet::FileUtils::readFile("data/save.json");
		Json::Reader reader;
		Json::Value root;
		bool parsingSuccsessful = reader.parse(saveJson, root);
		assert(parsingSuccsessful);
		plotPosition = static_cast<ScenarioOrder>(root["plotPosition"].asInt());
		player->dissStats->incrementStrength(root["strength"].asInt());
		player->dissStats->incrementSass(root["sass"].asInt());
		player->dissStats->incrementDefense(root["defense"].asInt());
		player->dissStats->incrementInsight(root["insight"].asInt());
		for(auto tex : root["lifeTokens"]) {
			Texture * texture = new Texture("data/images/" + tex.asString(), true, true);
			texture->load();
			uiDissBattle->addLife(texture);
		}
	}
}


void PD_Scene_Main::updateSelection(){
	if(player->isEnabled()){
		NodeBulletBody * lastHoverTarget = currentHoverTarget;
		btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(0,0,0),btVector3(0,0,0));
		NodeBulletBody * me = bulletWorld->raycast(activeCamera, 4, &rayCallback);
		
		if(me != nullptr && uiInventory->getSelected() == nullptr){
			PD_Item * item = dynamic_cast<PD_Item *>(me);
			if(item != nullptr){
				if(item->actuallyHovered(glm::vec3(rayCallback.m_hitPointWorld.getX(), rayCallback.m_hitPointWorld.getY(), rayCallback.m_hitPointWorld.getZ()))){
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
								currentRoom->removeItem(item);

								// run item pickup triggers
								item->triggerPickup();

								uiBubble->clear();
							});
						}else{
							uiBubble->addOption("Use " + item->definition->name, [this, item](sweet::Event * _event){
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
				PD_Character * person = dynamic_cast<PD_Character*>(me);
				if(person != nullptr && person->isEnabled()){
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
								dialogue["text"].append((person->dissedAt ? (person->wonDissBattle ? incidentalPhraseGenerator.getLineWon() : incidentalPhraseGenerator.getLineLost()) : incidentalPhraseGenerator.getLineNormal(person)));
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
						if(!person->dissedAt){
							uiBubble->addOption("Diss " + person->definition->name, [this, person](sweet::Event * _event){
								triggerDissBattle(person);
								// TODO: pass in the character that's fighting here
							});
						}
						// if we have an item, also add the "use on" option
						/*if(uiInventory->getSelected() != nullptr){
							uiBubble->addOption("Use " + uiInventory->getSelected()->definition->name + " on " + person->definition->name, [this](sweet::Event * _event){
								uiBubble->clear();
								player->disable();
								// TODO: pass in the character that's interacting with the item here
							});
						}*/
					}
				}else{
					// prop carrying selection
					PD_Prop * prop = dynamic_cast<PD_Prop*>(me);
					if(prop != nullptr){
						if(mouse->leftJustPressed()){
							carriedProp = prop;
							carriedPropDistance = glm::distance(player->playerCamera->childTransform->getWorldPos(), carriedProp->meshTransform->getWorldPos());
							carriedProp->body->setDamping(0.8f,0.5f);
							carriedProp->body->setGravity(btVector3(0,0,0));
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
					uiInventory->getSelected()->triggerInteract();
					auto item = uiInventory->removeSelected();
					auto items = PD_Listing::listings[item->definition->scenario]->items;
					items.erase(items.find(item->definition->id));
					delete item;
					resetCrosshair();

				});
				uiBubble->addOption("Drop " + uiInventory->getSelected()->definition->name, [this](sweet::Event * _event){
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

					resetCrosshair();
				});
			}
		}
	}
}