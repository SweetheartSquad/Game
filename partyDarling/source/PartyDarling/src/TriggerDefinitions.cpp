#pragma once

#include <PD_Scene_Main.h>
#include <PD_Scene_MenuMain.h>
#include <PD_Scene_IntermissionSlideshow.h>
#include <PD_ResourceManager.h>
#include <Player.h>
#include <PD_DissStats.h>
#include <PD_UI_Text.h>

void PD_Scene_Main::setupConditions(){
	
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

	(*PD_ResourceManager::conditionImplementations)["checkGlobalInt"] = [this](sweet::Event * _event){
		// check an integer variable that has been set
		// STRING name
		// INT desiredValue
		
		std::string name = _event->getStringData("name");
		int desiredValue = _event->getIntData("desiredValue", INT_MAX);
		Scenario * scenario = PD_Listing::listingsById[_event->getStringData("scenario")]->scenario;

		if(name == "" || desiredValue == INT_MAX) {
			ST_LOG_ERROR("Missing argument in condition checkInt");
		}

		int curVal = PD_Game::progressManager->getInt(name);
		
		return curVal == desiredValue;
	};

	(*PD_ResourceManager::conditionImplementations)["checkGlobalString"] = [this](sweet::Event * _event){
		// check an integer variable that has been set
		// STRING name
		// STRING desiredValue
		
		std::string name = _event->getStringData("name");
		std::string desiredValue = _event->getStringData("desiredValue", "NO_VALUE");
		Scenario * scenario = PD_Listing::listingsById[_event->getStringData("scenario")]->scenario;

		if(name == "" || desiredValue == "NO_VALUE") {
			ST_LOG_ERROR("Missing argument in condition checkString");
		}
		std::string curVal = PD_Game::progressManager->getString(name);
		return curVal == desiredValue;
	};

	(*PD_ResourceManager::conditionImplementations)["wonLastYellingContest"] = [this](sweet::Event * _event){
		//checks if the last diss battle was won. If no diss battles have occurred than false is returned
		return player->wonLastDissBattle;
	};
}

