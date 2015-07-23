#pragma once

#include <RoomLayout.h>

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

RoomLayout::~RoomLayout(void){
}

Room * RoomLayout::getRoom(std::string _json, BulletWorld * _world, ComponentShaderBase * _shader){
	
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

	Room * room = new Room(_world, _shader, static_cast<RoomLayout_t>(json.get("type", 0).asInt()), glm::vec2(l, w), PD_ResourceManager::scenario->getTexture("UV-TEST-ALT")->texture);
	
	room->objects = objects;
	for(unsigned int i = 0; i < room->objects.size(); ++i){
		room->childTransform->addChild(room->objects.at(i));
		vox::Box box = room->objects.at(i)->mesh->calcBoundingBox();
		room->objects.at(i)->setTranslationPhysical(rand() % l * ROOM_TILE, 0, rand() % w * ROOM_TILE);
		// position object
	}

	return room;
}

std::vector<MeshInterface *> RoomLayout::getWalls(RoomLayout_t type, glm::vec2 size){
	// split up later into walls, floor, and cieling?
	switch(type){
		case kT:
			return getTRoom(size);
			break;
		case kL:
			return getLRoom(size);
			break;
		default:
			return getRectRoom(size);
			break;
	}
}

std::vector<MeshInterface *> RoomLayout::getRectRoom(glm::vec2 size){

	return box(size, glm::vec2(), true, true, true, true);
}

std::vector<MeshInterface *> RoomLayout::getTRoom(glm::vec2 size){
	
	// size.x = # units right from intersection of T (reflected)
	// size.y = # units down from intersection of T

	// actual dimensions
	size.x = size.x * 2 + 1;
	size.y = size.y + 1;

	std::vector<MeshInterface *> boundaries;

	// horizontal block index
	int middle = floor(size.x / 2);

	glm::vec2 s = glm::vec2(1, 1);

	glm::vec2 pos = glm::vec2(-floor(size.x / 2.f), 0);
	
	// across (min 3, odd number only)
	for(unsigned int i = 0; i < size.x; ++i){
		std::vector<MeshInterface *> b;
		if(i == 0){
			// left top of T
			b = box(s, pos, true, true, true, false);
		}else if(i == size.x - 1){
			// right top of T
			b = box(s, pos, true, true, false, true);
		}else if(i == middle){
			// top intersect with vertical part of T
			b = box(s, pos, false, true, false, false);
		}else{
			// top of T
			b = box(s, pos, true, true, false, false);
		}
		boundaries.insert(boundaries.end(), b.begin(), b.end());
		pos.x += 1;
	}

	pos.x = 0;

	// down (min 2, including top part)
	for(unsigned int i = 0; i < size.y - 1; ++i){
		std::vector<MeshInterface *> b;
		pos.y += 1;
		
		if(i == size.y - 2){
			// bottom of T
			b = box(s, pos, true, false, true, true);
		}else{
			// vertical part of T
			b = box(s, pos, false, false, true, true);
		}
		boundaries.insert(boundaries.end(), b.begin(), b.end());
	}

	return boundaries;
}

std::vector<MeshInterface *> RoomLayout::getLRoom(glm::vec2 size){

	// size.x = # units up from intersection of L
	// size/y = # units right from intersection of L

	size.x = size.x + 1;
	size.y = size.y + 1;

	std::vector<MeshInterface *> boundaries;

	glm::vec2 s = glm::vec2(1, 1);
	glm::vec2 pos = glm::vec2();

	// down (min 2, including bottom part)
	for(unsigned int i = 0; i < size.y; ++i){
		std::vector<MeshInterface *> b;
		if(i == 0){
			// top of L
			b = box(s, pos, false, true, true, true);
		}else if(i == size.y - 1){
			// bottom left corner of L
			b = box(s, pos, true, false, true, false);
		}else{
			// vertical part of L
			b = box(s, pos, false, false, true, true);
		}
		boundaries.insert(boundaries.end(), b.begin(), b.end());
		pos.y += 1;
	}

	--pos.y;

	// across (min 2, including vertical part)
	for(unsigned int i = 0; i < size.x - 1; ++i){
		std::vector<MeshInterface *> b;
		pos.x += 1;

		if(i == size.x - 2){
			// bottom right end of L
			b = box(s, pos, true, true, false, true);
		}else{
			// horizontal part of L
			b = box(s, pos, true, true, false, false);
		}

		boundaries.insert(boundaries.end(), b.begin(), b.end());
	}

	return boundaries;
}

