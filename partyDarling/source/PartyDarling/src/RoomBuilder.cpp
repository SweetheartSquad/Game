#pragma once

#include <RoomBuilder.h>

#include <Room.h>
#include <RoomObject.h>
#include <PD_Slot.h>
#include <MeshFactory.h>
#include <scenario\Asset.h>

#include <math.h>
#include <string.h>

#include <Resource.h>

#include <PD_Character.h>
#include <Item.h>
#include <PD_ResourceManager.h>

#include <stb/stb_image.h>

#include <PD_TilemapGenerator.h>
#include <TextureUtils.h>
#include <NumberUtils.h>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include <PD_Furniture.h>

#include <Sprite.h>
#include <Texture.h>

#include <PD_Assets.h>

#include <glm\gtc\quaternion.hpp>

#include <PD_Prop.h>
#include <PD_Door.h>

#include <PD_PhraseGenerator_Incidental.h>

//#define RG_DEBUG

Edge::Edge(glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _normal) :
	p1(_p1),
	p2(_p2),
	angle(glm::degrees(glm::atan(_normal.y, _normal.x))),
	slope((p2.y - p1.y) /  (p2.x - p1.x)),
	yIntercept(p1.y - slope * p1.x)
{
}

// https://github.com/sromku/polygon-contains-point/blob/master/Polygon/src/com/sromku/polygon/Polygon.java
bool Edge::intersects(Edge * _ray, float _scale){
	glm::vec2 intersectPoint;

	// if both vectors aren't from the kind of x=1 lines then go into
	if (!_ray->isVertical() && !isVertical()){
		// check if both vectors are parallel. If they are parallel then no intersection point will exist
		if (_ray->slope - slope == 0){
			return false;
		}

		float x = ((yIntercept * _scale - _ray->yIntercept) / (_ray->slope - slope)); // x = (b2-b1)/(a1-a2)
		float y = slope * x + yIntercept * _scale; // y = a2*x+b2
		intersectPoint = glm::vec2(x, y);
	}

	else if (_ray->isVertical() && !isVertical()){
		float x = _ray->p1.x;
		float y = slope * x + yIntercept * _scale;
		intersectPoint = glm::vec2(x, y);
	}

	else if (!_ray->isVertical() && isVertical()){
		float x = p1.x * _scale;
		float y = _ray->slope * x + _ray->yIntercept;
		intersectPoint = glm::vec2(x, y);
	}

	else{
		return false;
	}

	if (isInside(intersectPoint, _scale) && _ray->isInside(intersectPoint))
	{
		return true;
	}

	return false;
}

bool Edge::isVertical(){
	return p2.x - p1.x == 0;
}

bool Edge::isInside(glm::vec2 _point, float _scale){
	float maxX = p1.x > p2.x ? p1.x : p2.x;
	float minX = p1.x < p2.x ? p1.x : p2.x;
	float maxY = p1.y > p2.y ? p1.y : p2.y;
	float minY = p1.y < p2.y ? p1.y : p2.y;

	bool equalMaxX = abs(_point.x - maxX * _scale) <= FLT_EPSILON * 10;
	bool equalMinX = abs(_point.x - minX * _scale) <= FLT_EPSILON * 10;
	bool equalMaxY = abs(_point.y - maxY * _scale) <= FLT_EPSILON * 10;
	bool equalMinY = abs(_point.y - minY * _scale) <= FLT_EPSILON * 10;

	if (( (_point.x > minX * _scale || equalMinX) && (_point.x < maxX * _scale || equalMaxX) ) && ( (_point.y > minY * _scale || equalMinY) && (_point.y < maxY * _scale || equalMaxY) ))
	{
		return true;
	}
	return false;
}

RoomBuilder::RoomBuilder(AssetRoom * _definition, BulletWorld * _world, Shader * _baseShader, Shader * _characterShader, Shader * _emoteShader):
	world(_world),
	baseShader(_baseShader),
	characterShader(_characterShader),
	definition(_definition),
	emoteShader(_emoteShader)
{
}

RoomBuilder::~RoomBuilder(){
}

