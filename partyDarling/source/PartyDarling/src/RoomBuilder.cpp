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

RoomBuilder::RoomBuilder(AssetRoom * _definition, BulletWorld * _world, Shader * _baseShader, Shader * _characterShader):
	world(_world),
	baseShader(_baseShader),
	characterShader(_characterShader),
	definition(_definition)
{
	
}

RoomBuilder::~RoomBuilder(){
}

Room * RoomBuilder::getRoom(){
	room = new Room(world, baseShader);
	
	// convert size enum to actual numbers
	unsigned long int l, w;
	definition->size = AssetRoom::Size_t::kSMALL;
	switch (definition->size){
		case AssetRoom::Size_t::kSMALL: l = sweet::NumberUtils::randomInt(4, 6); w = sweet::NumberUtils::randomInt(4, 6); break;
		case AssetRoom::Size_t::kMEDIUM: l = sweet::NumberUtils::randomInt(4, 8); w = sweet::NumberUtils::randomInt(4, 8); break;
		case AssetRoom::Size_t::kLARGE: l = sweet::NumberUtils::randomInt(6, 12); w = sweet::NumberUtils::randomInt(6, 12); break;
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
	BulletMeshEntity * bulletFloor = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), baseShader);
	bulletFloor->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletFloor->createRigidBody(0);
	bulletFloor->body->setFriction(1);
	room->childTransform->addChild(bulletFloor);
	bulletFloor->meshTransform->scale(-fullL, fullW, 1.f);
	bulletFloor->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletFloor->body->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(180.f)));
	bulletFloor->setTranslationPhysical(room->getCenter());
	bulletFloor->mesh->setScaleMode(GL_NEAREST);
	bulletFloor->mesh->pushTexture2D(getFloorTex());
#ifndef RG_DEBUG
	// adjust UVs so that the texture tiles in squares
	for(Vertex & v : bulletFloor->mesh->vertices){
		v.u *= l;
		v.v *= w;
	}
#endif

	BulletMeshEntity * bulletCeil = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), baseShader);
	bulletCeil->setColliderAsStaticPlane(0, -1, 0, 0);
	bulletCeil->createRigidBody(0);
	bulletCeil->body->setFriction(1);
	room->childTransform->addChild(bulletCeil);
	bulletCeil->meshTransform->scale(-fullL, fullW, -1.f);
	bulletCeil->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletCeil->body->getWorldTransform().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(180.f)));
	bulletCeil->setTranslationPhysical(room->getCenter() + glm::vec3(0, ROOM_HEIGHT * ROOM_TILE, 0));
	bulletCeil->mesh->setScaleMode(GL_NEAREST);
	bulletCeil->mesh->pushTexture2D(getCeilTex());
#ifndef RG_DEBUG
	// adjust UVs so that the texture tiles in squares
	for(Vertex & v : bulletCeil->mesh->vertices){
		v.u *= l;
		v.v *= w;
	}
#endif

	thresh = 5;
	createWalls();
	sweet::Box boundingBox = room->mesh->calcBoundingBox();
	roomUpperBound = boundingBox.getMaxCoordinate();
	roomLowerBound = boundingBox.getMinCoordinate();
	
	std::vector<RoomObject *> objects = getRoomObjects();

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
	
	for(auto obj : objects){
		Log::info("\nAttempting to place OBJECT");
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
		for(auto component : boundary->components){
			room->addComponent(component);
		}
			
#ifdef RG_DEBUG
			boundary->components.clear();
			room->addComponent(boundary);
#else
			delete boundary;
#endif
	}

	// Center room at origin
	room->translatePhysical(-room->getCenter(), true);

	return room;
}

