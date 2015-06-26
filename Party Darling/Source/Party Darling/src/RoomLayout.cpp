#include <RoomLayout.h>
#include <Room.h>
#include <MeshEntity.h>
#include <math.h>

RoomLayout::~RoomLayout(void){
}

std::vector<MeshInterface *> RoomLayout::getWalls(RoomLayout_t type, glm::vec2 size){
	// split up later into walls, floor, and cieling?
	switch(type){
		case T:
			return getTRoom(size);
			break;
		case L:
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

	float posX = pos.x * ROOM_UNIT;
	float posZ = pos.y * ROOM_UNIT;

	float halfX = size.x / 2.f * ROOM_UNIT;
	float halfY = ROOM_HEIGHT / 2.f;
	float halfZ = size.y / 2.f * ROOM_UNIT;

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

std::vector<RoomObject *> RoomLayout::getRoomObjects(std::vector<Character *> _characters, std::vector<Furniture *> _furniture, std::vector<Item *> _items){
	// calculate size of room, get random # of furniture/items?
	// dining table set 1, tv couch set, bathroom set, ...

	// if type == OTHER, random

	///
}