Room * RoomBuilder::getRoom(){
	room = new Room(world, baseShader, definition);

	// texture floor and ceiling planes
	room->floor->mesh->pushTexture2D(getFloorTex());
	room->ceiling->mesh->pushTexture2D(getCeilTex());

	thresh = 5;
	bool success;

	unsigned long int l, w;
	float fullL, fullW;

	switch (definition->size){
		case AssetRoom::Size_t::kSMALL: l = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/4, ROOM_SIZE_MAX/2); w = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/4, ROOM_SIZE_MAX/2); break;
		case AssetRoom::Size_t::kMEDIUM: l = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/3, ROOM_SIZE_MAX/1.5f); w = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/3, ROOM_SIZE_MAX/1.5f); break;
		case AssetRoom::Size_t::kLARGE: l = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/2, ROOM_SIZE_MAX); w = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/2, ROOM_SIZE_MAX); break;
		break;
	}

	// get mandatory objects
	std::vector<RoomObject *> objects = getSpecifiedObjects();
	std::sort(objects.begin(), objects.end(), [](RoomObject * i, RoomObject * j) -> bool{
		if(i->parentTypes.size() == 0 || j->parentTypes.size() == 0){
			return i->parentTypes.size() < j->parentTypes.size();
		}

		bool isParent = false;
		for(auto &parent: j->parentTypes){
			if(parent.parent == i->type){
				isParent = true;
				break;
			}
		}
		return (isParent);
	});

	int numAttempts = 0;

	do{
		++numAttempts;
		std::stringstream s;
		s << "ATTEMPT " << numAttempts; 
		Log::info(s.str());

		success = true;
		// convert size enum to actual numbers

		fullL = l * ROOM_TILE;
		fullW = ROOM_TILE * w;
	
		// Generate tilemap image
		room->tilemap = new PD_TilemapGenerator(l, w, true);
		unsigned long int pixelIncrement = 158;
		room->tilemap->configure(sweet::NumberUtils::randomInt(pixelIncrement, 255), pixelIncrement);
		room->tilemap->load();
		room->tilemap->saveImageData("tilemap.tga");

		createWalls();
		sweet::Box boundingBox = room->mesh->calcBoundingBox();
		roomUpperBound = boundingBox.getMaxCoordinate() * (1.f/ROOM_TILE);
		roomLowerBound = boundingBox.getMinCoordinate() * (1.f/ROOM_TILE);
		tiles = getTiles();
	
		// Add boundaries into shuffle vectors
		sweet::ShuffleVector<RoomObject *> shuffleBoundaries;
		for(auto boundary : boundaries){
			// only walls in boundaries should have child slots (not floor, cieling)
			if(boundary->emptySlots.size() > 0){
				shuffleBoundaries.push(boundary);
	#ifdef RG_DEBUG
				boundary->mesh->pushTexture2D(getDebugTex());
	#endif
			}
		}

		// Shuffle, and load into available parents
		while(shuffleBoundaries.shuffleCount() < 2){
			addObjectToLists(shuffleBoundaries.pop());
		}
	
		// place doors before anything else
		if(placeDoors()){
			for(auto obj : objects){
				std::string name = (obj->mesh->textures.size() > 0 ? obj->mesh->textures.at(0)->src : std::string(" noTex"));
				std::stringstream s1;
				s1 << "\nAttempting to place OBJECT: " << name;
				Log::info(s1.str());
				std::stringstream s;
				s << "Total Placed Objects: " << placedObjects.size();
				Log::info(s.str());
				if(!search(obj)){
					Log::warn("Search FAILED; room object not placed.");
					success = false;
					break;
				}else{
					Log::info("Search SUCCEEDED.");
		#ifdef RG_DEBUG
					if(obj->parent != nullptr && obj->parent->mesh->textures.size() > 0){
						obj->mesh->pushTexture2D(obj->parent->mesh->textures.at(0));
					}
		#endif
				}
			}
		}else{
			success = false;
		}

		if(!success){
			if(numAttempts > MAX_ROOMBUILDER_ATTEMPTS){
				Log::error("Max attempts reached, failed to build room");
			}

			Log::warn("INCREASING ROOM SIZE.");
			// unload walls, increase size
			if(room->world != nullptr && room->body != nullptr) {
				room->world->world->removeRigidBody(room->body);
				delete room->body;
				delete room->shape;
				room->body = nullptr;
				room->shape = nullptr;
			}
			room->mesh->vertices.clear();

			delete room->tilemap;
			room->tilemap = nullptr;

			for(auto e : edges){
				delete e;
			}
			edges.clear();

			availableParents.clear();

			for(auto o : placedObjects){
				if(o != nullptr){
					o->resetObject();
					o->realign();
					o->meshTransform->makeCumulativeModelMatrixDirty();
					room->removeComponent(o);
				}
			}
			placedObjects.clear();

			for(auto b : boundaries){
				delete b;
			}
			boundaries.clear();
			
			tiles.clear();

			typedef std::map<PD_Door::Door_t, PD_Door *>::iterator it_type;
			for(it_type it = room->doors.begin(); it != room->doors.end(); it++) {
				delete it->second;
			}
			room->doors.clear();

			l += 1;
			w +=1;
		}
	} while (!success);
	
	// Random room stuff
	objects.clear();
	objects = getRandomObjects();
	std::sort(objects.begin(), objects.end(), [](RoomObject * i, RoomObject * j) -> bool{
		if(i->parentTypes.size() == 0 || j->parentTypes.size() == 0){
			return i->parentTypes.size() < j->parentTypes.size();
		}

		bool isParent = false;
		for(auto &parent: j->parentTypes){
			if(parent.parent == i->type){
				isParent = true;
				break;
			}
		}
		return (isParent);
	});
	
	for(auto obj : objects){
		std::string name = (obj->mesh->textures.size() > 0 ? obj->mesh->textures.at(0)->src : std::string(" noTex"));
		std::stringstream s1;
		s1 << "\nAttempting to place OBJECT: " << name;
		Log::info(s1.str());
		std::stringstream s;
		s << "Total Placed Objects: " << placedObjects.size();
		Log::info(s.str());
		if(!search(obj)){
			Log::warn("Search FAILED; room object not placed.");
			
			// make sure we remove the object from the item or character list if it's there
			if(PD_Character * c = dynamic_cast<PD_Character *>(obj)){
				PD_Listing::listings[c->definition->scenario]->characters.erase(c->definition->id);
				for(signed long int i = room->characters.size()-1; i >= 0; --i){
					if(room->characters.at(i) == obj){
						room->characters.erase(room->characters.begin() + i);
						break;
					}
				}
			}else if(dynamic_cast<PD_Item *>(obj) != nullptr){
				for(signed long int i = room->items.size()-1; i >= 0; --i){
					if(room->items.at(i) == obj){
						room->items.erase(room->items.begin() + i);
						break;
					}
				}
			}
			delete obj;
		}else{
			Log::info("Search SUCCEEDED.");
#ifdef RG_DEBUG
			if(obj->parent != nullptr && obj->parent->mesh->textures.size() > 0){
				obj->mesh->pushTexture2D(obj->parent->mesh->textures.at(0));
			}
#endif
		}
	}

	// Props
	objects.clear();

	std::vector<PD_Prop *> props = getProps();
	objects.insert(objects.begin(), props.begin(), props.end());
	std::sort(objects.begin(), objects.end(), [](RoomObject * i, RoomObject * j) -> bool{ return (i->parentTypes.size() < j->parentTypes.size());});
	int cntBlah = objects.size();
	for(auto obj : objects){
		std::string name = (obj->mesh->textures.size() > 0 ? obj->mesh->textures.at(0)->src : std::string(" noTex"));
		std::stringstream s1;
		s1 << "\nAttempting to place OBJECT: " << name;
		Log::info(s1.str());
		std::stringstream s;
		s << "Total Placed Objects: " << placedObjects.size();
		Log::info(s.str());
		if(!search(obj)){
			Log::warn("Search FAILED; room object not placed.");
			delete obj;
			--cntBlah;
		}else{
			Log::info("Search SUCCEEDED.");
#ifdef RG_DEBUG
			if(obj->parent != nullptr && obj->parent->mesh->textures.size() > 0){
				obj->mesh->pushTexture2D(obj->parent->mesh->textures.at(0));
			}
#endif
		}
	}
	std::stringstream sBlah;
	sBlah << "TOTAL PROPS: " << objects.size() << " PLACED: " << cntBlah;
	Log::info(sBlah.str());

	// copy lights from any placed objects into the rooms lights
	for(auto obj : placedObjects) {
		if(obj->lights.size() > 0){
			room->lights.insert(room->lights.end(), obj->lights.begin(), obj->lights.end());
		}
	}

	// Get rid of temporary boundary room objects
	for(auto boundary : boundaries){
#ifdef RG_DEBUG
			room->addComponent(boundary);
#else
			delete boundary;
#endif
	}
	
	room->floor->meshTransform->scale(-fullL-1, fullW+1, 1.f);
	room->floor->translatePhysical(room->getCenter(), false);
	#ifndef RG_DEBUG
		// adjust UVs so that the texture tiles in squares
		for(Vertex & v : room->floor->mesh->vertices){
			v.u *= l+1;
			v.v *= w+1;
		}
	#endif

	room->ceiling->meshTransform->scale(-fullL-1, fullW+1, -1.f);
	room->ceiling->translatePhysical(room->getCenter() + glm::vec3(0, ROOM_HEIGHT * ROOM_TILE, 0), false);
	#ifndef RG_DEBUG
		// adjust UVs so that the texture tiles in squares
		for(Vertex & v : room->ceiling->mesh->vertices){
			v.u *= l+1;
			v.v *= w+1;
		}
	#endif

	// Center room at origin
	room->translatePhysical(-room->getCenter(), true);

	availableParents.clear();
	placedObjects.clear();
	edges.clear();


	// generate incidental dialogue for each character
	PD_PhraseGenerator_Incidental phraseGenerator;
	for(auto c : room->characters){
		phraseGenerator.updateNames(c);
		for(unsigned long int i = 0; i < 3; ++i){
			c->incidentalLineLost.push(phraseGenerator.getLineLost());
			c->incidentalLineNormal.push(phraseGenerator.getLineNormal());
			c->incidentalLineWon.push(phraseGenerator.getLineWon());
		}
	}

	return room;
}

