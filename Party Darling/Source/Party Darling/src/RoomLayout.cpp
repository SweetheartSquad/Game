#include <RoomLayout.h>
#include <MeshFactory.h>
#include <math.h>

RoomLayout::RoomLayout(void)
{
}

MeshInterface * RoomLayout::getWalls(RoomLayout_t type, glm::vec2 size){
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

MeshInterface * RoomLayout::getRectRoom(glm::vec2 size){

	QuadMesh * m = new QuadMesh();
	m = box(m, size, glm::vec2(), true, true, true, true);

	return m;
}

MeshInterface * RoomLayout::getTRoom(glm::vec2 size){
	
	// size.x = # units right from intersection of T (reflected)
	// size.y = # units down from intersection of T

	// actual dimensions
	size.x = size.x * 2 + 1;
	size.y = size.y + 1;

	QuadMesh * m = new QuadMesh();

	// horizontal block index
	int middle = floor(size.x / 2);

	glm::vec2 s = glm::vec2(1, 1);

	glm::vec2 pos = glm::vec2(-floor(size.x / 2.f), 0);
	
	// across (min 3, odd number only)
	for(unsigned int i = 0; i < size.x; ++i){
		if(i == 0){
			// left top of T
			m = box(m, s, pos, true, true, true, false);
		}else if(i == size.x - 1){
			// right top of T
			m = box(m, s, pos, true, true, false, true);
		}else if(i == middle){
			// top intersect with vertical part of T
			m = box(m, s, pos, false, true, false, false);
		}else{
			// top of T
			m = box(m, s, pos, true, true, false, false);
		}

		pos.x += 1;
	}

	pos.x = 0;

	// down (min 2, including top part)
	for(unsigned int i = 0; i < size.y - 1; ++i){
		pos.y += 1;
		
		if(i == size.y - 2){
			// bottom of T
			m = box(m, s, pos, true, false, true, true);
		}else{
			// vertical part of T
			m = box(m, s, pos, false, false, true, true);
		}
	}

	return m;
}

MeshInterface * RoomLayout::getLRoom(glm::vec2 size){

	// size.x = # units up from intersection of L
	// size/y = # units right from intersection of L

	size.x = size.x + 1;
	size.y = size.y + 1;

	QuadMesh * m = new QuadMesh();

	glm::vec2 s = glm::vec2(1, 1);
	glm::vec2 pos = glm::vec2();

	// down (min 2, including bottom part)
	for(unsigned int i = 0; i < size.y; ++i){
		
		if(i == 0){
			// top of L
			m = box(m, s, pos, false, true, true, true);
		}else if(i == size.y - 1){
			// bottom left corner of L
			m = box(m, s, pos, true, false, true, false);
		}else{
			// vertical part of L
			m = box(m, s, pos, false, false, true, true);
		}

		pos.y += 1;
	}

	--pos.y;

	// across (min 2, including vertical part)
	for(unsigned int i = 0; i < size.x - 1; ++i){
		pos.x += 1;

		if(i == size.x - 2){
			// bottom right end of L
			m = box(m, s, pos, true, true, false, true);
		}else{
			// horizontal part of L
			m = box(m, s, pos, true, true, false, false);
		}
	}

	return m;
}

QuadMesh * RoomLayout::box(QuadMesh * m, glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top, bool bottom){
	
	float posX = pos.x * ROOM_UNIT;
	float posZ = pos.y * ROOM_UNIT;

	float halfX = size.x / 2.f * ROOM_UNIT;
	float halfY = ROOM_HEIGHT / 2.f;
	float halfZ = size.y / 2.f * ROOM_UNIT;

	int idx = m->vertices.size() - 1;

	int indices[4];
	if(top){
		// Top
		indices[0] = ++idx;
		indices[1] = ++idx;
		indices[2] = ++idx;
		indices[3] = ++idx;
		
		m->pushVert(Vertex(-halfX + posX, halfY, halfZ + posZ)); // top left
		m->pushVert(Vertex(-halfX + posX, halfY, -halfZ + posZ)); // bottom left
		m->pushVert(Vertex(halfX + posX, halfY, -halfZ + posZ)); // bottom right
		m->pushVert(Vertex(halfX + posX, halfY, halfZ + posZ)); // top right
		m->setNormal(indices[0], 0.0, -1.0, 0.0);
		m->setNormal(indices[1], 0.0, -1.0, 0.0);
		m->setNormal(indices[2], 0.0, -1.0, 0.0);
		m->setNormal(indices[3], 0.0, -1.0, 0.0);
		m->setUV(indices[0], 0.0, 0.0);
		m->setUV(indices[1], 0.0, size.y);
		m->setUV(indices[2], size.x, size.y);
		m->setUV(indices[3], size.x, 0.0);
	}


	if(bottom){
		// Bottom
		indices[0] = ++idx;
		indices[1] = ++idx;
		indices[2] = ++idx;
		indices[3] = ++idx;

		m->pushVert(Vertex(-halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, -halfZ + posZ));
		m->setNormal(indices[0], 0.0, 1.0, 0.0);
		m->setNormal(indices[1], 0.0, 1.0, 0.0);
		m->setNormal(indices[2], 0.0, 1.0, 0.0);
		m->setNormal(indices[3], 0.0, 1.0, 0.0);
		m->setUV(indices[0], 0.0, 0.0);
		m->setUV(indices[1], 0.0, size.y);
		m->setUV(indices[2], size.x, size.y);
		m->setUV(indices[3], size.x, 0.0);
	}

	if(front){
		//Front
		indices[0] = ++idx;
		indices[1] = ++idx;
		indices[2] = ++idx;
		indices[3] = ++idx;
		
		m->pushVert(Vertex(halfX + posX, halfY, halfZ + posZ)); // top right - top left
		m->pushVert(Vertex(halfX + posX, -halfY, halfZ + posZ)); // bottom right - bottom left
		m->pushVert(Vertex(-halfX + posX, -halfY, halfZ + posZ)); // bottom left - bottom right
		m->pushVert(Vertex(-halfX + posX, halfY, halfZ + posZ)); // top left - top right
		m->setNormal(indices[0], 0.0, 0.0, -1.0);
		m->setNormal(indices[1], 0.0, 0.0, -1.0);
		m->setNormal(indices[2], 0.0, 0.0, -1.0);
		m->setNormal(indices[3], 0.0, 0.0, -1.0);
		m->setUV(indices[0], 0.0, 0.0);
		m->setUV(indices[1], 0.0, 1.0);
		m->setUV(indices[2], size.x, 1.0);
		m->setUV(indices[3], size.x, 0.0);
	}

	if(back){
		//Back
		indices[0] = ++idx;
		indices[1] = ++idx;
		indices[2] = ++idx;
		indices[3] = ++idx;

		m->pushVert(Vertex(-halfX + posX, halfY, -halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, halfY, -halfZ + posZ));
		m->setNormal(indices[0], 0.0, 0.0, 1.0);
		m->setNormal(indices[1], 0.0, 0.0, 1.0);
		m->setNormal(indices[2], 0.0, 0.0, 1.0);
		m->setNormal(indices[3], 0.0, 0.0, 1.0);
		m->setUV(indices[0], 0.0, 0.0);
		m->setUV(indices[1], 0.0, 1.0);
		m->setUV(indices[2], size.x, 1.0);
		m->setUV(indices[3], size.x, 0.0);
	}

	if(left){
		//Left
		indices[0] = ++idx;
		indices[1] = ++idx;
		indices[2] = ++idx;
		indices[3] = ++idx;

		m->pushVert(Vertex(-halfX + posX, halfY, halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(-halfX + posX, halfY, -halfZ + posZ));
		m->setNormal(indices[0], 1.0, 0.0, 0.0);
		m->setNormal(indices[1], 1.0, 0.0, 0.0);
		m->setNormal(indices[2], 1.0, 0.0, 0.0);
		m->setNormal(indices[3], 1.0, 0.0, 0.0);
		m->setUV(indices[0], 0.0, 0.0);
		m->setUV(indices[1], 0.0, 1.0);
		m->setUV(indices[2], size.y, 1.0);
		m->setUV(indices[3], size.y, 0.0);
	}

	if(right){
		//Right
		indices[0] = ++idx;
		indices[1] = ++idx;
		indices[2] = ++idx;
		indices[3] = ++idx;

		m->pushVert(Vertex(halfX + posX, halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, -halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, -halfY, halfZ + posZ));
		m->pushVert(Vertex(halfX + posX, halfY, halfZ + posZ));
		m->setNormal(indices[0], -1.0, 0.0, 0.0);
		m->setNormal(indices[1], -1.0, 0.0, 0.0);
		m->setNormal(indices[2], -1.0, 0.0, 0.0);
		m->setNormal(indices[3], -1.0, 0.0, 0.0);
		m->setUV(indices[0], 0.0, 0.0);
		m->setUV(indices[1], 0.0, 1.0);
		m->setUV(indices[2], size.y, 1.0);
		m->setUV(indices[3], size.y, 0.0);
	}

	return m;
}

RoomLayout::~RoomLayout(void){
}
