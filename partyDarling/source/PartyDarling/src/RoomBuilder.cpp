#pragma once

#include <RoomBuilder.h>

#include <Room.h>
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

	if ((_point.x >= minX * _scale && _point.x <= maxX * _scale) && (_point.y >= minY * _scale && _point.y <= maxY * _scale))
	{
		return true;
	}
	return false;
}

sweet::ShuffleVector<unsigned long int> RoomBuilder::debugTexIdx;
sweet::ShuffleVector<unsigned long int> RoomBuilder::wallTexIdx;
sweet::ShuffleVector<unsigned long int> RoomBuilder::ceilTexIdx;
sweet::ShuffleVector<unsigned long int> RoomBuilder::floorTexIdx;
sweet::ShuffleVector<unsigned long int> RoomBuilder::doorTexIdx;

bool RoomBuilder::staticInit(){
	for(unsigned long int i = 1; i <= 5; ++i){
		wallTexIdx.push(i);
		ceilTexIdx.push(i);
		floorTexIdx.push(i);
		doorTexIdx.push(i);
	}
	for(unsigned long int i = 1; i <= 12; ++i){
		debugTexIdx.push(i);
	}
	return true;
}
bool RoomBuilder::staticInitialized = RoomBuilder::staticInit();

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
	
	// convert size enum to actual numbers
	unsigned long int l, w;
	switch (definition->size){
		case AssetRoom::Size_t::kSMALL: l = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/4, ROOM_SIZE_MAX/2); w = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/4, ROOM_SIZE_MAX/2); break;
		case AssetRoom::Size_t::kMEDIUM: l = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/3, ROOM_SIZE_MAX/1.5f); w = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/3, ROOM_SIZE_MAX/1.5f); break;
		case AssetRoom::Size_t::kLARGE: l = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/2, ROOM_SIZE_MAX); w = sweet::NumberUtils::randomInt(ROOM_SIZE_MAX/2, ROOM_SIZE_MAX); break;
		break;
	}

	float fullL = l * ROOM_TILE, fullW = ROOM_TILE * w;

	
	// Generate tilemap image
	room->tilemap = new PD_TilemapGenerator(l, w, true);
	unsigned long int pixelIncrement = 158;
	room->tilemap->configure(sweet::NumberUtils::randomInt(pixelIncrement, 255), pixelIncrement);
	room->tilemap->load();
	room->tilemap->saveImageData("tilemap.tga");

	// create floor/ceiling as static bullet planes
	room->floor = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), baseShader);
	room->floor->setColliderAsStaticPlane(0, 1, 0, 0);
	room->floor->createRigidBody(0);
	room->floor->body->setFriction(1);
	room->childTransform->addChild(room->floor);
	room->floor->meshTransform->scale(-fullL, fullW, 1.f);
	room->floor->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	room->floor->body->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(180.f)));
	room->floor->translatePhysical(room->getCenter(), false);
	room->floor->mesh->setScaleMode(GL_NEAREST);
	room->floor->mesh->pushTexture2D(getFloorTex());
#ifndef RG_DEBUG
	// adjust UVs so that the texture tiles in squares
	for(Vertex & v : room->floor->mesh->vertices){
		v.u *= l;
		v.v *= w;
	}
#endif

	room->ceiling = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), baseShader);
	room->ceiling->setColliderAsStaticPlane(0, -1, 0, 0);
	room->ceiling->createRigidBody(0);
	room->ceiling->body->setFriction(1);
	room->childTransform->addChild(room->ceiling);
	room->ceiling->meshTransform->scale(-fullL, fullW, -1.f);
	room->ceiling->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	room->ceiling->body->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(180.f)));
	room->ceiling->translatePhysical(room->getCenter() + glm::vec3(0, ROOM_HEIGHT * ROOM_TILE, 0), false);
	room->ceiling->mesh->setScaleMode(GL_NEAREST);
	room->ceiling->mesh->pushTexture2D(getCeilTex());
#ifndef RG_DEBUG
	// adjust UVs so that the texture tiles in squares
	for(Vertex & v : room->ceiling->mesh->vertices){
		v.u *= l;
		v.v *= w;
	}
