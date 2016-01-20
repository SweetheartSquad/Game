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

#define RG_DEBUG 0

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
}

RoomBuilder::~RoomBuilder(){
}

Room * RoomBuilder::getRoom(){
	
	std::vector<RoomObject *> objects = getRoomObjects();

	room = new Room(world);


	// create floor/ceiling as static bullet planes
	BulletMeshEntity * bulletFloor = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), baseShader);
	bulletFloor->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletFloor->createRigidBody(0);
	room->childTransform->addChild(bulletFloor);
	bulletFloor->meshTransform->scale(1000,1000,1000);
	bulletFloor->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletFloor->body->translate(btVector3(0, 0, 0));
	bulletFloor->body->setFriction(1);
	bulletFloor->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GREY")->texture);

	BulletMeshEntity * bulletCeil = new BulletMeshEntity(world, MeshFactory::getPlaneMesh(), baseShader);
	bulletCeil->setColliderAsStaticPlane(0, -1, 0, -ROOM_HEIGHT * ROOM_TILE);
	bulletCeil->createRigidBody(0);
	room->childTransform->addChild(bulletCeil);
	bulletCeil->meshTransform->scale(1000,1000,1000);
	bulletCeil->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletCeil->body->translate(btVector3(0, ROOM_HEIGHT * ROOM_TILE, 0));
	bulletCeil->body->setFriction(1);
	bulletCeil->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GREY")->texture);
	
	// convert size enum to actual numbers
	unsigned long int l, w;
	switch (definition->size){
		case AssetRoom::Size_t::kSMALL: l = sweet::NumberUtils::randomInt(4, 6); w = sweet::NumberUtils::randomInt(4, 6); break;
		case AssetRoom::Size_t::kMEDIUM: l = sweet::NumberUtils::randomInt(4, 8); w = sweet::NumberUtils::randomInt(4, 8); break;
		case AssetRoom::Size_t::kLARGE: l = sweet::NumberUtils::randomInt(6, 12); w = sweet::NumberUtils::randomInt(6, 12); break;
		break;
	}

	// Generate tilemap image
	tilemap = new PD_TilemapGenerator(l, w, true);
	unsigned long int pixelIncrement = 158;
	tilemap->configure(sweet::NumberUtils::randomInt(pixelIncrement, 255), pixelIncrement);
	tilemap->load();
	tilemap->saveImageData("tilemap.tga");

	room->tilemapSprite = new Sprite();
	room->tilemapSprite->mesh->pushTexture2D(tilemap);
	room->tilemapSprite->childTransform->scale(tilemap->width * ROOM_TILE, tilemap->height * ROOM_TILE, 1);
	room->tilemapSprite->meshTransform->translate(0.5, 0.5, 0);
	room->tilemapSprite->mesh->setScaleMode(GL_NEAREST);

	room->tilemapSprite->childTransform->rotate(90, 1, 0, 0, kOBJECT);

	thresh = 5;
	createWalls();

	// list of available placed parent objects
	std::vector<RoomObject *> availableParents;
	for(unsigned int i = 0; i < boundaries.size(); ++i){
		// only walls in boundaries should have child slots (not floor, cieling)
		if(boundaries.at(i)->emptySlots.size() > 0){
			availableParents.push_back(boundaries.at(i));
			if(RG_DEBUG){
				std::stringstream s;
				s << "ROOM_GEN_" << i+1;
				boundaries.at(i)->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture(s.str())->texture);
			}
		}
	}
	
	for(unsigned int i = 0; i < objects.size(); ++i){
		if(!search(objects.at(i), availableParents, room)){
			Log::warn("Search failed; room object not placed.");
		}else{
			/*if(RG_DEBUG){
				if(objects.at(i)->parent != nullptr && objects.at(i)->parent->mesh->textures.size() > 0){
					objects.at(i)->mesh->pushTexture2D(objects.at(i)->parent->mesh->textures.at(0));
				}
			}*/
		}
	}
	
	// Get rid of temporary boundary room objects
	for(unsigned long int i = 0; i < boundaries.size(); ++i){ 

		for(unsigned long int j = 0; j < boundaries.at(i)->components.size(); ++j){
			room->addComponent(boundaries.at(i)->components.at(j));
		}
			
		if(RG_DEBUG){
			boundaries.at(i)->components.clear();
			for(unsigned long int v = 0; v < boundaries.at(v)->mesh->vertices.size(); ++v){
				boundaries.at(i)->mesh->vertices.at(v).y *= 2;
			}
			room->addComponent(boundaries.at(i));
		}else{
			delete boundaries.at(i);
		}
	}

	// Center room at origin
	room->translatePhysical(glm::vec3(-(l/2.f) * ROOM_TILE, 0.f, -(w/2.f) * ROOM_TILE), true);

	return room;
}