bool RoomBuilder::placeDoors(){

	PD_Door * doorNorth = new PD_Door(world, baseShader, PD_Door::kNORTH, getDoorTexIdx());
	PD_Door * doorSouth = new PD_Door(world, baseShader, PD_Door::kSOUTH, getDoorTexIdx());
	PD_Door * doorEast = new PD_Door(world, baseShader, PD_Door::kEAST, getDoorTexIdx());
	PD_Door * doorWest = new PD_Door(world, baseShader, PD_Door::kWEST, getDoorTexIdx());

	// Place new door
	if(!placeDoor(doorNorth)){
		Log::warn("North door not placed!!!");
	}
	if(!placeDoor(doorSouth)){
		Log::warn("South door not placed!!!");
	}
	if(!placeDoor(doorEast)){
		Log::warn("East door not placed!!!");
	}
	if(!placeDoor(doorWest)){
		Log::warn("West door not placed!!!");
	}

	room->doors.insert(std::make_pair(PD_Door::kNORTH, doorNorth));
	room->doors.insert(std::make_pair(PD_Door::kSOUTH, doorSouth));
	room->doors.insert(std::make_pair(PD_Door::kEAST, doorEast));
	room->doors.insert(std::make_pair(PD_Door::kWEST, doorWest));

	return true;
}

bool RoomBuilder::placeDoor(PD_Door * _door){
	float offset = 0;
	glm::vec3 center = room->getCenter();
	RoomObject * wall = nullptr;

	PD_Door::Door_t _side = _door->side;
	float max = (_side == PD_Door::Door_t::kNORTH || _side == PD_Door::Door_t::kSOUTH ? center.z : center.x) / ROOM_TILE;

	float cornerCollsionOffset = ROOM_TILE * 0.1; // avoid intersection failures on diagonal corners
	while(offset < max){
		std::vector<Edge *> intersected;

		// Find edges that intersect raycast
		if(_side == PD_Door::Door_t::kNORTH || _side == PD_Door::Door_t::kSOUTH){
			float o = offset * (_side == PD_Door::Door_t::kSOUTH ? -1 : 1);
			// Vertical
			Edge * vRay = new Edge(glm::vec2(center.x + o - cornerCollsionOffset, -1), glm::vec2(center.x + o - cornerCollsionOffset, center.z * 2 + 1), glm::vec2(1, 0));
			
			for(auto e : edges){
				if(e->intersects(vRay, ROOM_TILE)){
					intersected.push_back(e);
				}
			}
			std::sort(intersected.begin(), intersected.end(), [](Edge * i, Edge * j) -> bool{ return ((i->p1.y + i->p2.y) / 2 < (j->p1.y + j->p2.y) / 2);});
		}else{
			float o = offset * (_side == PD_Door::Door_t::kWEST ? -1 : 1);
			// Horizontal
			Edge * hRay = new Edge(glm::vec2(-1, center.z + o - cornerCollsionOffset), glm::vec2(center.x * 2 + 1, center.z + o - cornerCollsionOffset), glm::vec2(0, 1));
		
			for(auto e : edges){
				if(e->intersects(hRay, ROOM_TILE)){
					intersected.push_back(e);
				}
			}
			std::sort(intersected.begin(), intersected.end(), [](Edge * i, Edge * j) -> bool{ return ((i->p1.x + i->p2.x) / 2 < (j->p1.x + j->p2.x) / 2);});
		}
		if(intersected.size() % 2 != 0){
			Log::error("Raycast through room failed");
		}
		if(intersected.size() > 0){
			// Get most outer wall
			if(_side == PD_Door::Door_t::kNORTH || _side == PD_Door::Door_t::kWEST){
				wall = getWallFromEdge(intersected.front());
			}else{
				wall = getWallFromEdge(intersected.back());
			}

			// Try to arrange
			if(wall != nullptr){
				if(arrange(_door, wall, PD_Side::kFRONT, wall->emptySlots.at(PD_Side::kFRONT))){
					addObjectToLists(_door);
					return true;
				}
			}
		}

		intersected.clear();

		// increase offset
		if(offset < 0.5 * ROOM_TILE){
			offset += 0.5 * ROOM_TILE;
		}else{
			offset += 1.f * ROOM_TILE;
		}
	}

	return false;	
}

RoomObject * RoomBuilder::getWallFromEdge(Edge * _e){
	for(int i = 0; i < boundaries.size(); ++i){
		if(edges.at(i) == _e){
			return boundaries.at(i);
		}
	}
	return nullptr;
}