#endif

	thresh = 5;
	createWalls();
	sweet::Box boundingBox = room->mesh->calcBoundingBox();
	roomUpperBound = boundingBox.getMaxCoordinate() * (1.f/ROOM_TILE);
	roomLowerBound = boundingBox.getMinCoordinate() * (1.f/ROOM_TILE);
	
	std::vector<RoomObject *> objects = getRoomObjects();
	std::sort(objects.begin(), objects.end(), [](RoomObject * i, RoomObject * j) -> bool{ return (i->parentTypes.size() < j->parentTypes.size());});

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
	placeDoors();

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
		}else{
			Log::info("Search SUCCEEDED.");
#ifdef RG_DEBUG
			if(obj->parent != nullptr && obj->parent->mesh->textures.size() > 0){
				obj->mesh->pushTexture2D(obj->parent->mesh->textures.at(0));
			}
#endif
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

	// Center room at origin
	room->translatePhysical(-room->getCenter(), true);

	availableParents.clear();
	placedObjects.clear();
	edges.clear();

	return room;
}

bool RoomBuilder::placeDoors(){
	// North,South, East, West
	glm::vec3 center = room->getCenter();
	
	Edge * vRay = new Edge(glm::vec2(center.x, -1), glm::vec2(center.x, center.z * 2 + 1), glm::vec2(1, 0));
	Edge * hRay = new Edge(glm::vec2(-1, center.z), glm::vec2(center.x * 2 + 1, center.z), glm::vec2(0, 1));

	std::vector<Edge *> vEdges;
	std::vector<Edge *> hEdges;
	
	// Vertical
	for(auto e : edges){
		if(e->intersects(vRay, ROOM_TILE)){
			vEdges.push_back(e);
		}
	}
	std::sort(vEdges.begin(), vEdges.end(), [](Edge * i, Edge * j) -> bool{ return ((i->p1.y + i->p2.y) / 2 < (j->p1.y + j->p2.y) / 2);});

	// Horizontal
	for(auto e : edges){
		if(e->intersects(hRay, ROOM_TILE)){
			hEdges.push_back(e);
		}
	}
	std::sort(hEdges.begin(), hEdges.end(), [](Edge * i, Edge * j) -> bool{ return ((i->p1.x + i->p2.x) / 2 < (j->p1.x + j->p2.x) / 2);});
	
	// Find Walls
	RoomObject * wallNorth = getWallFromEdge(vEdges.back());
	RoomObject * wallSouth = getWallFromEdge(vEdges.front());
	RoomObject * wallEast = getWallFromEdge(hEdges.back());
	RoomObject * wallWest = getWallFromEdge(hEdges.front());

	RoomObject * doorNorth = getDoor();
	RoomObject * doorSouth = getDoor();
	RoomObject * doorEast = getDoor();
	RoomObject * doorWest = getDoor();

	// Place new door
	if(arrange(doorNorth, wallNorth, PD_Side::kFRONT, wallNorth->emptySlots.at(PD_Side::kFRONT).front())){
		addObjectToLists(doorNorth);
	}else{
		Log::error("North door not placed!!!");
	}
	if(arrange(doorSouth, wallSouth, PD_Side::kFRONT, wallSouth->emptySlots.at(PD_Side::kFRONT).front())){
		addObjectToLists(doorSouth);
	}else{
		Log::error("South door not placed!!!");
	}
	if(arrange(doorEast, wallEast, PD_Side::kFRONT, wallEast->emptySlots.at(PD_Side::kFRONT).front())){
		addObjectToLists(doorEast);
	}else{
		Log::error("East door not placed!!!");
	}
	if(arrange(doorWest, wallWest, PD_Side::kFRONT, wallWest->emptySlots.at(PD_Side::kFRONT).front())){
		addObjectToLists(doorWest);
	}else{
		Log::error("West door not placed!!!");
	}

	room->doors[Door_t::kNORTH] = doorNorth;
	room->doors[Door_t::kSOUTH] = doorSouth;
	room->doors[Door_t::kEAST] = doorEast;
	room->doors[Door_t::kWEST] = doorWest;

	return true;
}

RoomObject * RoomBuilder::getWallFromEdge(Edge * _e){
	for(int i = 0; i < boundaries.size(); ++i){
		if(edges.at(i) == _e){
			return boundaries.at(i);
		}
	}
	return nullptr;
}