void PD_Scene_Main::setupEventListeners(){
	
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

	PD_ResourceManager::scenario->eventManager->addEventListener("setGlobalInt", [](sweet::Event * _event){
		// change/create a local int variable for a specific scenario
		// STRING name 
		// INT value
		std::string name = _event->getStringData("name");
		int value = _event->getIntData("value", INT_MAX);

		if(name == "" || value == INT_MAX) {
			ST_LOG_ERROR("Missing argument in trigger setInt");
		}
		PD_Game::progressManager->setInt(name, value);
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("setGlobalString", [](sweet::Event * _event){
		// change/create a local string variable for a specific scenario
		// STRING name 
		// STRING value
		std::string name = _event->getStringData("name");
		std::string value = _event->getStringData("value", "NO_VALUE");
	
		if(name == "" || value == "NO_VALUE") {
			ST_LOG_ERROR("Missing argument in trigger setString");
		}
		PD_Game::progressManager->setString(name, value);
	});

	// Called when going through a door
	PD_ResourceManager::scenario->eventManager->addEventListener("navigate", [this](sweet::Event * _event){
		glm::ivec2 navigation(_event->getIntData("x"), _event->getIntData("y"));
		
		PD_ResourceManager::scenario->getAudio("doorOpen")->sound->play();
		player->disable();
		uiBubble->disable();
		
		
		transition = 0.f;
		transitionTarget = 1.f;

		screenSurfaceShader->bindShader();
		wipeColour = Colour::getRandomFromHsvMean(glm::ivec3(300, 67, 61), glm::ivec3(30, 25, 25));
		
		Timeout * t = new Timeout(1.f, [this, navigation](sweet::Event * _event){
			navigate(navigation);

			PD_ResourceManager::scenario->getAudio("doorClose")->sound->play();
			player->enable();
			uiBubble->enable();
		});
		t->start();
		childTransform->addChild(t, false);

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
	PD_ResourceManager::scenario->eventManager->addEventListener("fadeIn", [this](sweet::Event * _event){
		uiFade->fadeIn(glm::uvec3(
			_event->getIntData("r"),
			_event->getIntData("g"),
			_event->getIntData("b")
		),
		_event->getIntData("length") / 1000.f);
	});
	PD_ResourceManager::scenario->eventManager->addEventListener("fadeOut", [this](sweet::Event * _event){
		uiFade->fadeOut(glm::uvec3(
			_event->getIntData("r"),
			_event->getIntData("g"),
			_event->getIntData("b")
		),
		_event->getIntData("length") / 1000.f);
	});

	
	PD_ResourceManager::scenario->eventManager->addEventListener("locked", [this](sweet::Event * _event){
		// Triggered when the player tries to open a locked door
		uiBubble->options.front()->label->setText("This door is locked.");
		PD_ResourceManager::scenario->getAudio("doorLocked")->sound->play();
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("changeDISSStat", [this](sweet::Event * _event){
		// Trigger
		// Modifies the DISS stats of the player. The stat can be chose from Defense, Insight, Strength, Sass. Delta is the amount to change the stat by, and can be positive or negative.
		// STRING stat
		// INT delta
		player->dissStats->setLastStats(); // so we get acurate increment values later

		std::string stat = _event->getStringData("stat");
		int delta = _event->getIntData("delta", -9999);

		if(stat == "" || delta == -9999) {
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

		uiDissStats->playChangeDissStat();
	});


	PD_ResourceManager::scenario->eventManager->addEventListener("addFriendToken", [this](sweet::Event * _event){
		// Trigger
		// Adds a friend token to the player
		addLifeToken();
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("changerOwnership", [this](sweet::Event * _event){
		// Trigger
		// Takes an item from a character and gives it to another character. If the previous owner does not actually have the item, it should do nothing.
		// CHARACTER newOwner
		// ITEM item
		// CHARACTER prevOwner
		std::string newOwnerCharId = _event->getStringData("newOwner");
		std::string itemId = _event->getStringData("item");
		std::string prevOwnerCharId = _event->getStringData("prevOwner");
		std::string scenarioId = _event->getStringData("scenario");

		if(newOwnerCharId == "" || itemId == "" || prevOwnerCharId == "") {
			ST_LOG_ERROR_V("Missing field in trigger changerOwnership")
		}
		auto listing = PD_Listing::listingsById[scenarioId];
		PD_Item * item = listing->items[itemId];
		
		if(newOwnerCharId == prevOwnerCharId) {
			ST_LOG_ERROR_V("Invalid arguments is trigger changeOwnership - owner == prevOwner");
		}

		bool prevOwnerHasItem = false;
		if(prevOwnerCharId == "0"){
			for(auto it : uiInventory->items){
				if(it == item){
					prevOwnerHasItem = true;
					break;
				}
			}
		}else{
			for(auto it : listing->characters[prevOwnerCharId]->items) {
				if(it == item->definition->id) {
					prevOwnerHasItem = true;
					break;
				}
			}
		}

		if(prevOwnerHasItem){
			if(newOwnerCharId == "0"){
				uiItemGainLoss->gainItem(item);
				uiInventory->pickupItem(item);
			}else{
				listing->characters[newOwnerCharId]->items.push_back(item->definition->id);
				if (prevOwnerCharId == "0"){
					uiItemGainLoss->loseItem(item);
					uiInventory->removeItem(item);
				}	
			}
			if(prevOwnerCharId != "0"){
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

	PD_ResourceManager::scenario->eventManager->addEventListener("unlockRoom", [this](sweet::Event * _event){
		//Unlock the chosen room. If it is already unlocked, nothing will happen.
		// ROOM room = room to unlock
		std::string room = _event->getStringData("room");
		std::string scenario = _event->getStringData("scenario");

		if(room == "") {
			ST_LOG_ERROR_V("Missing field on trigger unlockRoom");
		}

		PD_Listing::listingsById[scenario]->rooms[room]->locked = false;

		// update the map to get rid of the locked icon for the room
		uiMap->updateMap(currentHousePosition);
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("unlockLab", [this](sweet::Event * _event){
		// unlocks the lab room for the run
		labRoom->locked = false;

		// update the map to get rid of the locked icon for the room
		uiMap->updateMap(currentHousePosition);
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("triggerYellingContest", [this](sweet::Event * _event){
		// Launch a diss battle with the selected character. 
		// playerInterjectInit is a boolean. If true, the player interjects first, if fasle, the player insults first.
		
		// CHARACTER oponent = character the player is fighting
		// INT(BOOLEAN) playerInterjectInit = true = player interects first, false = player insults first
		std::string opponent = _event->getStringData("opponent");
		bool interjectingFirst = _event->getIntData("playerInterjectInit") == 1;
		std::string scenario = _event->getStringData("scenario");

		if(opponent == "") {
			ST_LOG_ERROR_V("Missing field on trigger triggerDissBattle");
		}
		
		// TODO - Configure addtional data once the diss battle is set up for it
		
		PD_Character * enemy = PD_Listing::listingsById[scenario]->characters[opponent];
		uiDialogue->setVisible(false);
		uiBubble->disable();
		triggerDissBattle(enemy, interjectingFirst);
		uiDissBattle->eventManager->addEventListener("complete", [this](sweet::Event * _event){
			if(uiDialogue->hadNextDialogue){
				uiDialogue->setVisible(true);
				uiBubble->enable();
			}
			player->wonLastDissBattle = _event->getIntData("win") == 1;
		});
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("hideCharacter", [this](sweet::Event * _event){
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
		
		if(visible == 1) {
			person->show();

			//////////////////////////////////////////////
			// SPECIAL CASE FOR SCIENTIST OMAR IN LAB_5 //
			//////////////////////////////////////////////
			if(PD_Game::progressManager->plotPosition == 5){
				glm::vec3 v = player->playerCamera->forwardVectorRotated;
				v.y = 0;
				v = glm::normalize(v);
				person->translatePhysical(player->getWorldPos() - v*8.f);
				v = person->getPhysicsBodyCenter();
				if(glm::abs(v.x) < glm::abs(v.z)){
					v.x = 0;
				}else{
					v.z = 0;
				}
				person->translatePhysical(v, false);
			}
		}else {
			person->hide();
		}
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("disableCharacter", [](sweet::Event * _event){
		// hide a character in a room until they need to appear
		// CHARACTER name
		// (BOOL)INT visibility

		std::string character = _event->getStringData("name");
		int visible = _event->getIntData("enabled", -1);
		std::string scenario = _event->getStringData("scenario");

		if(character == "" || visible == -1) {
			ST_LOG_ERROR_V("Missing field on trigger disableCharacter");
		}

		PD_Character * person = PD_Listing::listingsById[scenario]->characters[character];
		
		if(visible == 1) {
			person->enable();
		}else {
			person->disable();
		}
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("goToNextLevel", [this](sweet::Event * _event){
		uiDialogue->setVisible(false);
		uiBubble->disable();		
		if(PD_Game::progressManager->plotPosition != kEPILOGUE){
			++PD_Game::progressManager->plotPosition;
			// Make sure to save the game 
			PD_Game::progressManager->save(player, uiDissBattle);
			game->scenes["intermission"] = new PD_Scene_IntermissionSlideshow(game, PD_Game::progressManager->plotPosition);
			game->switchScene("intermission", true);	
		}else {
			PD_Game::progressManager->eraseSave();
			PD_Game::progressManager->plotPosition = kBEGINNING;
			dynamic_cast<PD_Scene_MenuMain *>(game->scenes.at("menu"))->continueText->disable();
 			game->switchScene("menu", true);
		}
	});

	PD_ResourceManager::scenario->eventManager->addEventListener("goToConversation", [this](sweet::Event * _event){
		uiDialogue->currentConversation = uiDialogue->currentConversation->scenario->getConversation(_event->getStringData("conversation"))->conversation;
		uiDialogue->currentConversation->reset();
		uiBubble->disable();
		uiDialogue->sayNext();
		uiBubble->enable();
	});
}