bool RoomBuilder::search(RoomObject * _child){
	// Look for parent
	if(_child->anchor != Anchor_t::CIELING && (_child->parentTypes.size() > 0 || _child->anchor == Anchor_t::WALL)){
		// create parent list
		sweet::ShuffleVector<RoomObject *> validParentsShuffle;
		std::vector<RoomObject *> validParents;
		std::vector<std::string> parentTypes;
		for(auto &d : _child->parentTypes){
			parentTypes.push_back(d.parent);
		}

		for(auto o : availableParents){
			// check if valid parent
			bool validParent = false;
			for(auto &type : parentTypes) {
				if(type == o->type) {
					// just top for now
					if(_child->type != "" && _child->parentMax > 0 && o->emptySlots.find(PD_Side::kTOP) != o->emptySlots.end()){
						int count = 0;
						for(auto c : o->emptySlots.at(PD_Side::kTOP)->children){
							if(c->type == _child->type){
								++count;
							}
						}
						if(count < _child->parentMax){
							validParent = true;
						}
					}else{
						validParent = true;
					}
					break;
				}
			}
			if(validParent){
				validParentsShuffle.push(o);
			}
		}

		validParents = validParentsShuffle.getShuffledItems();

		// prioritize
		std::sort(validParents.begin(), validParents.end(), [parentTypes](RoomObject * _i, RoomObject * _j) -> bool{
			int iPriority = parentTypes.size();
			int jPriority = parentTypes.size();

			for(int i = 0; i < parentTypes.size(); ++i){
				if(parentTypes.at(i) == _i->type){
					iPriority = i;
					break;
				}
			}
			for(int i = 0; i < parentTypes.size(); ++i){
				if(parentTypes.at(i) == _j->type){
					jPriority = i;
					break;
				}
			}

			return (iPriority < jPriority);
		});

		for(auto parent: validParents){

			if(parent->anchor == Anchor_t::CIELING){
				continue;
			}
			// check if valid parent
			bool validParent = false;

			PD_ParentDef parentDef;
			for(auto parentType : _child->parentTypes) {
				if(parentType.parent == parent->type) {
					parentDef = parentType;
					validParent = true;
					break;
				}
			}

			if(!validParent) {
				continue;
			}

			typedef std::map<PD_Side, PD_Slot *>::iterator it_type;
			for(it_type iterator = parent->emptySlots.begin(); iterator != parent->emptySlots.end(); iterator++) {
				PD_Side side = iterator->first;
				PD_Slot * slot = iterator->second;

				bool validSide = false;
				// check if valid side
				for(auto parentSide : parentDef.sides) {
					if(parentSide == side) {
						validSide = true;
						break;
					}
				}
				if(!validSide) {
					Log::warn("Not valid side.");
					continue;
				}
				
				Log::info("Side found.");
				// if the object can be placed without collision
				if(arrange(_child, parent, side, slot)){
					addObjectToLists(_child);
					Log::info("Parenting and placing object.");
					return true;
				}
			}
			Log::warn("No sides availalble.");
		}
	}
	Log::warn("NO PARENT found.");
	
	if(_child->anchor != Anchor_t::WALL && !_child->parentDependent){
		float angle = _child->billboarded ? 0.f : sweet::NumberUtils::randomFloat(-180.f, 180.f);
		glm::quat orient = glm::quat(glm::angleAxis(angle, glm::vec3(0.f, 1.f, 0.f)));
		
		// Look for space in room (20 tries)
		for(unsigned int i = 0; i < tiles.size(); ++i){
			Log::info("Tile selected");

			Log::info("Position found.");
			// Validate bounding box is inside room
			if(canPlaceObject(_child, tiles.at(i), orient)){
				room->addComponent(_child);
				addObjectToLists(_child);

				tiles.erase(tiles.begin() + i);
				return true;
			}
		}
	}
	return false;
}