bool RoomBuilder::search(RoomObject * child){
	// Look for parent
	if(child->anchor != Anchor_t::CIELING && (child->parentTypes.size() > 0 || child->anchor == Anchor_t::WALL)){
		for(unsigned int i = 0; i < availableParents.size(); ++i){
			RoomObject  * parent = availableParents.at(i);

			if(parent->anchor == Anchor_t::CIELING){
				continue;
			}
			// check if valid parent
			bool validParent = false;

			PD_ParentDef parentDef;
			for(auto parentType : child->parentTypes) {
				if(parentType.parent == parent->type) {
					parentDef = parentType;
					validParent = true;
					break;
				}
			}

			if(!validParent) {
				continue;
			}

			typedef std::map<PD_Side, std::vector<Slot *>>::iterator it_type;
			for(it_type iterator = parent->emptySlots.begin(); iterator != parent->emptySlots.end(); iterator++) {
				// go through available slots of side
				for(unsigned int j = 0; j < iterator->second.size(); ++j){
					PD_Side side = iterator->first;
					Slot * slot = iterator->second.at(j);

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

					// check length of slot
					if(child->boundingBox.width > slot->length){
						Log::warn("Not enough SPACE along side.");
						continue;
					}
					Log::info("Side found.");
					// if the object can be placed without collision
					if(arrange(child, parent, side, slot)){
						if(slot->length <= 0){
							iterator->second.erase(iterator->second.begin() + j);
						}
						addObjectToLists(child);
						Log::info("Parenting and placing object.");
						return true;
					}
				}
			}
			Log::warn("No sides availalble.");
		}
	}
	Log::warn("NO PARENT found.");
	
	if(child->anchor != Anchor_t::WALL){
		child->rotatePhysical(sweet::NumberUtils::randomFloat(-180.f, 180.f), 0, 1.f, 0);
		// Look for space in room (20 tries)
		for(unsigned int i = 0; i < 20; ++i){
			
			// Find random point within bounding box of room (x, z)
			glm::vec3 pos = glm::vec3(sweet::NumberUtils::randomFloat(roomLowerBound.x, roomUpperBound.x) * float(ROOM_TILE), 0.f, sweet::NumberUtils::randomFloat(roomLowerBound.z, roomUpperBound.z) * float(ROOM_TILE));
			std::stringstream s;
			s << "Random position: " << pos.x << ", " << pos.y << ", " << pos.z;
			Log::info(s.str());

			btQuaternion bOrient = child->body->getWorldTransform().getRotation();
			glm::quat orient = glm::quat(bOrient.w(), bOrient.x(), bOrient.y(), bOrient.z());

			// Check that the transformed origin will be inside the room
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

			Log::info("Position found.");
			// Validate bounding box is inside room
			if(canPlaceObject(child, pos, orient)){
				room->addComponent(child);
				addObjectToLists(child);
				return true;
			}
		}
	}
	return false;
}

