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
#include <ShaderComponentOutline.h>

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

#include <PD_Scene_MenuMain.h>
#include <PD_UI_Text.h>

#define MAX_SIDE_SCENARIOS 5

PD_Scene_Main::PD_Scene_Main(PD_Game * _game) :
	Scene(_game),
	panSpeed(20.f),
	panLeft(false),
	panRight(false),
	trackSpeed(0.1f),
	trackLeft(false),
	trackRight(false),
	screenSurfaceShader(new Shader("assets/RenderSurface", false, false)),
	screenSurface(new RenderSurface(screenSurfaceShader, false)),
	screenFBO(new StandardFrameBuffer(false)),
	uiLayer(new UILayer(0,0,0,0)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr),
	selectedItem(nullptr),
	toonShader(new ComponentShaderBase(false)),
	itemShader(new ComponentShaderBase(false)),
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
	carriedPropDistance(0),
	wipeColour(glm::ivec3(125/255.f,200/255.f,50/255.f)),
	dissEnemy(nullptr),
	playerStartsDissBattle(true)
{
	_game->showLoading(0);

	player = new Player(bulletWorld);
	uiBubble = new PD_UI_Bubble(uiLayer->world);
	uiDissBattle = new PD_UI_DissBattle(uiLayer->world, player, PD_ResourceManager::scenario->getFont("FIGHT-FONT")->font, uiBubble->textShader, uiLayer->shader);
	// Load the save file
	Log::warn("before RNG:\t" + std::to_string(sweet::NumberUtils::numRandCalls));
	PD_Game::progressManager->loadSave(player, uiDissBattle);
	Log::warn("start RNG:\t" + std::to_string(sweet::NumberUtils::numRandCalls));

	toonRamp = new RampTexture(lightStart, lightEnd, 4, false);
	toonShader->addComponent(new ShaderComponentMVP(toonShader));
	toonShader->addComponent(new PD_ShaderComponentSpecialToon(toonShader, toonRamp, true));
	toonShader->addComponent(new ShaderComponentTexture(toonShader, 0));
	toonShader->compileShader();

	itemShader->addComponent(new ShaderComponentMVP(itemShader));
	itemShader->addComponent(new ShaderComponentOutline(itemShader, 0));
	itemShader->compileShader();


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


	uiFade = new PD_UI_Fade(uiLayer->world);
	uiLayer->addChild(uiFade);
	uiFade->setRationalHeight(1.f, uiLayer);
	uiFade->setRationalWidth(1.f, uiLayer);
	
	uiMap = new PD_UI_Map(uiLayer->world, PD_ResourceManager::scenario->getFont("FONT")->font, uiBubble->textShader);
	uiLayer->addChild(uiMap);
	uiMap->setRationalHeight(1.f, uiLayer);
	uiMap->setRationalWidth(1.f, uiLayer);
	uiMap->enable();
	

	uiBubble->setRationalWidth(1.f, uiLayer);
	uiBubble->setRationalHeight(0.25f, uiLayer);
	uiLayer->addChild(uiBubble);

	uiInventory = new PD_UI_Inventory(uiLayer->world, player);
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
		crosshairIndicator->autoResize();
		crosshairIndicator->invalidateLayout();
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
	childTransform->addChild(player);
	cameras.push_back(player->playerCamera);
	activeCamera = player->playerCamera;
	childTransform->addChild(player->playerCamera);
	player->playerCamera->firstParent()->translate(0, 5, 0);

	
	uiLayer->addChild(uiDissBattle);
	uiDissBattle->setRationalHeight(1.f, uiLayer);
	uiDissBattle->setRationalWidth(1.f, uiLayer);

	uiDissBattle->eventManager->addEventListener("complete", [this](sweet::Event * _event){
		uiDissBattle->disable();
		uiDissStats->playOutro(uiDissBattle->wonXP);
		player->wonLastDissBattle = _event->getIntData("win") == 1;
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


	uiMessage = new PD_UI_Message(uiLayer->world);
	uiLayer->addChild(uiMessage);
	uiMessage->setRationalWidth(1.f, uiLayer);
	uiMessage->setRationalHeight(1.f, uiLayer);


	playerLight = new PointLight(glm::vec3(lightIntensity), 0.0f, 0.003f, -1);
	player->playerCamera->childTransform->addChild(playerLight);
	playerLight->firstParent()->translate(0.f, 1.f, 0.f);
	lights.push_back(playerLight);

	// Set the scenario condition implentations pointer
	PD_ResourceManager::scenario->conditionImplementations = PD_ResourceManager::conditionImplementations;

	// Setup conditions
	setupConditions();

	// setup event listeners
	// make sure to clear the old ones in case they already exist
	PD_ResourceManager::scenario->eventManager->listeners.clear();
	setupEventListeners();

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

	uiDissStats = new PD_UI_DissStats(uiLayer->world, player, uiLayer->shader);
	uiDissStats->setRationalWidth(1.f, uiLayer);
	uiDissStats->setRationalHeight(1.f, uiLayer);
	uiDissStats->setVisible(false);
	uiLayer->addChild(uiDissStats);

	uiDissStats->eventManager->addEventListener("introComplete", [this](sweet::Event * _event){
		uiDissBattle->startNewFight(dissEnemy, playerStartsDissBattle);
	});
	
	uiDissStats->eventManager->addEventListener("outroComplete", [this](sweet::Event * _event){
		if(!uiDialogue->hadNextDialogue){
			player->enable();
			currentHoverTarget = nullptr;
			updateSelection();
		}
	});

	uiDissStats->eventManager->addEventListener("changeDissStatComplete", [this](sweet::Event * _event){
		if(!uiDialogue->hadNextDialogue){
			player->enable();
			currentHoverTarget = nullptr;
			updateSelection();
		}
	});

	Log::warn("end RNG:\t" + std::to_string(sweet::NumberUtils::numRandCalls));
	_game->showLoading(1.f);
}


void PD_Scene_Main::pickScenarios(){
	for(auto scenarioDef : PD_Game::progressManager->currentScenarios["scenarios"]) {
		activeScenarios.push_back(new PD_Scenario("assets/" + scenarioDef.asString()));
		activeScenarios.back()->load();
	}

	//activeScenarios.push_back(new PD_Scenario("assets/scenarios/Adventure_Omar.json"));
	//activeScenarios.push_back(new PD_Scenario("assets/scenario-external-2.json"));
	//activeScenarios.push_back(new PD_Scenario("assets/scenario-intro.json"));
	//activeScenarios.push_back(new Scenario("assets/scenario-external-3.json"));
	// set event managers on selected scenarios as children of the global scenario
	for(auto s : activeScenarios){
		PD_ResourceManager::scenario->eventManager->addChildManager(s->eventManager);
		s->conditionImplementations = PD_ResourceManager::conditionImplementations;
		
		// create a listing for this scenario
		PD_Listing * listing = new PD_Listing(s);
	}

	
	// pop the last two scenarios off of the file (they're the intro and lab scenario for this run)
	labScenario = activeScenarios.back();
	activeScenarios.pop_back();
	introScenario = activeScenarios.back();
	activeScenarios.pop_back();

	if(labScenario->getAsset("room","1") == nullptr){
		Json::Value r;
		r["id"] = "1";
		r["type"] = "room";
		r["locked"] = true;
		labScenario->assets["room"]["1"] = AssetRoom::create(r, labScenario);
	}if(introScenario->getAsset("room","1") == nullptr){
		Json::Value r;
		r["id"] = "1";
		r["type"] = "room";
		introScenario->assets["room"]["1"] = AssetRoom::create(r, introScenario);
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
	
	sweet::ShuffleVector<glm::ivec2> openCells;

	// place the starting room in the starting position
	houseGrid[std::make_pair(currentHousePosition.x, currentHousePosition.y)] = introRoom;
	allCells[std::make_pair(currentHousePosition.x, currentHousePosition.y)] = false;
	if(currentHousePosition.x == 0){
		introRoom->setEdge(PD_Door::kWEST);
	}else if(currentHousePosition.x == houseSize-1){
		introRoom->setEdge(PD_Door::kEAST);
	}else if(currentHousePosition.y == 0){
		introRoom->setEdge(PD_Door::kNORTH);
	}else{
		introRoom->setEdge(PD_Door::kSOUTH);
	}

	for (unsigned long int i = 0; i < possibleBlockedCellPositions.size() / 3 * 2; ++i){
		glm::ivec2 pos = possibleBlockedCellPositions.pop();
		if (pos != currentHousePosition){
			blockedPositions.push(pos);
			allCells[std::make_pair(pos.x, pos.y)] = false;
		}
	}


	// place the cells adjacent to the starting position into the list of open cells
	openCells.push(getAdjacentCells(currentHousePosition, allCells, houseSize));

	// place the unlocked rooms by picking a random open cell and a random room,
	// assigning the room to the cell, and storing any open adjacent cells in the list
	while(unlockedRooms.size() > 0){
		glm::ivec2 cell;
		if(openCells.size() > 0){
			cell = openCells.pop(true); // make sure to remove the cell from the shuffle vector
		}else if(blockedPositions.size() > 0){
			cell = blockedPositions.pop(true); // if we ran out of possible places to go, use one of the pre-blocked cells instead
		}else{
			Log::error("Room can't be placed!");
		}
		allCells[std::make_pair(cell.x, cell.y)] = false;
		Room * room = unlockedRooms.pop(true);
		if (houseGrid.count(std::make_pair(cell.x, cell.y)) != 0){
			Log::error("House position already used.");
		}
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

		if (houseGrid.count(std::make_pair(cell.x, cell.y)) != 0){
			Log::error("House position already used.");
		}
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

	if (houseGrid.count(std::make_pair(cell.x, cell.y)) != 0){
		Log::error("House position already used.");
	}
	houseGrid[std::make_pair(cell.x, cell.y)] = labRoom;


	
	// loop through all of the rooms and remove doors which don't lead anywhere
	// and save the door positions while we're at it
	float doorSpacing = 1.5f; // how far in front of the door the player is placed when entering
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
		_cells.at(std::make_pair(temp.x, temp.y)) = false;
	}

	temp = _pos + glm::ivec2(1,0);
	if(temp.x < _maxSize && _cells.at(std::make_pair(temp.x, temp.y))){
		res.push_back(temp);
		_cells.at(std::make_pair(temp.x, temp.y)) = false;
	}

	temp = _pos + glm::ivec2(0,-1);
	if(temp.y >= 0 && _cells.at(std::make_pair(temp.x, temp.y))){
		res.push_back(temp);
		_cells.at(std::make_pair(temp.x, temp.y)) = false;
	}

	temp = _pos + glm::ivec2(0,1);
	if(temp.y < _maxSize && _cells.at(std::make_pair(temp.x, temp.y))){
		res.push_back(temp);
		_cells.at(std::make_pair(temp.x, temp.y)) = false;
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

		// build the rooms in this scenario
		for(auto rd : scenario->assets.at("room")){
			g->showLoading((float)++progress/numRooms);
			Room * room = RoomBuilder(dynamic_cast<AssetRoom *>(rd.second), bulletWorld, toonShader, itemShader, characterShader, emoteShader).getRoom();
			
			// run the physics simulation for a few seconds to let things settle
			/*Log::info("Letting the bodies hit the floor...");
			Step s;
			s.setDeltaTime(1/60.f);
			//unsigned long int i = bulletWorld->maxSubSteps;
			//bulletWorld->maxSubSteps = 10000;
			//bulletWorld->update(&s);
			//bulletWorld->maxSubSteps = i;
			for (unsigned long int i = 0; i<10; ++i){
				for(auto c : room->components){
					c->body->activate(true);
				}
				bulletWorld->update(&s);
			}
			Log::info("The bodies have finished hitting the floor.");*/

			// remove the physics bodies (we'll put them back in as needed)
			room->removePhysics();


			// save the room for later access
			PD_Listing::listings.at(room->definition->scenario)->addRoom(room);

			// put the room into the shuffle vector
			res.push_back(room);
		}
	}


	// construct static rooms (into room, lab room)
	res.push_back(new LabRoom(bulletWorld, toonShader, characterShader, emoteShader, labScenario));
	res.push_back(new IntroRoom(bulletWorld, toonShader, characterShader, emoteShader, introScenario));


	return res;
}

void PD_Scene_Main::triggerDissBattle(PD_Character * _enemy, bool _playerStarts) {
	playerStartsDissBattle = _playerStarts;
	dissEnemy = _enemy;
	uiBubble->clear();
	player->disable();
	uiDissStats->playIntro(dissEnemy);
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
	glm::vec3 pos = currentRoom->doorPositions.at(doorToEnter);
	pos.y = player->childTransform->getTranslationVector().y;
	player->translatePhysical(pos, false);

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
	
	// add the intro/lab scenarios back to the active list so they get deleted too
	activeScenarios.push_back(introScenario);
	activeScenarios.push_back(labScenario);
	// delete all of the scenarios
	while(activeScenarios.size() > 0){
		PD_Listing::removeListing(activeScenarios.back());
		delete activeScenarios.back();
		activeScenarios.pop_back();
	}

	delete bulletWorld;
	delete toonShader;
	delete itemShader;
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


void PD_Scene_Main::addLifeToken(std::string _name) {
	Texture * tex = getToken();
	tex->load();
	uiDissBattle->addLife(tex);

	uiMessage->gainLifeToken(_name, tex);
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
		addLifeToken("Some Person");
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
	if(keyboard->keyJustDown(GLFW_KEY_7)){
		uiMessage->displayMessage("Wow this is such an amazing game!!! :)");
	}
	if(keyboard->keyJustDown(GLFW_KEY_0)){
		PD_ResourceManager::scenario->eventManager->triggerEvent("goToNextLevel");
	}


	// look up at current speaker's face during conversations
	PD_Character * facing = nullptr;
	if(uiDialogue->isVisible()){
		if(uiDialogue->currentSpeaker != nullptr){
			facing = uiDialogue->currentSpeaker;
		}
	}else if(uiDissBattle->isVisible() || (uiDissStats->dissEnemy && uiDissStats->isVisible())){
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
	float newa = fmod(_step->time, 142.f/300.f)*0.01f + 0.01f;
	playerLight->setAttenuation(newa);
	if(newa < a){
		lightStart = glm::vec3((glm::sin(_step->time)*0.5f+0.5f)*0.2f + 0.3f, (glm::sin(_step->time*19)*0.5f+0.5f)*0.2f + 0.3f, (glm::sin(_step->time*17)*0.5f+0.5f)*0.2f + 0.3f);
		lightEnd = glm::vec3((glm::sin(_step->time*29)*0.5f+0.5f)*0.6f + 0.9f, (glm::sin(_step->time*7)*0.5f+0.5f)*0.6f + 0.9f, (glm::sin(_step->time*3)*0.5f+0.5f)*0.6f + 0.9f);
		lightIntensity = (glm::sin(_step->time*31)*0.5f+0.5f)*0.25f+1.f * currentRoom->lights.size() > 0 ? 0.05f : 1.4f;
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
			if(dl > 1.0f){
				d = glm::normalize(d);
			}
			carriedProp->applyLinearImpulseToCenter(d/carriedProp->body->getInvMass());
		}
	}
	
	// inventory toggle
	if(!uiDialogue->isVisible() && !uiDissBattle->isVisible() && !uiDissStats->isVisible()){
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
		PD_Game::progressManager->save(player, uiDissBattle);
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
	sd /= 3;
	glm::vec2 half = glm::vec2(sd)*0.5f;

	// hide the UI
	uiLayer->setVisible(false);

	//re-draw the current frame (swap the buffers a second time to avoid this render actually being visible)
	game->draw(this);
	glfwSwapBuffers(sweet::currentContext);
		
	// allocate enough space for our token and read the center of the newly drawn screen into it
	ProgrammaticTexture * res = new ProgrammaticTexture(nullptr, true);
	res->allocate(sd.x, sd.y, 4);
	auto tempData = res->data;
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

	// flip the texture
	for(signed long int y = 0; y < sd.y; ++y){
		memcpy(&tempData[y*sd.x*4], &res->data[(sd.y-y-1)*sd.x*4], sd.x*4);
	}
	res->unloadImageData();
	res->data = tempData;

	// unhide the UIyel
	uiLayer->setVisible(true);

	return res;
}

void PD_Scene_Main::resetCrosshair() {
	// replace the crosshair item texture with the actual crosshair texture
	crosshairIndicator->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	crosshairIndicator->setWidth(16);
	crosshairIndicator->setHeight(16);
	crosshairIndicator->autoResize();
	crosshairIndicator->invalidateLayout();
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
					if(person != currentHoverTarget && person->isEnabled()){
						// if we aren't already looking at the person,
						// clear out the bubble UI and add the relevant options
						uiBubble->clear();
						uiBubble->addOption("Talk to " + person->definition->name, [this, person](sweet::Event * _event){
							std::string c = person->state->conversation;
							if(c == "NO_CONVO" || c == ""){
								// incidental conversation
								Json::Value dialogue;
								dialogue["text"].append((person->dissedAt ? (person->wonDissBattle ? person->incidentalLineWon.pop() : person->incidentalLineLost.pop()) : person->incidentalLineNormal.pop()));
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
								triggerDissBattle(person, true);
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
					PD_Item * item = uiInventory->removeSelected();
					if(item->definition->consumable){
						auto items = PD_Listing::listings[item->definition->scenario]->items;
						items.erase(items.find(item->definition->id));
						delete item;
					}else{
						uiInventory->pickupItem(item);
					}
					resetCrosshair();

				});
				uiBubble->addOption("Nevermind.", [this](sweet::Event * _event){
					// dropping an item
					PD_Item * item = uiInventory->removeSelected();
					uiInventory->pickupItem(item);
					resetCrosshair();
				});
			}
		}
	}
}