bool RoomBuilder::arrange(RoomObject * _child, RoomObject * _parent, PD_Side _side, PD_Slot * _slot){
	std::stringstream s;
	s << "side: " << int(_side);
	Log::info(s.str());

	glm::vec3 pos = _parent->childTransform->getTranslationVector();
	pos.y = 0;
	glm::quat orient = _parent->childTransform->getOrientationQuat();

	float angle = 0;
	glm::vec3 childDimensions = glm::vec3(_child->boundingBox.width, _child->boundingBox.height, _child->boundingBox.depth);
	glm::vec3 childLowerBound = glm::vec3(_child->boundingBox.x, _child->boundingBox.y, _child->boundingBox.z);

	// Rotate child according to childSide
	if(_slot->childSide != PD_Side::kBACK && _slot->childSide != PD_Side::kTOP && _side != PD_Side::kBOTTOM){
		float childAngle = 0.f;
		// orient child side pos
		switch(_slot->childSide){
			case PD_Side::kRIGHT:
				childAngle  = 90.f;
				childLowerBound.x = -(_child->boundingBox.depth + _child->boundingBox.z);
				childLowerBound.z = _child->boundingBox.x;
				break;
			case PD_Side::kFRONT:
				childAngle  = 180.f;
				childLowerBound.x = -(_child->boundingBox.width + _child->boundingBox.x);
				childLowerBound.z = -(_child->boundingBox.depth + _child->boundingBox.z);
				break;
			case PD_Side::kLEFT:
				childAngle  = 270.f;
				childLowerBound.x = _child->boundingBox.z;
				childLowerBound.z = -(_child->boundingBox.width + _child->boundingBox.x);
				break;
		}
		angle += childAngle;
	}
	childDimensions = glm::rotate(childDimensions, angle, glm::vec3(0, 1.f, 0));
	childDimensions.x = abs(childDimensions.x);
	childDimensions.y = abs(childDimensions.y);
	childDimensions.z = abs(childDimensions.z);

	// check length of side
	if(!_slot->overflow && childDimensions.x > _slot->length - _slot->spaceFilled){
		Log::warn("Not enough SPACE along side.");
		return false;
	}
	
	// Orient child to side
	if(_side != PD_Side::kFRONT && _side != PD_Side::kTOP && _side != PD_Side::kBOTTOM){
		float sideAngle = 0.f;
		// orient child side pos
		switch(_side){
			case PD_Side::kRIGHT:
				sideAngle  = 90.f;
				break;
			case PD_Side::kBACK:
				sideAngle  = 180.f;
				break;
			case PD_Side::kLEFT:
				sideAngle  = 270.f;
				break;
		}
		angle += sideAngle;
	}

	_child->rotatePhysical(angle, 0, 1.f, 0);
	_child->realign();
	_child->meshTransform->makeCumulativeModelMatrixDirty();
	// object side position
	glm::vec3 sidePos = glm::vec3();

	glm::vec3 moveChildren;
	// parent side transformations
	bool centered = true;
	if(!centered){
		switch(_side){
			case PD_Side::kFRONT:
				sidePos.x += (-_parent->boundingBox.width + childDimensions.x) / 2.f + _slot->spaceFilled;
				sidePos.z += (_parent->boundingBox.depth + _parent->boundingBox.z) - childLowerBound.z;
				break;
			case PD_Side::kBACK:
				sidePos.x += (_parent->boundingBox.width - childDimensions.x) / 2.f - _slot->spaceFilled;
				sidePos.z += _parent->boundingBox.z - (childDimensions.z + childLowerBound.z);
				break;
			case PD_Side::kLEFT:
				sidePos.x += _parent->boundingBox.x - (childDimensions.z + childLowerBound.z);
				sidePos.z += (-_parent->boundingBox.depth + childDimensions.x) / 2.f + _slot->spaceFilled;
				break;
			case PD_Side::kRIGHT:
				sidePos.x += (_parent->boundingBox.width + _parent->boundingBox.x) - childLowerBound.z;
				sidePos.z += (_parent->boundingBox.depth - childDimensions.x) / 2.f - _slot->spaceFilled;
				break;
			case PD_Side::kTOP:
				sidePos.x += (-_parent->boundingBox.width + childDimensions.x) / 2.f + _slot->spaceFilled;
				sidePos.y += _parent->boundingBox.height;
				sidePos.z += sweet::NumberUtils::randomFloat(-_parent->boundingBox.depth / 2.f + childDimensions.z, _parent->boundingBox.depth / 2.f - childDimensions.z) / 2.f;
				break;	
		}
	}else{
		// Centering
		float centerPos = 0.5 * (_slot->spaceFilled + childDimensions.x) + childLowerBound.x;
		
		switch(_side){
			case PD_Side::kFRONT:
				sidePos.z += (_parent->boundingBox.depth + _parent->boundingBox.z) - childLowerBound.z;
				sidePos.x += centerPos;
				moveChildren.x = -1;
				break;
			case PD_Side::kBACK:
				sidePos.z += _parent->boundingBox.z - (childDimensions.z + childLowerBound.z);
				sidePos.x += centerPos;
				moveChildren.x = 1;
				break;
			case PD_Side::kLEFT:
				sidePos.x += _parent->boundingBox.x - (childDimensions.z + childLowerBound.z);
				sidePos.z += centerPos;
				moveChildren.z = -1;
				break;
			case PD_Side::kRIGHT:
				sidePos.x += (_parent->boundingBox.width + _parent->boundingBox.x) - childLowerBound.z;
				sidePos.z += centerPos;
				moveChildren.z = 1;
				break;
			case PD_Side::kTOP:
				sidePos.y += _parent->boundingBox.height;
				sidePos.x += centerPos;
				sidePos.z += sweet::NumberUtils::randomFloat(-_parent->boundingBox.depth / 2.f + childDimensions.z, _parent->boundingBox.depth / 2.f - childDimensions.z) / 2.f;
				moveChildren.x = -1;
				break;	
		}
		moveChildren *= childDimensions.x * 0.5f;
		moveChildren = glm::rotate(orient, moveChildren);

		for(auto c : _slot->children){
			c->translatePhysical(moveChildren);
			c->moveChildren(moveChildren);
			c->realign();
			c->meshTransform->makeCumulativeModelMatrixDirty();
		}
	}

	// rotate side space translation vector
	sidePos = glm::rotate(orient, sidePos);
	// add side space translation vector to final pos
	pos += sidePos;
	
	// check for collision/inside room
	
	if(!canPlaceObject(_child, pos, orient, _parent)){
		Log::warn("Collided");
		if(centered){
			for(auto c : _slot->children){
				c->translatePhysical(-moveChildren);
				c->moveChildren(-moveChildren);
				c->realign();
				c->meshTransform->makeCumulativeModelMatrixDirty();
			}
		}
		_child->rotatePhysical(-angle, 0, 1.f, 0);
		return false;
	}
	/*
	std::stringstream tex;
	tex << "assets/textures/room/debug/" << _slot->children.size()+1 << ".png";
	Texture * res = new Texture(tex.str(), false, true, true);
	res->load();
	_child->mesh->replaceTextures(res);
	*/

	_slot->children.push_back(_child);

	// adjust remaining slot space
	_slot->spaceFilled += abs(childDimensions.x);

	// so we don't have infinite things parented to an overflowed side
	if(_slot->overflow && _slot->spaceFilled >= _slot->length){
		_slot->overflow = false;
	}

	room->addComponent(_child);

	return true;
}

bool RoomBuilder::oppositeSides(PD_Side _side1, PD_Side _side2){
	switch(_side1){
		case PD_Side::kFRONT:
			return _side2 == PD_Side::kBACK;
		case PD_Side::kBACK:
			return _side2 == PD_Side::kFRONT;
		case PD_Side::kLEFT:
			return _side2 == PD_Side::kRIGHT;
		case PD_Side::kRIGHT:
			return _side2 == PD_Side::kLEFT;
		case PD_Side::kTOP:
			return _side2 == PD_Side::kBOTTOM;
		case PD_Side::kBOTTOM:
			return _side2 == PD_Side::kTOP;
	}

	return false;
}