bool RoomBuilder::search(RoomObject * child){
	// Look for parent
	if(child->anchor != Anchor_t::CIELING && sweet::NumberUtils::randomBool()){
		for(unsigned int i = 0; i < availableParents.size(); ++i){
			if(availableParents.at(i)->anchor == Anchor_t::CIELING){
				continue;
			}
			Log::info("Checking parent.");
			typedef std::map<Side_t, std::vector<Slot *>>::iterator it_type;
			for(it_type iterator = availableParents.at(i)->emptySlots.begin(); iterator != availableParents.at(i)->emptySlots.end(); iterator++) {
				// go through available slots of side
				for(unsigned int j = 0; j < iterator->second.size(); ++j){
					Side_t side = iterator->first;
					Slot * slot = iterator->second.at(j);
					sweet::Box childBox = child->mesh->calcBoundingBox();
					
					// check length of slot
					if(childBox.width > slot->length){
						Log::warn("Not enough SPACE along side.");
						continue;
					}
					Log::info("Side found.");
					// if the object can be placed without collision
					if(arrange(child, availableParents.at(i), side, slot)){
						addObjectToLists(child);
						if(childBox.width < slot->length){
							// adjust remaining slot space
							slot->loc += childBox.width;
							slot->length -= childBox.width;
						}else{
							// remove slot
							iterator->second.erase(iterator->second.begin() + j);
						}
						Log::info("Parenting and placing object.");
						return true;
					}
				}
			}
			Log::warn("No sides availalble.");
		}
	}
	Log::warn("NO PARENT found.");

	// Look for space in room (20 tries)
	for(unsigned int i = 0; i < 20; ++i){
		Log::info("Randomly finding a position.");
		// Find random point within bounding box of room (x, z)
		glm::vec3 pos = glm::vec3(sweet::NumberUtils::randomFloat(roomLowerBound.x, roomUpperBound.x), 0.f, sweet::NumberUtils::randomFloat(roomLowerBound.z, roomUpperBound.z));
		glm::quat orient = glm::quat();

		// TODO: Check that the transformed origin will be inside the room
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
			//child->body->getWorldTransform().setRotation(orient);
			child->translatePhysical(pos, true);
			room->addComponent(child);
			addObjectToLists(child);
			return true;
		}
	}
	return false;
}

bool RoomBuilder::arrange(RoomObject * child, RoomObject * parent, Side_t side, Slot * slot){
	
	// position
	btVector3 bPos = parent->body->getWorldTransform().getOrigin();
	glm::vec3 pos = glm::vec3(bPos.x(), bPos.y(), bPos.z());

	btQuaternion bOrient = parent->body->getWorldTransform().getRotation();
	glm::quat orient = glm::quat(bOrient.w(), bOrient.x(), bOrient.y(), bOrient.z());
	

	sweet::Box p = parent->boundingBox;
	sweet::Box c = child->boundingBox;

	// object side position
	glm::vec3 sidePos = glm::vec3();

	switch(side){
		case FRONT:
			sidePos.z += parent->boundingBox.depth / 2.f + child->boundingBox.depth / 2.f;
			sidePos.x += -parent->boundingBox.width / 2.f + child->boundingBox.width / 2.f + slot->loc;
			break;
		case BACK:
			sidePos.z += -parent->boundingBox.depth / 2.f - child->boundingBox.depth / 2.f;
			sidePos.x += parent->boundingBox.width / 2.f - child->boundingBox.width / 2.f - slot->loc;
			break;
		case LEFT:
			sidePos.x += -parent->boundingBox.width / 2.f - child->boundingBox.width / 2.f;
			sidePos.z += -parent->boundingBox.depth / 2.f + child->boundingBox.depth / 2.f + slot->loc;
			break;
		case RIGHT:
			sidePos.x += parent->boundingBox.width / 2.f + child->boundingBox.width / 2.f;
			sidePos.z += parent->boundingBox.depth / 2.f - child->boundingBox.depth / 2.f - slot->loc;
			break;						
	}
	// rotate side space translation vector
	sidePos = glm::rotate(orient, sidePos);
	// add side space translation vector to final pos
	pos += sidePos;

	// check for collision/inside room
	if(!canPlaceObject(child, pos, orient)){
		return false;
	}

	child->body->getWorldTransform().setRotation(bOrient);
	child->translatePhysical(pos, true);

	parent->addComponent(child);

	return true;
}

bool RoomBuilder::canPlaceObject(RoomObject * _obj, glm::vec3 _pos, glm::quat _orientation){

	// Get object (A's)  model matrix
	// For each object B placed in the room: get B's model matrix and transform A's vertices into B's coordinate space
	// Create bounding box from transformed coordinates relative to B, then check for bounding box intersection
	
	glm::vec3 vMin = _obj->boundingBox.getMinCoordinate();	// left bottom back
	glm::vec3 vMax = _obj->boundingBox.getMaxCoordinate();	// right top front

	// get object's bullet model matrix
	glm::mat4 rot = glm::toMat4(_orientation);
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), _pos);
	glm::mat4 mm = trans *rot;

	// Check for collision with other objects in room
	for(auto o : placedObjects){
		// get o's orientation
		btQuaternion oBOrient = o->body->getWorldTransform().getRotation();
		glm::quat oOrient = glm::quat(oBOrient.w(), oBOrient.x(), oBOrient.y(), oBOrient.z());

		// get o's position
		btVector3 oBPos = o->body->getWorldTransform().getOrigin();
		glm::vec3 oPos = glm::vec3(oBPos.x(), oBPos.y(), oBPos.z());

		// get o's bullet model matrix
		glm::mat4 oRot = glm::toMat4(oOrient);
		glm::mat4 oTrans = glm::translate(glm::mat4(1.0f), oPos);
		glm::mat4 oMM = oTrans *oRot;
		 
		// Check if object intersects o
		if(o->boundingBox.intersects(getLocalBoundingBoxVertices(vMin, vMax, mm, oMM))){
			std::stringstream s;
			s << "Can't place due to COLLISION with: " << o->boundingBox.height;
			Log::warn(s.str());
			return false;
		}	
	}

	return true;
}