bool RoomBuilder::arrange(RoomObject * _child, RoomObject * _parent, PD_Side _side, Slot * _slot){
	std::stringstream s;
	s << "side: " << int(_side);
	Log::info(s.str());

	glm::vec3 pos = _parent->childTransform->getTranslationVector();
	pos.y = 0;
	glm::quat orient = _parent->childTransform->getOrientationQuat();

	float angle = 0;
	glm::vec3 childDimensions = glm::vec3(_child->boundingBox.width, _child->boundingBox.height, _child->boundingBox.depth);
	
	// Rotate child according to childSide
	if(_slot->childSide != PD_Side::kBACK && _slot->childSide != PD_Side::kTOP && _side != PD_Side::kBOTTOM){
		float childAngle = 0.f;
		// orient child side pos
		switch(_slot->childSide){
			case PD_Side::kRIGHT:
				childAngle  = 90.f;
				break;
			case PD_Side::kFRONT:
				childAngle  = 180.f;
				break;
			case PD_Side::kLEFT:
				childAngle  = 270.f;
				break;
		}
		angle += childAngle;
	}
	childDimensions = glm::rotate(childDimensions, angle, glm::vec3(0, 1.f, 0));
	/*
	std::stringstream tex;
	tex << "assets/textures/room/debug/" << int(side)+1 << ".png";
	Texture * res = new Texture(tex.str(), false, true, true);
	res->load();
	child->mesh->replaceTextures(res);
	*/
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
	// object side position
	glm::vec3 sidePos = glm::vec3();

	// parent side transformations
	switch(_side){
		case PD_Side::kFRONT:
			sidePos.z += _parent->boundingBox.depth / 2.f + _child->boundingBox.depth / 2.f;
			sidePos.x += -_parent->boundingBox.width / 2.f + _child->boundingBox.width / 2.f + _slot->loc;
			break;
		case PD_Side::kBACK:
			sidePos.z += -_parent->boundingBox.depth / 2.f - _child->boundingBox.depth / 2.f;
			sidePos.x += _parent->boundingBox.width / 2.f - _child->boundingBox.width / 2.f - _slot->loc;
			break;
		case PD_Side::kLEFT:
			sidePos.x += -_parent->boundingBox.width / 2.f - _child->boundingBox.width / 2.f;
			sidePos.z += -_parent->boundingBox.depth / 2.f + _child->boundingBox.depth / 2.f + _slot->loc;
			break;
		case PD_Side::kRIGHT:
			sidePos.x += _parent->boundingBox.width / 2.f + _child->boundingBox.width / 2.f;
			sidePos.z += _parent->boundingBox.depth / 2.f - _child->boundingBox.depth / 2.f - _slot->loc;
			break;
		case PD_Side::kTOP:
			sidePos.y += _parent->boundingBox.height;
			break;	
	}

	// rotate side space translation vector
	sidePos = glm::rotate(orient, sidePos);
	// add side space translation vector to final pos
	pos += sidePos;

	// check for collision/inside room
	if(!canPlaceObject(_child, pos, orient, _parent)){
		Log::warn("Collided");
		_child->rotatePhysical(-angle, 0, 1.f, 0);
		return false;
	}

	// adjust remaining slot space
	_slot->loc += abs(childDimensions.x);
	_slot->length -= abs(childDimensions.x);

	room->addComponent(_child);

	return true;
}