bool RoomBuilder::canPlaceObject(RoomObject * _obj, glm::vec3 _pos, glm::quat _orientation, RoomObject * _parent){
	glm::mat4 mmPrev = _obj->meshTransform->getCumulativeModelMatrix();
	// Get object (A's)  model matrix
	// For each object B placed in the room: get B's model matrix and transform A's vertices into B's coordinate space
	// Create bounding box from transformed coordinates relative to B, then check for bounding box intersection
	std::string tex = (_obj->mesh->textures.size() > 0 ? _obj->mesh->textures.at(0)->src : "");
	std::vector<glm::vec3> verts = _obj->boundingBox.getVertices();
	
	float angle = glm::angle(_orientation);
	glm::vec3 axis = glm::axis(_orientation);
	_obj->rotatePhysical(angle, axis.x, axis.y, axis.z);
	_obj->translatePhysical(_pos);
	_obj->realign();
	_obj->meshTransform->makeCumulativeModelMatrixDirty();
	glm::mat4 mm = _obj->meshTransform->getCumulativeModelMatrix();
	
	// Check for collision with other objects in room
	for(auto o : placedObjects){
		if(_parent != nullptr && o == _parent){
			continue;
		}

		glm::mat4 oMM = o->meshTransform->getCumulativeModelMatrix();
		 
		// Check if object intersects o
		if(o->boundingBox.intersects(getLocalBoundingBoxVertices(verts, mm, oMM), 0.0001f)){
			std::stringstream s;
			s << "Can't place due to COLLISION with: " << o->type << " address: " << o;
			Log::warn(s.str());
			
			_obj->translatePhysical(-_pos);
			_obj->rotatePhysical(-angle, axis.x, axis.y, axis.z);
			_obj->realign();
			_obj->meshTransform->makeCumulativeModelMatrixDirty();
			return false;
			
		}
	}

	return true;
}

std::vector<glm::vec3> RoomBuilder::getLocalBoundingBoxVertices(std::vector<glm::vec3> _verts, glm::mat4 _mmA, glm::mat4 _mmB){
	glm::mat4 m = glm::inverse(_mmB) * _mmA;

	std::vector<glm::vec3> transformed; 
	for(auto v : _verts){
		transformed.push_back(glm::vec3(m * glm::vec4(v, 1)));
	}

	return transformed;
}

void RoomBuilder::addObjectToLists(RoomObject * _obj){
	placedObjects.push_back(_obj);
	if(canBeParent(_obj)){
		availableParents.push_back(_obj);
	}
}

bool RoomBuilder::canBeParent(RoomObject * _obj){
	if(_obj->emptySlots.size() > 0){
		return true;
	}
	return false;
}


void RoomBuilder::createWalls(){
	std::vector<glm::vec2> verts = sweet::TextureUtils::getMarchingSquaresContour(room->tilemap, thresh, false, true);

	assert(verts.size() != 0);

	// error check to make sure size makes sense
	if(verts.size() % 3 != 0){
		throw;
	}

	// Pair vertices to create edges
	for(unsigned int i = 0; i < verts.size(); i += 3){
		Edge * e = new Edge(verts.at(i), verts.at(i+1), verts.at(i+2));
		edges.push_back(e);
	}

	// Reduce vertices
	for (std::vector<Edge *>::iterator it = edges.begin(); it != edges.end();){
		int idx = it - edges.begin();
		int N = edges.size();
		if(N < 2){
			// exit if has less than 2 edges
			break;
		}

		Edge * e1 = *it;
		
		++it;
		/*
		std::wcout << L"\nEdges" << std::endl;
		std::wcout << L"------" << std::endl;
		for(int blah = 0; blah < edges.size(); ++blah){

			std::wcout << L"edge " << blah << ": (" << edges.at(blah)->p1.x << ", " << edges.at(blah)->p1.y << "), (" << edges.at(blah)->p2.x << ", " << edges.at(blah)->p2.y << ")" << std::endl;
		}*/

		for(std::vector<Edge *>::iterator jt = edges.begin(); jt != edges.end(); ++jt){
			int jdx = jt - edges.begin();
			Edge * e2 = *jt;

			if(e1 == e2){
				continue;
			}
			// If e1's end point is the same as e2's start point (should only happen once)
			glm::vec2 C;
			if( (C = e1->p2) == e2->p1 || (C = e1->p1) == e2->p2 || (C = e1->p1) == e2->p1 || (C = e1->p2) == e2->p2){
	
				Edge AB = (e1->p2 == e2->p1 ? Edge(e1->p1, e2->p2) : e1->p1 == e2->p2 ? Edge(e1->p2, e2->p1) : e1->p1 == e2->p1 ? Edge(e1->p2, e2->p2) : Edge(e1->p1, e2->p1));

				float d1 = glm::distance(AB.p1, C);
				float d2 = glm::distance(AB.p2, C);
				float d = glm::distance(AB.p1, AB.p2);
				if((glm::distance(AB.p1, C) + glm::distance(AB.p2, C) - glm::distance(AB.p1, AB.p2)) < 0.00000001){
					// Combine first edge into second edge
					e2->p1.x = AB.p1.x;
					e2->p1.y = AB.p1.y;
					e2->p2.x = AB.p2.x;
					e2->p2.y = AB.p2.y;
					// Delete first edge from edges and overwrite it
					it = edges.erase(--it);
					break;
				}
			}
		}
	}

	// Create walls from edges
	for(Edge * e : edges){
		addWall(glm::distance(e->p1, e->p2), glm::vec2((e->p1.x+e->p2.x)/2.f, (e->p1.y+e->p2.y)/2.f), e->angle);
	}
	
	// convert the room's mesh (which is made of quads) into a TriMesh so that we can use it as a bullet collider
	TriMesh tm(dynamic_cast<QuadMesh *>(room->mesh), false);
	room->setColliderAsMesh(&tm, false);
	room->createRigidBody(0);
#ifdef RG_DEBUG
	room->meshTransform->setVisible(false);
#endif

	// Get wall texture
	room->mesh->pushTexture2D(getWallTex());
}