bool RoomBuilder::search(RoomObject * child, std::vector<RoomObject *> objects, Room * room){
	// Look for parent
	if(child->anchor != Anchor_t::CIELING){
		for(unsigned int i = 0; i < objects.size(); ++i){
			if(objects.at(i)->anchor == Anchor_t::CIELING){
				continue;
			}

			typedef std::map<Side_t, std::vector<Slot *>>::iterator it_type;
			for(it_type iterator = objects.at(i)->emptySlots.begin(); iterator != objects.at(i)->emptySlots.end(); iterator++) {
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
					if(arrange(child, objects.at(i), side, slot)){
					
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
			objects.push_back(child);
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

void RoomBuilder::createWalls(){
	std::vector<glm::vec2> verts = sweet::TextureUtils::getMarchingSquaresContour(tilemap, thresh, false, true);

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

	// Get wall texture
	Texture * wallTexture = PD_ResourceManager::scenario->getTexture("WALLS")->texture; // TODO: replace this with random selection based on tags or something

	// Create walls from edges
	for(Edge * e : edges){
		addWall(glm::distance(e->p1, e->p2), glm::vec2((e->p1.x+e->p2.x)/2.f, (e->p1.y+e->p2.y)/2.f), e->angle);
	}
	
	// convert the room's mesh (which is made of quads) into a TriMesh so that we can use it as a bullet collider
	TriMesh tm(dynamic_cast<QuadMesh *>(room->mesh), false);
	room->setColliderAsMesh(&tm, false);
	room->createRigidBody(0);

	if(wallTexture != nullptr){
		room->mesh->pushTexture2D(wallTexture);
	}
}

void RoomBuilder::addWall(float width, glm::vec2 pos, float angle){
	RoomObject * wall;

	float posX = pos.x * ROOM_TILE;
	float posZ = pos.y * ROOM_TILE;

	float halfW = width / 2.f * ROOM_TILE;
	float H = ROOM_HEIGHT * ROOM_TILE;

	glm::vec3
		axis(0.f, 1.f, 0.f),
		n(glm::rotate(glm::vec3(0.f, 0.f, 1.f), angle, axis));
	
	// Create Wall RoomObject
	QuadMesh * m = new QuadMesh();
	m->pushVert(Vertex(-halfW, H, 0));
	m->pushVert(Vertex(-halfW, 0, 0));
	m->pushVert(Vertex(halfW, 0, 0));
	m->pushVert(Vertex(halfW, H, 0));
	m->setNormal(0, n.x, n.y, n.z);
	m->setNormal(1, n.x, n.y, n.z);
	m->setNormal(2, n.x, n.y, n.z);
	m->setNormal(3, n.x, n.y, n.z);
	m->setUV(0, 0.0, 0.0);
	m->setUV(1, 0.0, 1.0);
	m->setUV(2, width, 1.0);
	m->setUV(3, width, 0.0);

	wall = new RoomObject(world, m);
	wall->setColliderAsBoundingBox();
	wall->createRigidBody(0);
	wall->body->getWorldTransform().setRotation(btQuaternion(btVector3(0.f, 1.f, 0.f), glm::radians(angle)));
	wall->translatePhysical(glm::vec3(posX, 0.f, posZ));
	wall->emptySlots[FRONT] = std::vector<Slot *>(1, new Slot(0.f, width * ROOM_TILE));
	boundaries.push_back(wall);


	QuadMesh * qM = new QuadMesh();
	for(unsigned int i = 0; i < m->vertices.size(); ++i){
		qM->pushVert(m->vertices.at(i));
	}

	MeshEntity mE(qM);
	mE.meshTransform->translate(posX, 0, posZ);
	mE.meshTransform->rotate(angle, axis.x, axis.y, axis.z, kOBJECT);
	mE.freezeTransformation();

	// Append to room's mesh
	for(unsigned int i = 0; i < mE.mesh->vertices.size(); ++i){
		Vertex v = mE.mesh->vertices.at(i);
		room->mesh->pushVert(v);
	}

	//delete qM; //we don't need to delete the mesh itself b/c of the auto-release
}

std::vector<RoomObject *> RoomBuilder::getRoomObjects(){

	std::vector<Person *> characters = getCharacters();
	std::vector<PD_Furniture *> furniture = getFurniture();
	std::vector<Item *> items = getItems();

	std::vector<RoomObject *> objects;
	// calculate size of room, get random # of furniture/items?
	// dining table set 1, tv couch set, bathroom set, ...

	for(unsigned int i = 0; i < characters.size(); ++i){
		objects.push_back(characters.at(i));
		characters.at(i)->setShader(characterShader, true);
	}

	for(unsigned int i = 0; i < furniture.size(); ++i){
		objects.push_back(furniture.at(i));
		furniture.at(i)->setShader(baseShader, true);
	}

	for(unsigned int i = 0; i < items.size(); ++i){
		objects.push_back(items.at(i));
		items.at(i)->setShader(baseShader, true);
	}

	return objects;
}

std::vector<Person *> RoomBuilder::getCharacters(){
	std::vector<AssetCharacter *> characterDefinitions = definition->getCharacters();
	std::vector<Person*> characters;
	
	for(auto def : characterDefinitions){
		characters.push_back(new Person(world, def, MeshFactory::getPlaneMesh(3.f)));
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
		int randIdx = sweet::NumberUtils::randomInt(0, PD_ResourceManager::furnitureDefinitions.size() - 1);
		furniture.push_back(new PD_Furniture(world, PD_ResourceManager::furnitureDefinitions.at(randIdx), WALL));


		/*MeshInterface * mesh = MeshFactory::getCubeMesh(ROOM_TILE * 0.4);//Resource::loadMeshFromObj("assets/meshes/RoomTest/couch.obj").at(0);
		for(int i = 0; i < mesh->vertices.size(); ++i){
			mesh->vertices.at(i).z *= 0.5;
			mesh->vertices.at(i).y += ROOM_TILE * 0.2;
		}
		//mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("")->texture);
		Anchor_t anchor = static_cast<Anchor_t>((int) rand() % 1);

		furniture.push_back(new Furniture(_world, mesh, WALL));*/
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

		items.push_back(new Item(world, mesh));
	}

	return items;
}