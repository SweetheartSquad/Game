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

//#define RG_DEBUG

Edge::Edge(glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _normal) :
	p1(_p1),
	p2(_p2),
	angle(glm::degrees(glm::atan(_normal.y, _normal.x)))
{		
}

RoomBuilder::RoomBuilder(AssetRoom * _definition, BulletWorld * _world, Shader * _baseShader, Shader * _characterShader):
	world(_world),
	baseShader(_baseShader),
	characterShader(_characterShader),
	definition(_definition)
{
	for(unsigned long int i = 1; i <= 5; ++i){
		wallTexIdx.push(i);
		ceilTexIdx.push(i);
		floorTexIdx.push(i);
	}
	for(unsigned long int i = 1; i <= 12; ++i){
		debugTexIdx.push(i);
	}
}

RoomBuilder::~RoomBuilder(){
}

Room * RoomBuilder::getRoom(){
	room = new Room(world, baseShader);
	
	// convert size enum to actual numbers
	unsigned long int l, w;
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
		availableParents.push_back(shuffleBoundaries.pop());
	}
	
	for(auto obj : objects){
		if(!search(obj)){
			Log::warn("Search failed; room object not placed.");
		}else{
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
	if(child->anchor != Anchor_t::CIELING){
		for(unsigned int i = 0; i < availableParents.size(); ++i){
			if(availableParents.at(i)->anchor == Anchor_t::CIELING){
				continue;
			}

			typedef std::map<Side_t, std::vector<Slot *>>::iterator it_type;
			for(it_type iterator = availableParents.at(i)->emptySlots.begin(); iterator != availableParents.at(i)->emptySlots.end(); iterator++) {
				// go through available slots of side
				for(unsigned int j = 0; j < iterator->second.size(); ++j){
					Side_t side = iterator->first;
					Slot * slot = iterator->second.at(j);
					sweet::Box childBox = child->mesh->calcBoundingBox();
					
					// check length of slot
					if(childBox.width > slot->length){
						continue;
					}
				
					// if the object can be placed without collision
					if(arrange(child, availableParents.at(i), side, slot)){
						canBeParent(child);
						if(childBox.width < slot->length){
							// adjust remaining slot space
							slot->loc += childBox.width;
							slot->length -= childBox.width;
						}else{
							// remove slot
							iterator->second.erase(iterator->second.begin() + j);
						}
						return true;
					}
				}
			}
		}
	}

	// Look for space in room (20 tries)
	for(unsigned int i = 0; i < 20; ++i){
		// Find random point within w and h
		glm::vec3 pos = glm::vec3();
		// Validate bounding box is inside room
		if(true){
			child->translatePhysical(pos);
			room->addComponent(child);
			canBeParent(child);
			return true;
		}
	}
	return false;
}

bool RoomBuilder::arrange(RoomObject * child, RoomObject * parent, Side_t side, Slot * slot){
	
	// position
	btVector3 bPos = parent->body->getWorldTransform().getOrigin();
	btQuaternion bOrient = parent->body->getWorldTransform().getRotation();
	glm::vec3 n = glm::vec3(parent->mesh->vertices.at(0).nx, parent->mesh->vertices.at(0).ny, parent->mesh->vertices.at(0).nz);

	glm::quat orient = glm::quat(bOrient.w(), bOrient.x(), bOrient.y(), bOrient.z());
	glm::vec3 pos = glm::vec3(bPos.x(), bPos.y(), bPos.z());

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

	// check space around
	if(false){
		return false;
	}

	child->body->getWorldTransform().setRotation(bOrient);
	child->translatePhysical(pos);
	//child->rotatePhysical(glm::degrees(orient.y), 0, 1.f, 0);

	parent->addComponent(child);

	return true;
}

bool RoomBuilder::canPlaceObject(RoomObject * _obj, glm::vec3 _pos, glm::quat _orientation){
	// Collides with walls?


	// Collides with other objects? (not room object parent)

	// Inside walls?
	return true;
}

bool RoomBuilder::canBeParent(RoomObject * _obj, bool _addToList){
	if(_obj->emptySlots.size() > 0){
		if(_addToList){
			// Insert at random index
			int idx = sweet::NumberUtils::randomInt(0, availableParents.size() - 1);
			availableParents.insert(availableParents.begin() + idx, _obj);
		}
		return true;
	}
	return false;
}


void RoomBuilder::createWalls(){
	std::vector<glm::vec2> verts = sweet::TextureUtils::getMarchingSquaresContour(room->tilemap, thresh, false, true);

	std::vector<Edge *> edges;

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
	//std::vector<Item *> items = getItems();

	for(auto c : characters){
		listing->addCharacter(c);
	}/*for(auto i : items){
		listing->addItem(i);
	}*/

	std::vector<RoomObject *> objects;
	// calculate size of room, get random # of furniture/items?
	// dining table set 1, tv couch set, bathroom set, ...
	
	objects.insert(objects.begin(), characters.begin(), characters.end());
	objects.insert(objects.begin(), furniture.begin(), furniture.end());
	//objects.insert(objects.begin(), items.begin(), items.end());

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
	unsigned long int n = sweet::NumberUtils::randomInt(0, 10);
	for(unsigned int i = 0; i < 10; ++i){
		//Anchor_t anchor = static_cast<Anchor_t>((int) rand() % 1);
		int randIdx = sweet::NumberUtils::randomInt(0, PD_ResourceManager::furnitureDefinitions.size() - 1);
		furniture.push_back(new PD_Furniture(world, PD_ResourceManager::furnitureDefinitions.at(randIdx), baseShader, GROUND));
	}

	return furniture;
}

std::vector<Item *> RoomBuilder::getItems(){
	std::vector<Item *> items;

	unsigned long int n = sweet::NumberUtils::randomInt(0, 10);
	for(unsigned long int i = 0; i < n; ++i){
		MeshInterface * mesh = MeshFactory::getPlaneMesh(2.f);
		Json::Value j;
		j["texture"] = "UV-TEST";
		Texture * tex = PD_ResourceManager::scenario->getTexture(j.get("texture", "DEFAULT").asString())->texture;
		tex->load();
		mesh->pushTexture2D(tex);

		items.push_back(new Item(world, mesh, baseShader));
	}

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