void RoomBuilder::addWall(float width, glm::vec2 pos, float angle){
	float posX = pos.x * ROOM_TILE;
	float posZ = pos.y * ROOM_TILE;

	float halfW = width / 2.f * ROOM_TILE;
	float H = ROOM_HEIGHT * ROOM_TILE;

	glm::vec3
		axis(0.f, 1.f, 0.f),
		n(glm::rotate(glm::vec3(0.f, 0.f, 1.f), angle, axis));
	
	// Create wall mesh
	// Positions are unrotated
	// Normals are rotated face-normal
	// U is extended to allow repeating; V is standard
	QuadMesh * wallMesh = new QuadMesh(true);
	wallMesh->pushVert(Vertex(-halfW, H, 0.f));
	wallMesh->pushVert(Vertex(-halfW, 0.f, 0.f));
	wallMesh->pushVert(Vertex(halfW, 0.f, 0.f));
	wallMesh->pushVert(Vertex(halfW, H, 0.f));
	wallMesh->setNormal(0, 0.f, 0.f, 1.f);
	wallMesh->setNormal(1, 0.f, 0.f, 1.f);
	wallMesh->setNormal(2, 0.f, 0.f, 1.f);
	wallMesh->setNormal(3, 0.f, 0.f, 1.f);
	wallMesh->setUV(0, 0.f, 0.f);
	wallMesh->setUV(1, 0.f, 1.f);
	wallMesh->setUV(2, width, 1.f);
	wallMesh->setUV(3, width, 0.f);

	// Create wall object as a static mesh, and then rotate and translate the bullet collider
	RoomObject * wall = new RoomObject(world, wallMesh, baseShader);
	wall->setColliderAsBoundingBox();
	wall->createRigidBody(0);
	wall->rotatePhysical(angle, axis.x, axis.y, axis.z);
	wall->translatePhysical(glm::vec3(posX, 0.f, posZ));
	wall->realign();
	wall->meshTransform->makeCumulativeModelMatrixDirty();
	wall->type = "wall";
	wall->emptySlots[PD_Side::kFRONT] = new PD_Slot(PD_Side::kBACK, width * ROOM_TILE);
	boundaries.push_back(wall);

	// copy verts into temp mesh (the other mesh is used for the RoomObject, so we can't modify it directly)
	QuadMesh tempMesh(false);
	tempMesh.insertVertices(*wallMesh);
	// transform verts
	Transform t;
	t.translate(posX, 0, posZ);
	t.rotate(angle, axis.x, axis.y, axis.z, kOBJECT);
	tempMesh.applyTransformation(&t);
	tempMesh.setNormal(0, n.x, n.y, n.z);
	tempMesh.setNormal(1, n.x, n.y, n.z);
	tempMesh.setNormal(2, n.x, n.y, n.z);
	tempMesh.setNormal(3, n.x, n.y, n.z);

	// Append transformed verts to room mesh
	room->mesh->insertVertices(tempMesh);
}

std::vector<glm::vec3> RoomBuilder::getTiles(){
	std::vector<glm::vec3> tiles;

	float offset = ROOM_TILE * 0.1; // avoid intersection failures on diagonal corners
	for(int x = (int)roomLowerBound.x; x < (int)roomUpperBound.x - 1; ++x){
		for(int z = (int)roomLowerBound.z; z < (int)roomUpperBound.z - 1; ++z){
			glm::vec3 pos = glm::vec3((x + 0.5f) * float(ROOM_TILE), 0.f, (z + 0.5f) * float(ROOM_TILE)); 
			pos.x -= offset;
			pos.z -= offset;

			// Cast a ray/line from outside (-1, 0, 0) of polygon to position, and check for odd number of intersections with room sides
			Edge * ray = new Edge(glm::vec2(-1, pos.z), glm::vec2(pos.x, pos.z), glm::vec2(0, 1.f));
			unsigned int intersections = 0;
			for(auto e : edges){
				if(e->intersects(ray, ROOM_TILE)){
					++intersections;
				}
			}
			// Random position not inside room, try again
			if(intersections % 2 == 0){
				continue;
			}

			tiles.push_back(pos);
		}
	}
	// Shuffle tiles
	int cnt = tiles.size();
	sweet::ShuffleVector<glm::vec3> shuffle;
	for(auto t : tiles){
		shuffle.push(t);
	}
	tiles.clear();
	shuffle.shuffle();

	for(int i = 0; i < cnt; ++i){
		tiles.push_back(shuffle.pop());
	}

	return tiles;
	
}

std::vector<RoomObject *> RoomBuilder::getSpecifiedObjects(){
	room->characters = getCharacters();
	room->items = getItems();

	PD_Listing * listing = PD_Listing::listings.at(definition->scenario);

	for(auto c : room->characters){
		listing->addCharacter(c);
		for(auto c : definition->getCharacters()) {
			for(auto i : c->getItems()) {
				PD_Item * item = new PD_Item(i, world, baseShader);
				listing->addItem(item);
				world->world->removeRigidBody(item->body);
			}
		}
	}
	for(auto i : room->items){
		listing->addItem(i);
	}

	std::vector<RoomObject *> objects;
	
	//objects.insert(objects.begin(), props.begin(), props.end());
	//objects.insert(objects.begin(), furniture.begin(), furniture.end());
	objects.insert(objects.begin(), room->items.begin(), room->items.end());
	objects.insert(objects.begin(), room->characters.begin(), room->characters.end());

	return objects;
}

std::vector<RoomObject *> RoomBuilder::getRandomObjects(){
	std::vector<PD_Character *> characters = getCharacters(true);
	std::vector<PD_Item *> items = getItems(true);
	std::vector<PD_Furniture *> furniture = getFurniture();
	//std::vector<PD_Prop *> props = getProps();
	
	PD_Listing * listing = PD_Listing::listings.at(definition->scenario);

	for(auto c : characters){
		listing->addCharacter(c);
		for(auto c : definition->getCharacters()) {
			for(auto i : c->getItems()) {
				PD_Item * item = new PD_Item(i, world, baseShader);
				listing->addItem(item);
				world->world->removeRigidBody(item->body);
			}
		}
	}for(auto i : items){
		listing->addItem(i);
	}

	std::vector<RoomObject *> objects;

	//objects.insert(objects.begin(), props.begin(), props.end());
	objects.insert(objects.begin(), furniture.begin(), furniture.end());
	objects.insert(objects.begin(), items.begin(), items.end());
	objects.insert(objects.begin(), characters.begin(), characters.end());

	
	room->characters.insert(room->characters.begin(), characters.begin(), characters.end());
	room->items.insert(room->items.begin(), items.begin(), items.end());

	return objects;
}