bool RoomBuilder::canPlaceObject(RoomObject * _obj, glm::vec3 _pos, glm::quat _orientation, RoomObject * _parent){

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

	glm::mat4 mm = _obj->meshTransform->getCumulativeModelMatrix();

	// Check for collision with other objects in room
	for(auto o : placedObjects){
		if(_parent != nullptr && o == _parent){
			continue;
		}

		glm::mat4 oMM = o->meshTransform->getCumulativeModelMatrix();
		 
		// Check if object intersects o
		if(o->boundingBox.intersects(getLocalBoundingBoxVertices(verts, mm, oMM), 0.01f)){
			std::stringstream s;
			s << "Can't place due to COLLISION with: " << o->boundingBox.height << " address: " << o;
			Log::warn(s.str());
			_obj->translatePhysical(-_pos);
			_obj->rotatePhysical(-angle, axis.x, axis.y, axis.z);
			_obj->realign();
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
	wall->type = "wall";
	wall->emptySlots[PD_Side::kFRONT] = std::vector<Slot *>(1, new Slot(PD_Side::kBACK, 0.f, width * ROOM_TILE));
	boundaries.push_back(wall);

	// copy verts into temp mesh (the other mesh is used for the RoomObject, so we can't modify it directly)
	QuadMesh tempMesh(false);
	tempMesh.insertVertices(wallMesh);
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
	room->mesh->insertVertices(&tempMesh);
}

std::vector<RoomObject *> RoomBuilder::getRoomObjects(){
	std::vector<Person *> characters = getCharacters();
	std::vector<PD_Furniture *> furniture = getFurniture();
	std::vector<PD_Item *> items = getItems();
	std::vector<PD_Prop *> props = getProps();

	PD_Listing * listing = PD_Listing::listings.at(definition->scenario);

	for(auto c : characters){
		listing->addCharacter(c);
	}for(auto i : items){
		listing->addItem(i);
	}

	std::vector<RoomObject *> objects;
	
	objects.insert(objects.begin(), characters.begin(), characters.end());
	objects.insert(objects.begin(), furniture.begin(), furniture.end());
	objects.insert(objects.begin(), items.begin(), items.end());
	objects.insert(objects.begin(), props.begin(), props.end());

	return objects;
}

std::vector<Person *> RoomBuilder::getCharacters(){
	std::vector<AssetCharacter *> characterDefinitions = definition->getCharacters();
	std::vector<Person*> characters;
	
	for(auto def : characterDefinitions){
		characters.push_back(new Person(world, def, MeshFactory::getPlaneMesh(3.f), characterShader, emoteShader));
	}

	characters.push_back(Person::createRandomPerson(PD_ResourceManager::scenario, world, characterShader, emoteShader));

	// Random
	//unsigned long int n = sweet::NumberUtils::randomInt(0, 10);
	/*for(unsigned int i = 0; i < 1; ++i){
		characters.push_back(new Person(world, MeshFactory::getPlaneMesh(3.f)));
		
		// stretching square planes for now
		characters.at(i)->childTransform->scale(glm::vec3(1.f, ROOM_TILE * 1.2, 1.f));
		characters.at(i)->childTransform->translate(0.f, characters.at(i)->mesh->calcBoundingBox().height / 2.f, 0.f, true);
	}*/
	
	return characters;
}

std::vector<PD_Furniture *> RoomBuilder::getFurniture(){
	std::vector<PD_Furniture *> furniture;
	
	// Random
	unsigned long int n = sweet::NumberUtils::randomInt(0, room->tilemap->width * room->tilemap->height * 0.5f);
	for(unsigned int i = 0; i < n; ++i){
		int randIdx = sweet::NumberUtils::randomInt(0, PD_ResourceManager::furnitureDefinitions.size() - 1);
		furniture.push_back(new PD_Furniture(world, PD_ResourceManager::furnitureDefinitions.at(randIdx), baseShader, GROUND));
	}

	return furniture;
}

RoomObject * RoomBuilder::getDoor(){
	std::stringstream ss;
	ss << doorTexIdx.pop();
	PD_Item * door = new PD_Item(dynamic_cast<AssetItem *>(PD_ResourceManager::scenario->getAsset("item","DOOR_" + ss.str())), world, baseShader, Anchor_t::WALL);
	PD_ParentDef wallDef;
	wallDef.parent = "wall";
	wallDef.sides.push_back(PD_Side::kFRONT);
	door->parentTypes.push_back(wallDef);
	return door;
}

std::vector<PD_Item *> RoomBuilder::getItems(){
	std::vector<AssetItem *> itemDefinitions = definition->getItems();
	std::vector<PD_Item *> items;

	// add a door manually
	std::stringstream ss;
	ss << doorTexIdx.pop();
	PD_Item * door = new PD_Item(dynamic_cast<AssetItem *>(PD_ResourceManager::scenario->getAsset("item","DOOR_" + ss.str())), world, baseShader, Anchor_t::WALL);
	PD_ParentDef wallDef;
	wallDef.parent = "wall";
	wallDef.sides.push_back(PD_Side::kFRONT);
	door->parentTypes.push_back(wallDef);
	room->door = door;
	items.push_back(door);
	

	for(auto def : itemDefinitions){
		items.push_back(new PD_Item(def, world, baseShader));
	}

	return items;
}

std::vector<PD_Prop *> RoomBuilder::getProps(){
	std::vector<PD_Prop *> props;
	
	// Random
	unsigned long int n = sweet::NumberUtils::randomInt(0, room->tilemap->width * room->tilemap->height * 0.5f);
	for(unsigned int i = 0; i < n; ++i){
		//Anchor_t anchor = static_cast<Anchor_t>((int) rand() % 1);
		int randIdx = sweet::NumberUtils::randomInt(0, PD_ResourceManager::propDefinitions.size() - 1);
		props.push_back(new PD_Prop(world, PD_ResourceManager::propDefinitions.at(randIdx), baseShader, GROUND));
	}

	return props;
}

Texture * RoomBuilder::getFloorTex(){
#ifdef RG_DEBUG
	return room->tilemap;
#else
	// grab a random floor texture
	std::stringstream ss;
	ss << "assets/textures/room/floor/" << floorTexIdx.pop() << ".png";
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
	ss << "assets/textures/room/ceiling/" << ceilTexIdx.pop() << ".png";
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
	ss << "assets/textures/room/walls/" << wallTexIdx.pop() << ".png";
	Texture * res = new Texture(ss.str(), false, true, true);
	res->load();
	return res;
#endif
}

Texture * RoomBuilder::getDebugTex(){
	std::stringstream s;
	s << "assets/textures/room/debug/" << debugTexIdx.pop() << ".png";
	Texture * res = new Texture(s.str(), false, true, true);
	res->load();
	return res;
}