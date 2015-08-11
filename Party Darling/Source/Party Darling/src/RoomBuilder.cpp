#pragma once

#include <RoomBuilder.h>

#include <Room.h>
#include <MeshFactory.h>
#include <scenario\Asset.h>

#include <math.h>
#include <string.h>

#include <Resource.h>

#include <Character.h>
#include <Furniture.h>
#include <Item.h>
#include <PD_ResourceManager.h>

#include <stb/stb_herringbone_wang_tile.h>
#include <stb/stb_image.h>

Tile::Tile(glm::vec2 _pos, Tile_t _type) :
	pos(_pos),
	type(_type),
	free(false)
{
}

Room * RoomBuilder::getRoom(std::string _json, BulletWorld * _world){
	

	Json::Value json;
	Json::Reader reader;

	if(!reader.parse(_json, json, false))
	{
		// throw exception
	}
	
	std::vector<RoomObject *> objects = getRoomObjects(json, _world); // This

	// Square room
	int l = 15;//;json["size"].get("l", objects.size() > 0 ? std::rand() % (int)sqrt(objects.size()) + sqrt(objects.size()) : DEFAULT_ROOM_LENGTH).asInt();
	int w = 15;//;json["size"].get("w", objects.size() > 0 ? std::rand() % (int)sqrt(objects.size()) + sqrt(objects.size()) : DEFAULT_ROOM_LENGTH).asInt();

	glm::vec2 size = glm::vec2(l, w);

	Room * room = new Room(_world, static_cast<RoomLayout_t>(json.get("type", 0).asInt()), size, PD_ResourceManager::scenario->getTexture("UV-TEST-ALT")->texture);

	// get all available tile positions
	std::vector<Tile *> tiles = getTiles(static_cast<RoomLayout_t>(json.get("type", 0).asInt()), size);
	
	// set map for coordinate access to tiles
	std::map<int, std::map<int, Tile_t>> map;
	for(unsigned int i = 0; i < tiles.size(); ++i){
		map[tiles.at(i)->pos.x][tiles.at(i)->pos.y] = tiles.at(i)->type;
	}

	// list of available placed parent objects
	std::vector<RoomObject *> availableParents;
	for(unsigned int i = 0; i < room->boundaries.size(); ++i){
		// only walls in boundaries should have child slots (not floor, cieling)
		if(room->boundaries.at(i)->emptySlots.size() > 0){
			availableParents.insert(availableParents.begin(), room->boundaries.at(i));
		}
	}
	
	for(unsigned int i = 0; i < objects.size(); ++i){
		// Stop if no tiles left
		if(tiles.size() == 0){
			break;
		}
		if(search(objects.at(i), availableParents)){
			room->addComponent(objects.at(i));
		}else{
			room->addComponent(objects.at(i));
			availableParents.push_back(objects.at(i));
		}
	}

	return room;
}