std::vector<PD_Character *> RoomBuilder::getCharacters(bool _random){
	std::vector<PD_Character*> characters;

	if(!_random){
		std::vector<AssetCharacter *> characterDefinitions = definition->getCharacters();
	
		for(auto def : characterDefinitions){
			auto p = new PD_Character(world, def, MeshFactory::getPlaneMesh(3.f), characterShader, emoteShader);
			p->room = room;
			characters.push_back(p);
		}
	}else{
		auto randPD_Character = PD_Character::createRandomPD_Character(definition->scenario, world, characterShader, emoteShader);
		randPD_Character->room = room;
		characters.push_back(randPD_Character);
	}
	// Random
	//unsigned long int n = sweet::NumberUtils::randomInt(0, 10);
	/*for(unsigned int i = 0; i < 1; ++i){
		characters.push_back(new PD_Character(world, MeshFactory::getPlaneMesh(3.f)));
		
		// stretching square planes for now
		characters.at(i)->childTransform->scale(glm::vec3(1.f, ROOM_TILE * 1.2, 1.f));
		characters.at(i)->childTransform->translate(0.f, characters.at(i)->mesh->calcBoundingBox().height / 2.f, 0.f, true);
	}*/
	
	return characters;
}

std::vector<PD_Furniture *> RoomBuilder::getFurniture(){
	std::vector<PD_Furniture *> furniture;
	
	// Get list of furniture types compatible for this room
	std::vector<PD_FurnitureDefinition *> definitions;
	std::vector<int> count;
	if(definition->roomType == "NO_TYPE"){
		for(auto def : PD_ResourceManager::furnitureDefinitions){
			definitions.push_back(def);
			count.push_back(0);
		}
	}else{
		for(auto def : PD_ResourceManager::furnitureDefinitions){
			for(std::string type : def->roomTypes){
				if(type == room->definition->roomType){
					definitions.push_back(def);
					count.push_back(0);
				}
			}
		}
	}

	// Random
	if(definitions.size() > 0){
		float area = room->tilemap->width * room->tilemap->height;
		unsigned long int n = sweet::NumberUtils::randomInt(area * 0.3f, area * 0.5f);
		for(unsigned int i = 0; i < n; ++i){
			int randIdx = sweet::NumberUtils::randomInt(0, definitions.size()-1);

			auto furn = new PD_Furniture(world, definitions.at(randIdx), baseShader, GROUND);
			furniture.push_back(furn);

			count.at(randIdx) += 1;
			if(definitions.at(randIdx)->max > 0 && count.at(randIdx) >= definitions.at(randIdx)->max){
				definitions.erase(definitions.begin() + randIdx);
				count.erase(count.begin() + randIdx);
				if(definitions.size() == 0){
					break;
				}
			}
		}
	}else{
		int blah = 0;
	}
	
	return furniture;
}

std::vector<PD_Item *> RoomBuilder::getItems(bool _random){
	std::vector<PD_Item *> items;
	if(!_random){
		std::vector<AssetItem *> itemDefinitions = definition->getItems();

		for(auto def : itemDefinitions){
			items.push_back(new PD_Item(def, world, baseShader));
		}



	}

	return items;
}

std::vector<PD_Prop *> RoomBuilder::getProps(){
	std::vector<PD_Prop *> props;
	
	// Look at furniture that is already placed
	std::map<std::string, std::vector<PD_Furniture *>> furnitureTypes;
	for(auto o : placedObjects){
		PD_Furniture * f = dynamic_cast<PD_Furniture *>(o);
		if(f != nullptr){
			furnitureTypes[o->type].push_back(f);
		}
	}
	
	typedef std::map<std::string, std::vector<PD_Furniture *>>::iterator it_type;
	for(it_type it = furnitureTypes.begin(); it != furnitureTypes.end(); it++) {
		std::string type = it->first;
		std::vector<PD_Furniture *> furniture = it->second;
		
		if(PD_ResourceManager::furniturePropDefinitions.at(type).size() > 0){
			// Per number of this type, select a prop def!
			// Assume we are only putting props on TOP side
			for(auto f : furniture){
				if(f->emptySlots.find(PD_Side::kTOP) != f->emptySlots.end()){
					float spaceFilled = 0;
					PD_Slot * slot = f->emptySlots.at(PD_Side::kTOP);

					while(spaceFilled <  slot->length){
						props.push_back(new PD_Prop(world, sweet::NumberUtils::randomItem(PD_ResourceManager::furniturePropDefinitions.at(type)), baseShader, GROUND));
						spaceFilled += props.back()->boundingBox.width;
					}
				}
			}
		}
	}

	// Random
	if(PD_ResourceManager::independentPropDefinitions.size() > 0){
		unsigned long int n = sweet::NumberUtils::randomInt(0, room->tilemap->width * room->tilemap->height * 0.01f);
		for(unsigned int i = 0; i < n; ++i){
			props.push_back(new PD_Prop(world, sweet::NumberUtils::randomItem(PD_ResourceManager::independentPropDefinitions), baseShader, GROUND));
		}
	}
	
	return props;
}

Texture * RoomBuilder::getFloorTex(){
#ifdef RG_DEBUG
	return room->tilemap;
#else
	// grab a random floor texture
	std::stringstream ss;
	ss << "assets/textures/room/floor/" << std::to_string(sweet::NumberUtils::randomInt(1, 5)) << ".png";
	Texture * res = new Texture(ss.str(), false, true, true);
	res->load();
	return res;
#endif
}

Texture * RoomBuilder::getCeilTex(){
#ifdef RG_DEBUG
	return room->tilemap;
#else
	// grab a random floor texture
	std::stringstream ss;
	ss << "assets/textures/room/ceiling/" << std::to_string(sweet::NumberUtils::randomInt(1, 5)) << ".png";
	Texture * res = new Texture(ss.str(), false, true, true);
	res->load();
	return res;
#endif
}

Texture * RoomBuilder::getWallTex(){
#ifdef RG_DEBUG
	return room->tilemap;
#else
	// grab a random floor texture
	std::stringstream ss;
	ss << "assets/textures/room/walls/" << std::to_string(sweet::NumberUtils::randomInt(1, 5)) << ".png";
	Texture * res = new Texture(ss.str(), false, true, true);
	res->load();
	return res;
#endif
}

Texture * RoomBuilder::getDebugTex(){
	std::stringstream s;
	s << "assets/textures/room/debug/" << std::to_string(sweet::NumberUtils::randomInt(1, 12)) << ".png";
	Texture * res = new Texture(s.str(), false, true, true);
	res->load();
	return res;
}

unsigned long int RoomBuilder::getDoorTexIdx(){
	return sweet::NumberUtils::randomInt(1, 5);
}