std::vector<MeshInterface *> RoomLayout::box(glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top, bool bottom){
	std::vector<MeshInterface *> boundaries;

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
		boundaries.push_back(m);
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
		boundaries.push_back(m);
	}

	if(front){
		//Front
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(halfX + posX, halfY, halfZ + posZ)); // top right - top left
		m->pushVert(Vertex(halfX + posX, -halfY, halfZ + posZ)); // bottom right - bottom left
		m->pushVert(Vertex(-halfX + posX, -halfY, halfZ + posZ)); // bottom left - bottom right
		m->pushVert(Vertex(-halfX + posX, halfY, halfZ + posZ)); // top left - top right
		m->setNormal(0, 0.0, 0.0, -1.0);
		m->setNormal(1, 0.0, 0.0, -1.0);
		m->setNormal(2, 0.0, 0.0, -1.0);
		m->setNormal(3, 0.0, 0.0, -1.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.x, 1.0);
		m->setUV(3, size.x, 0.0);
		boundaries.push_back(m);
	}

	if(back){
		//Back
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(-halfX + posX, halfY, -halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, halfY, -halfZ + posZ));
		m->setNormal(0, 0.0, 0.0, 1.0);
		m->setNormal(1, 0.0, 0.0, 1.0);
		m->setNormal(2, 0.0, 0.0, 1.0);
		m->setNormal(3, 0.0, 0.0, 1.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.x, 1.0);
		m->setUV(3, size.x, 0.0);
		boundaries.push_back(m);
	}

	if(left){
		//Left
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(-halfX + posX, halfY, halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, halfY, -halfZ + posZ));
		m->setNormal(0, 1.0, 0.0, 0.0);
		m->setNormal(1, 1.0, 0.0, 0.0);
		m->setNormal(2, 1.0, 0.0, 0.0);
		m->setNormal(3, 1.0, 0.0, 0.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.y, 1.0);
		m->setUV(3, size.y, 0.0);
		boundaries.push_back(m);
	}

	if(right){
		//Right
		QuadMesh * m = new QuadMesh();
		m->pushVert(Vertex(halfX + posX, halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, halfY, halfZ + posZ));
		m->setNormal(0, -1.0, 0.0, 0.0);
		m->setNormal(1, -1.0, 0.0, 0.0);
		m->setNormal(2, -1.0, 0.0, 0.0);
		m->setNormal(3, -1.0, 0.0, 0.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, 1.0);
		m->setUV(2, size.y, 1.0);
		m->setUV(3, size.y, 0.0);
		boundaries.push_back(m);
	}

	return boundaries;
}

std::vector<RoomObject *> RoomLayout::getRoomObjects(Json::Value json, BulletWorld * _world){

	std::vector<Person *> characters = getCharacters(json["characters"], _world);
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

std::vector<Person *> RoomLayout::getCharacters(Json::Value json, BulletWorld * _world){
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

std::vector<Furniture *> RoomLayout::getFurniture(Json::Value json, BulletWorld * _world){
	std::vector<Furniture *> furniture;
	for(Json::ArrayIndex i = 0; i < furniture.size(); ++i){
		furniture.push_back(readFurniture(json[i], _world));
	}

	// Random
	int n = rand() % 10;
	for(unsigned int i = 0; i < n; ++i){
		MeshInterface * mesh = Resource::loadMeshFromObj("assets/meshes/LOD_2/couch_LOD_2.obj").at(0);
		Anchor_t anchor = static_cast<Anchor_t>((int) rand() % 1);

		furniture.push_back(new Furniture(_world, mesh, anchor));
	}

	return furniture;
}

std::vector<Item *> RoomLayout::getItems(Json::Value _json, BulletWorld * _world){
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

Person * RoomLayout::readCharacter(Json::Value _json, BulletWorld * _world){
	MeshInterface * mesh = MeshFactory::getPlaneMesh(3);
	AssetTexture * tex = new AssetTexture(_json["texture"]);
	mesh->pushTexture2D(tex->texture);
	return new Person(_world, mesh);
}

Furniture * RoomLayout::readFurniture(Json::Value _json, BulletWorld * _world){
	TriMesh * mesh = Resource::loadMeshFromObj(_json.get("resource", "assets/meshes/LOD_2/couch_LOD_2.obj").asString()).at(0);
	Anchor_t anchor = static_cast<Anchor_t>(_json.get("anchor", 0).asInt());
	
	return new Furniture(_world, mesh, anchor);
}

Item * RoomLayout::readItem(Json::Value _json, BulletWorld * _world){
	MeshInterface * mesh = Resource::loadMeshFromObj(_json.get("resource", "assets/meshes/LOD_2/dish_LOD_2.obj").asString()).at(0);
	return new Item(_world, mesh);
}