bool RoomBuilder::search(RoomObject * child, std::vector<RoomObject *> objects){
	for(unsigned int i = 0; i < objects.size(); ++i){
		typedef std::map<Side_t, std::vector<Slot *>>::iterator it_type;
		for(it_type iterator = objects.at(i)->emptySlots.begin(); iterator != objects.at(i)->emptySlots.end(); iterator++) {
			// go through available slots of side
			for(unsigned int j = 0; j < iterator->second.size(); ++j){
				Side_t side = iterator->first;
				Slot * slot = iterator->second.at(j);
				vox::Box childBox = child->mesh->calcBoundingBox();
					
				// check length of slot
				if(childBox.width > slot->length){
					continue;
				}
				
				if(arrange(child, objects.at(i), side, slot)){

					if(childBox.width < slot->length){
						// adjust remaining slot space
						slot->loc += childBox.width;
						slot->length = slot->length - childBox.width;
					}else{
						// remove slot
						iterator->second.erase(iterator->second.begin() + j);
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool RoomBuilder::arrange(RoomObject * child, RoomObject * parent, Side_t side, Slot * slot){
	// check space around

	// position
	glm::vec3 pos = parent->childTransform->getTranslationVector();
	vox::Box p = parent->boundingBox;
	vox::Box c = child->boundingBox;
	switch(side){
		case FRONT:
			pos.z += parent->boundingBox.depth / 2.f + child->boundingBox.depth / 2.f;
			pos.x += -parent->boundingBox.width / 2.f + child->boundingBox.width / 2.f + slot->loc;
			break;
		case BACK:
			pos.z += -parent->boundingBox.depth / 2.f - child->boundingBox.depth / 2.f;
			pos.x += parent->boundingBox.width / 2.f - child->boundingBox.width / 2.f - slot->loc;
			break;
		case LEFT:
			pos.x += -parent->boundingBox.width / 2.f - child->boundingBox.width / 2.f;
			pos.z += -parent->boundingBox.depth / 2.f + child->boundingBox.depth / 2.f + slot->loc;
			break;
		case RIGHT:
			pos.x += parent->boundingBox.width / 2.f + child->boundingBox.width / 2.f;
			pos.z += parent->boundingBox.depth / 2.f - child->boundingBox.depth / 2.f - slot->loc;
			break;
								
	}
	child->translatePhysical(pos);

	return true;
}

std::vector<Tile *> RoomBuilder::getTiles(RoomLayout_t _type, glm::vec2 _size){
	std::vector<Tile *> tiles;
	switch(_type){
		case RoomLayout_t::kRECT:
			{
				glm::vec2 pos = glm::vec2(-_size.x / 2.f + 0.5f, -_size.y / 2.f + 0.5f);
				for(unsigned int x = 0; x < _size.x; ++x){
					bool xBoundary = x == 0 || x == _size.x - 1;
					for(unsigned int z = 0; z < _size.y; ++z){
						bool zBoundary = z == 0 || z == _size.y - 1;

						Tile_t tileType = xBoundary && zBoundary ? kCORNER : xBoundary || zBoundary ? kSIDE : kNONE; 
						tiles.push_back(new Tile(glm::vec2(pos.x * ROOM_TILE, pos.y * ROOM_TILE), tileType));
						++pos.y;
					}
					++pos.x;
				}
			}
			break;
		case RoomLayout_t::kT:
			break;
		case RoomLayout_t::kL:
			break;
	}

	return tiles;
}

std::vector<RoomObject *> RoomBuilder::getBoundaries(BulletWorld * _world, RoomLayout_t type, glm::vec2 size){
	int w, h;
	int n = 3;
	unsigned char * pixels = stbi_load("assets/map.png", &w, &h, &n, 0);

	assert(pixels);
	if(pixels){
		
		int blah = 0;
	}
	
	// vector conaining: vec2 pos1, vec2 pos2, vec2 N (normal)
	std::vector<glm::vec2> backE;
	std::vector<glm::vec2> frontE;
	// keep track of vertical edges in order along y
	std::map<unsigned int, std::vector<glm::vec2>> leftE;
	std::map<unsigned int, std::vector<glm::vec2>> rightE;

	for(unsigned int y = 0; y < h; ++y){
		for(unsigned int x = 0; x < w; ++x){
			int r = pixels[(y*w+x)*n];
			// int g
			// int b

			if(r == 255){
				continue;
			}

			/**
			*	--- B ---
			*	|		 |
			*	L		 R
			*	|		 |
			*	--- F ---
			*/

			// Back Edge
			if(y == 0 || pixels[((y-1)*w+x)*n] == 255){
				backE.push_back(glm::vec2(x,y));
			}
			// Front Edge
			if(y == h-1 || (y < h-1 && pixels[((y+1)*w+x)*n] == 255)){
				frontE.push_back(glm::vec2(x,y));
			}

			int l, rt;

			// Left Edge
			if (x > 0){
				l = pixels[(y*w+(x-1))*n];
			}
			if(x < w){
				rt = pixels[(y*w+(x+1))*n];
			}
			
			if(x == 0 || pixels[(y*w+(x-1))*n] == 255){
				if(leftE.find(x) == leftE.end()){
					leftE[x] = std::vector<glm::vec2>();
				}
				leftE[x].push_back(glm::vec2(x, y));
			}
			// Right Edge
			if(x == w-1 || pixels[(y*w+(x+1))*n] == 255){
				if(rightE.find(x) == rightE.end()){
					rightE[x] = std::vector<glm::vec2>();
				}
				rightE[x].push_back(glm::vec2(x, y));
			}
		}
	}
	
	std::vector<RoomObject *> walls;

	glm::vec2 * startPos = nullptr;
	glm::vec2 * endPos = nullptr;
	// Back walls
	for(unsigned int i = 0; i < backE.size(); ++i){
		glm::vec2 * edge = &backE.at(i);

		// Start wall if: startPos is nullptr
		if(startPos == nullptr){
			startPos = edge;
		}

		// End wall if: last item, last item of row, last adjacent item
		if(i == backE.size() - 1 || backE.at(i+1).y != edge->y || (backE.at(i+1).x - edge->x > 1)){
			endPos = edge;
		}

		if(startPos != nullptr && endPos != nullptr){
			int width = endPos->x - startPos->x + 1;
			walls.push_back(getWall(_world, width, glm::vec2(startPos->x + width / 2.f, startPos->y), BACK));
			startPos = nullptr;
			endPos = nullptr;
		}
	}

	startPos = nullptr;
	endPos = nullptr;
	// Front walls
	for(unsigned int i = 0; i < frontE.size(); ++i){
		glm::vec2 * edge = &frontE.at(i);
		// Start wall if: startPos is nullptr
		if(startPos == nullptr){
			startPos = edge;
		}

		// End wall if: last item, last item of row, last adjacent item
		if(i == frontE.size() - 1 || frontE.at(i+1).y != edge->y || (frontE.at(i+1).x - edge->x > 1)){
			endPos = edge;
		}

		if(startPos != nullptr && endPos != nullptr){
			int width = endPos->x - startPos->x + 1;
			walls.push_back(getWall(_world, width, glm::vec2(startPos->x + width / 2.f, startPos->y + 1), FRONT));
			startPos = nullptr;
			endPos = nullptr;
		}
	}

	typedef std::map<unsigned int, std::vector<glm::vec2>>::iterator it_type;

	startPos = nullptr;
	endPos = nullptr;
	// Left walls
	for(it_type iterator = leftE.begin(); iterator != leftE.end(); iterator++) {
		// go through all columns (second is the value)
		for(unsigned int i = 0; i < iterator->second.size(); ++i){
			glm::vec2 * edge = &iterator->second.at(i);
			// Start wall if: startPos is nullptr
			if(startPos == nullptr){
				startPos = edge;
			}

			// End wall if: last item of column, last adjacent item
			if(i == iterator->second.size() - 1 || (iterator->second.at(i+1).y - edge->y > 1)){
				endPos = edge;
			}

			if(startPos != nullptr && endPos != nullptr){
				int width = endPos->y - startPos->y + 1;
				walls.push_back(getWall(_world, width, glm::vec2(startPos->x, startPos->y + width / 2.f), LEFT));
				startPos = nullptr;
				endPos = nullptr;
			}
		}
	}

	startPos = nullptr;
	endPos = nullptr;
	// Right walls
	for(it_type iterator = rightE.begin(); iterator != rightE.end(); iterator++) {
		// go through all columns (second is the value)
		for(unsigned int i = 0; i < iterator->second.size(); ++i){
			glm::vec2 * edge = &iterator->second.at(i);
			// Start wall if: startPos is nullptr
			if(startPos == nullptr){
				startPos = edge;
			}

			// End wall if: last item of column, last adjacent item
			if(i == iterator->second.size() - 1 || (iterator->second.at(i+1).y - edge->y > 1)){
				endPos = edge;
			}

			if(startPos != nullptr && endPos != nullptr){
				int width = endPos->y - startPos->y + 1;
				walls.push_back(getWall(_world, width, glm::vec2(startPos->x + 1, startPos->y + width / 2.f), RIGHT));
				startPos = nullptr;
				endPos = nullptr;
			}
		}
	}

	return walls;
	/*
	// split up later into walls, floor, and cieling?
	switch(type){
		case kT:
			return getTRoom(_world, size);
			break;
		case kL:
			return getLRoom(_world, size);
			break;
		default:
			return getRectRoom(_world, size);
			break;
	}*/
	
}

RoomObject * RoomBuilder::getWall(BulletWorld * _world, float width, glm::vec2 pos, Side_t side){
	RoomObject * wall;

	float posX = pos.x * ROOM_TILE;
	float posZ = pos.y * ROOM_TILE;

	float halfW = width / 2.f * ROOM_TILE;
	float halfH = (ROOM_HEIGHT * ROOM_TILE) / 2.f;

	float angle = 0.f;
	glm::vec3 axis = glm::vec3(0.f, 1.f, 0.f);

	glm::vec3 n = glm::vec3(0.0, 0.0 , 0.0);
	switch(side){
		case BACK:
			n.z = 1.f;
			break;
		case FRONT:
			n.z = -1.f;
			break;
		case LEFT:
			angle = 90.f;
			n.x = 1.f;
			break;
		case RIGHT:
			angle = 90.f;
			n.x = -1.f;
		default:
			break;
	}

	
	QuadMesh * m = new QuadMesh();
	m->pushVert(Vertex(-halfW, halfH, 0));
	m->pushVert(Vertex(-halfW, -halfH, 0));
	m->pushVert(Vertex(halfW, -halfH, 0));
	m->pushVert(Vertex(halfW, halfH, 0));
	m->setNormal(0, n.x, n.y, n.z);
	m->setNormal(1, n.x, n.y, n.z);
	m->setNormal(2, n.x, n.y, n.z);
	m->setNormal(3, n.x, n.y, n.z);
	m->setUV(0, 0.0, 0.0);
	m->setUV(1, 0.0, 1.0);
	m->setUV(2, width, 1.0);
	m->setUV(3, width, 0.0);
	wall = new RoomObject(_world, m);
	wall->meshTransform->rotate(angle, axis.x, axis.y, axis.z, kOBJECT);
	wall->setColliderAsBoundingBox();
	wall->createRigidBody(0);
	wall->translatePhysical(glm::vec3(posX, 0.f, posZ));
	wall->emptySlots[FRONT] = std::vector<Slot *>(1, new Slot(0.f, width));

	return wall;
}

std::vector<RoomObject *> RoomBuilder::getRectRoom(BulletWorld * _world, glm::vec2 size){

	return box(_world, size, glm::vec2(), true, true, true, true);
}

std::vector<RoomObject *> RoomBuilder::getTRoom(BulletWorld * _world, glm::vec2 size){
	
	// size.x = # units right from intersection of T (reflected)
	// size.y = # units down from intersection of T

	// actual dimensions
	size.x = size.x * 2 + 1;
	size.y = size.y + 1;

	std::vector<RoomObject *> boundaries;

	// horizontal block index
	int middle = floor(size.x / 2);

	glm::vec2 s = glm::vec2(1, 1);

	glm::vec2 pos = glm::vec2(-floor(size.x / 2.f), 0);
	
	// across (min 3, odd number only)
	for(unsigned int i = 0; i < size.x; ++i){
		std::vector<RoomObject *> b;
		if(i == 0){
			// left top of T
			b = box(_world, s, pos, true, true, true, false);
		}else if(i == size.x - 1){
			// right top of T
			b = box(_world, s, pos, true, true, false, true);
		}else if(i == middle){
			// top intersect with vertical part of T
			b = box(_world, s, pos, false, true, false, false);
		}else{
			// top of T
			b = box(_world, s, pos, true, true, false, false);
		}
		boundaries.insert(boundaries.end(), b.begin(), b.end());
		pos.x += 1;
	}

	pos.x = 0;

	// down (min 2, including top part)
	for(unsigned int i = 0; i < size.y - 1; ++i){
		std::vector<RoomObject *> b;
		pos.y += 1;
		
		if(i == size.y - 2){
			// bottom of T
			b = box(_world, s, pos, true, false, true, true);
		}else{
			// vertical part of T
			b = box(_world, s, pos, false, false, true, true);
		}
		boundaries.insert(boundaries.end(), b.begin(), b.end());
	}

	return boundaries;
}

std::vector<RoomObject *> RoomBuilder::getLRoom(BulletWorld * _world, glm::vec2 size){

	// size.x = # units up from intersection of L
	// size/y = # units right from intersection of L

	size.x = size.x + 1;
	size.y = size.y + 1;

	std::vector<RoomObject *> boundaries;

	glm::vec2 s = glm::vec2(1, 1);
	glm::vec2 pos = glm::vec2();

	// down (min 2, including bottom part)
	for(unsigned int i = 0; i < size.y; ++i){
		std::vector<RoomObject *> b;
		if(i == 0){
			// top of L
			b = box(_world, s, pos, false, true, true, true);
		}else if(i == size.y - 1){
			// bottom left corner of L
			b = box(_world, s, pos, true, false, true, false);
		}else{
			// vertical part of L
			b = box(_world, s, pos, false, false, true, true);
		}
		boundaries.insert(boundaries.end(), b.begin(), b.end());
		pos.y += 1;
	}

	--pos.y;

	// across (min 2, including vertical part)
	for(unsigned int i = 0; i < size.x - 1; ++i){
		std::vector<RoomObject *> b;
		pos.x += 1;

		if(i == size.x - 2){
			// bottom right end of L
			b = box(_world, s, pos, true, true, false, true);
		}else{
			// horizontal part of L
			b = box(_world, s, pos, true, true, false, false);
		}

		boundaries.insert(boundaries.end(), b.begin(), b.end());
	}

	return boundaries;
}

std::vector<RoomObject *> RoomBuilder::box(BulletWorld * _world, glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top, bool bottom){
	std::vector<RoomObject *> boundaries;

	float posX = pos.x * ROOM_TILE;
	float posZ = pos.y * ROOM_TILE;

	float halfX = size.x / 2.f * ROOM_TILE;
	float halfY = (ROOM_HEIGHT * ROOM_TILE) / 2.f;
	float halfZ = size.y / 2.f * ROOM_TILE;

	if(top){
		// Top
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(-halfX + posX, halfY, halfZ + posZ)); // top left
		m->pushVert(Vertex(-halfX + posX, halfY, -halfZ + posZ)); // bottom left
		m->pushVert(Vertex(halfX + posX, halfY, -halfZ + posZ)); // bottom right
		m->pushVert(Vertex(halfX + posX, halfY, halfZ + posZ)); // top right
		m->setNormal(0, 0.0, -1.0, 0.0);
		m->setNormal(1, 0.0, -1.0, 0.0);
		m->setNormal(2, 0.0, -1.0, 0.0);
		m->setNormal(3, 0.0, -1.0, 0.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, size.y);
		m->setUV(2, size.x, size.y);
		m->setUV(3, size.x, 0.0);
		boundaries.push_back(new RoomObject(_world, m));
		boundaries.back()->setColliderAsBoundingBox();
		boundaries.back()->createRigidBody(0);
	}


	if(bottom){
		// Bottom
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(-halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, -halfZ + posZ));
		m->setNormal(0, 0.0, 1.0, 0.0);
		m->setNormal(1, 0.0, 1.0, 0.0);
		m->setNormal(2, 0.0, 1.0, 0.0);
		m->setNormal(3, 0.0, 1.0, 0.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, size.y);
		m->setUV(2, size.x, size.y);
		m->setUV(3, size.x, 0.0);
		boundaries.push_back(new RoomObject(_world, m));
		boundaries.back()->setColliderAsBoundingBox();
		boundaries.back()->createRigidBody(0);
	}

	if(front){
		//Front
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(halfX + posX, halfY, 0)); // top right
		m->pushVert(Vertex(halfX + posX, -halfY, 0)); // bottom right
		m->pushVert(Vertex(-halfX + posX, -halfY, 0)); // bottom left
		m->pushVert(Vertex(-halfX + posX, halfY, 0)); // top left
		m->setNormal(0, 0.0, 0.0, -1.0);
		m->setNormal(1, 0.0, 0.0, -1.0);
		m->setNormal(2, 0.0, 0.0, -1.0);
		m->setNormal(3, 0.0, 0.0, -1.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.x, 1.0);
		m->setUV(3, size.x, 0.0);
		boundaries.push_back(new RoomObject(_world, m));
		boundaries.back()->setColliderAsBoundingBox();
		boundaries.back()->createRigidBody(0);
		boundaries.back()->translatePhysical(glm::vec3(0, 0, halfZ + posZ));
		boundaries.back()->emptySlots[BACK] = std::vector<Slot *>(1, new Slot(0.f, 2 * halfX));
	}

	if(back){
		//Back
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(-halfX + posX, halfY, 0));
		m->pushVert(Vertex(-halfX + posX, -halfY, 0));
		m->pushVert(Vertex(halfX + posX, -halfY, 0));
		m->pushVert(Vertex(halfX + posX, halfY, 0));
		m->setNormal(0, 0.0, 0.0, 1.0);
		m->setNormal(1, 0.0, 0.0, 1.0);
		m->setNormal(2, 0.0, 0.0, 1.0);
		m->setNormal(3, 0.0, 0.0, 1.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.x, 1.0);
		m->setUV(3, size.x, 0.0);
		boundaries.push_back(new RoomObject(_world, m));
		boundaries.back()->setColliderAsBoundingBox();
		boundaries.back()->createRigidBody(0);
		boundaries.back()->translatePhysical(glm::vec3(0, 0, -halfZ + posZ));
		boundaries.back()->emptySlots[FRONT] = std::vector<Slot *>(1, new Slot(0.f, 2 * halfX));
	}

	if(left){
		//Left
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(0, halfY, halfZ + posZ));
		m->pushVert(Vertex(0, -halfY, halfZ + posZ));
		m->pushVert(Vertex(0, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(0, halfY, -halfZ + posZ));
		m->setNormal(0, 1.0, 0.0, 0.0);
		m->setNormal(1, 1.0, 0.0, 0.0);
		m->setNormal(2, 1.0, 0.0, 0.0);
		m->setNormal(3, 1.0, 0.0, 0.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.y, 1.0);
		m->setUV(3, size.y, 0.0);
		boundaries.push_back(new RoomObject(_world, m));
		boundaries.back()->setColliderAsBoundingBox();
		boundaries.back()->createRigidBody(0);
		boundaries.back()->translatePhysical(glm::vec3(-halfX + posX, 0, 0));
		int n = back ? size.y - 1 : size.y;
		boundaries.back()->emptySlots[RIGHT] = std::vector<Slot *>(1, new Slot(0.f, 2 * halfZ));
	}

	if(right){
		//Right
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(0, halfY, -halfZ + posZ));
		m->pushVert(Vertex(0, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(0, -halfY, halfZ + posZ));
		m->pushVert(Vertex(0, halfY, halfZ + posZ));
		m->setNormal(0, -1.0, 0.0, 0.0);
		m->setNormal(1, -1.0, 0.0, 0.0);
		m->setNormal(2, -1.0, 0.0, 0.0);
		m->setNormal(3, -1.0, 0.0, 0.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.y, 1.0);
		m->setUV(3, size.y, 0.0);
		boundaries.push_back(new RoomObject(_world, m));
		boundaries.back()->setColliderAsBoundingBox();
		boundaries.back()->createRigidBody(0);
		boundaries.back()->translatePhysical(glm::vec3(halfX + posX, 0, 0));
		int n = front ? size.y - 1 : size.y;
		boundaries.back()->emptySlots[LEFT] = std::vector<Slot *>(1, new Slot(0.f, 2 * halfZ));
	}

	return boundaries;
}

std::vector<RoomObject *> RoomBuilder::getRoomObjects(Json::Value json, BulletWorld * _world){

	std::vector<Person *> characters;// = getCharacters(json["characters"], _world);
	std::vector<Furniture *> furniture = getFurniture(json["furniture"], _world);
	std::vector<Item *> items = getItems(json["items"], _world);

	std::vector<RoomObject *> objects;
	// calculate size of room, get random # of furniture/items?
	// dining table set 1, tv couch set, bathroom set, ...

	for(unsigned int i = 0; i < characters.size(); ++i){
		objects.push_back(characters.at(i));
	}

	for(unsigned int i = 0; i < furniture.size(); ++i){
		objects.push_back(furniture.at(i));
	}

	for(unsigned int i = 0; i < items.size(); ++i){
		objects.push_back(items.at(i));
	}

	return objects;
}

std::vector<Person *> RoomBuilder::getCharacters(Json::Value json, BulletWorld * _world){
	std::vector<Person*> characters;
	
	for(Json::ArrayIndex i = 0; i < characters.size(); ++i){
		characters.push_back(readCharacter(json[i], _world));
	}

	// Random
	int n = rand() % 15;
	for(unsigned int i = 0; i < n; ++i){
		MeshInterface * mesh = MeshFactory::getPlaneMesh(3);
		Json::Value j;
		j["src"] = "indexedColourTest.png";
		AssetTexture * tex = new AssetTexture(j);
		mesh->pushTexture2D(tex->texture);
		characters.push_back(new Person(_world, MeshFactory::getPlaneMesh(3.f)));
		
		// stretching square planes for now
		characters.at(i)->childTransform->scale(glm::vec3(1.f, 2.f, 1.f));
		characters.at(i)->childTransform->translate(0.f, characters.at(i)->mesh->calcBoundingBox().height / 2.f, 0.f, true);
	}
	
	return characters;
}

std::vector<Furniture *> RoomBuilder::getFurniture(Json::Value json, BulletWorld * _world){
	std::vector<Furniture *> furniture;
	for(Json::ArrayIndex i = 0; i < furniture.size(); ++i){
		furniture.push_back(readFurniture(json[i], _world));
	}

	// Random
	int n = rand() % 10;
	for(unsigned int i = 0; i < 6; ++i){
		MeshInterface * mesh = Resource::loadMeshFromObj("assets/meshes/RoomTest/couch.obj").at(0);
		Anchor_t anchor = static_cast<Anchor_t>((int) rand() % 1);

		furniture.push_back(new Furniture(_world, mesh, WALL));
	}

	return furniture;
}

std::vector<Item *> RoomBuilder::getItems(Json::Value _json, BulletWorld * _world){
	std::vector<Item *> items;
	for(Json::ArrayIndex i = 0; i < items.size(); ++i){
		items.push_back(readItem(_json[i], _world));
	}

	int n = rand() % 10;
	for(unsigned int i = 0; i < n; ++i){
		MeshInterface * mesh = MeshFactory::getPlaneMesh(2.f);
		Json::Value j;
		j["src"] = "uvs.png";
		AssetTexture * tex = new AssetTexture(j);
		mesh->pushTexture2D(tex->texture);

		items.push_back(new Item(_world, mesh));
	}

	return items;
}

Person * RoomBuilder::readCharacter(Json::Value _json, BulletWorld * _world){
	MeshInterface * mesh = MeshFactory::getPlaneMesh(3);
	AssetTexture * tex = new AssetTexture(_json["texture"]);
	mesh->pushTexture2D(tex->texture);
	return new Person(_world, mesh);
}

Furniture * RoomBuilder::readFurniture(Json::Value _json, BulletWorld * _world){
	TriMesh * mesh = Resource::loadMeshFromObj(_json.get("resource", "assets/meshes/LOD_2/couch_LOD_2.obj").asString()).at(0);
	Anchor_t anchor = static_cast<Anchor_t>(_json.get("anchor", 0).asInt());
	
	return new Furniture(_world, mesh, anchor);
}

Item * RoomBuilder::readItem(Json::Value _json, BulletWorld * _world){
	MeshInterface * mesh = Resource::loadMeshFromObj(_json.get("resource", "assets/meshes/LOD_2/dish_LOD_2.obj").asString()).at(0);
	return new Item(_world, mesh);
}