std::vector<glm::vec3> RoomBuilder::getLocalBoundingBoxVertices(glm::vec3 _lowerBound, glm::vec3 _upperBound, glm::mat4 _mmA, glm::mat4 _mmB){

	glm::mat4 immA = glm::inverse(_mmA);
	// Transform into a's local coordinate space
	glm::vec4 min = _mmB * immA * glm::vec4(_lowerBound, 1);
	glm::vec4 max = _mmB * immA * glm::vec4(_upperBound, 1);

	std::vector<glm::vec3> vertices; 
	// Only need the extremes
	vertices.push_back(glm::vec3(min.x, min.y, min.z));	// left bottom back
	vertices.push_back(glm::vec3(max.x, max.y, max.z));	// right top front

	return vertices;
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

	edges.clear();

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
	QuadMesh * wallMesh = new QuadMesh();
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
	wall->body->getWorldTransform().setRotation(btQuaternion(btVector3(0.f, 1.f, 0.f), glm::radians(angle)));
	wall->translatePhysical(glm::vec3(posX, 0.f, posZ));
	wall->emptySlots[FRONT] = std::vector<Slot *>(1, new Slot(0.f, width * ROOM_TILE));
	boundaries.push_back(wall);

	// copy verts into temp mesh (the other mesh is used for the RoomObject, so we can't modify it directly)
	QuadMesh tempMesh;
	tempMesh.insertVertices(wallMesh);
	tempMesh.setNormal(0, n.x, n.y, n.z);
	tempMesh.setNormal(1, n.x, n.y, n.z);
	tempMesh.setNormal(2, n.x, n.y, n.z);
	tempMesh.setNormal(3, n.x, n.y, n.z);
	// transform verts
	Transform t;
	t.translate(posX, 0, posZ);
	t.rotate(angle, axis.x, axis.y, axis.z, kOBJECT);
	tempMesh.applyTransformation(&t);

	// Append transformed verts to room mesh
	room->mesh->insertVertices(&tempMesh);
}

std::vector<RoomObject *> RoomBuilder::getRoomObjects(){
	PD_Listing * listing = new PD_Listing(this->definition->scenario);

	std::vector<Person *> characters = getCharacters();
	std::vector<PD_Furniture *> furniture = getFurniture();
	std::vector<PD_Item *> items = getItems();

	for(auto c : characters){
		listing->addCharacter(c);
	}for(auto i : items){
		listing->addItem(i);
	}

	std::vector<RoomObject *> objects;
	
	objects.insert(objects.begin(), characters.begin(), characters.end());
	objects.insert(objects.begin(), furniture.begin(), furniture.end());
	objects.insert(objects.begin(), items.begin(), items.end());

	return objects;
}

std::vector<Person *> RoomBuilder::getCharacters(){
	std::vector<AssetCharacter *> characterDefinitions = definition->getCharacters();
	std::vector<Person*> characters;
	
	for(auto def : characterDefinitions){
		characters.push_back(new Person(world, def, MeshFactory::getPlaneMesh(3.f), characterShader));
	}

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
	unsigned long int n = sweet::NumberUtils::randomInt(0, 20);
	for(unsigned int i = 0; i < 15; ++i){
		//Anchor_t anchor = static_cast<Anchor_t>((int) rand() % 1);
		int randIdx = sweet::NumberUtils::randomInt(0, PD_ResourceManager::furnitureDefinitions.size() - 1);
		furniture.push_back(new PD_Furniture(world, PD_ResourceManager::furnitureDefinitions.at(randIdx), baseShader, GROUND));
	}

	return furniture;
}

std::vector<PD_Item *> RoomBuilder::getItems(){
	std::vector<AssetItem *> itemDefinitions = definition->getItems();
	std::vector<PD_Item *> items;
	
	for(auto def : itemDefinitions){
		items.push_back(new PD_Item(def, world, baseShader));
	}


	// add a door manually
	std::stringstream ss;
	ss << doorTexIdx.pop();
	items.push_back(new PD_Item(dynamic_cast<AssetItem *>(PD_ResourceManager::scenario->getAsset("item","DOOR_" + ss.str())), world, baseShader));

	return items;
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