#include <RoomLayout.h>
#include <MeshFactory.h>
#include <math.h>

RoomLayout::RoomLayout(void)
{
}

MeshInterface * RoomLayout::getWalls(RoomLayout_t type, glm::vec2 size){

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

QuadMesh * RoomLayout::box(QuadMesh * m, glm::vec2 size, glm::vec2 pos, bool front, bool back, bool left, bool right, bool top, bool bottom){
	
	float halfX = size.x / 2.f;
	float halfY = ROOM_HEIGHT / 2.f;
	float halfZ = size.y / 2.f;

	if(top){
		//Top
		m->pushVert(Vertex(-halfX, halfY, halfZ)); // top left
		m->pushVert(Vertex(-halfX, halfY, -halfZ)); // bottom left
		m->pushVert(Vertex(halfX, halfY, -halfZ)); // bottom right
		m->pushVert(Vertex(halfX, halfY, halfZ)); // top right
		m->setNormal(0, 0.0, 1.0, 0.0);
		m->setNormal(1, 0.0, 1.0, 0.0);
		m->setNormal(2, 0.0, 1.0, 0.0);
		m->setNormal(3, 0.0, 1.0, 0.0);
		m->setUV(0, 0.0, 0.0);
		m->setUV(1, 0.0, size.y);
		m->setUV(2, size.x, size.y);
		m->setUV(3, size.x, 0.0);
	}

	if(bottom){
		//Bottom
		m->pushVert(Vertex(-halfX, -halfY, halfZ));
		m->pushVert(Vertex(halfX, -halfY, halfZ));
		m->pushVert(Vertex(halfX, -halfY, -halfZ));
		m->pushVert(Vertex(-halfX, -halfY, -halfZ));
		m->setNormal(4, 0.0, -1.0, 0.0);
		m->setNormal(5, 0.0, -1.0, 0.0);
		m->setNormal(6, 0.0, -1.0, 0.0);
		m->setNormal(7, 0.0, -1.0, 0.0);
		m->setUV(4, 0.0, 0.0);
		m->setUV(5, 0.0, size.y);
		m->setUV(6, size.x, size.y);
		m->setUV(7, size.x, 0.0);
	}

	if(front){
		//Front
		m->pushVert(Vertex(-halfX, halfY, halfZ));
		m->pushVert(Vertex(halfX, halfY, halfZ));
		m->pushVert(Vertex(halfX, -halfY, halfZ));
		m->pushVert(Vertex(-halfX, -halfY, halfZ));
		m->setNormal(8, 0.0, 0.0, 1.0);
		m->setNormal(9, 0.0, 0.0, 1.0);
		m->setNormal(10, 0.0, 0.0, 1.0);
		m->setNormal(11, 0.0, 0.0, 1.0);
		m->setUV(8, 0.0, 0.0);
		m->setUV(9, 0.0, ROOM_HEIGHT);
		m->setUV(10, size.x, ROOM_HEIGHT);
		m->setUV(11, size.x, 0.0);
	}

	if(back){
		//Back
		m->pushVert(Vertex(halfX, halfY, -halfZ));
		m->pushVert(Vertex(-halfX, halfY, -halfZ));
		m->pushVert(Vertex(-halfX, -halfY, -halfZ));
		m->pushVert(Vertex(halfX, -halfY, -halfZ));
		m->setNormal(12, 0.0, 0.0, -1.0);
		m->setNormal(13, 0.0, 0.0, -1.0);
		m->setNormal(14, 0.0, 0.0, -1.0);
		m->setNormal(15, 0.0, 0.0, -1.0);
		m->setUV(12, 0.0, 0.0);
		m->setUV(13, 0.0, ROOM_HEIGHT);
		m->setUV(14, size.x, ROOM_HEIGHT);
		m->setUV(15, size.x, 0.0);
	}

	if(left){
		//Left
		m->pushVert(Vertex(-halfX, halfY, -halfZ));
		m->pushVert(Vertex(-halfX, halfY, halfZ));
		m->pushVert(Vertex(-halfX, -halfY, halfZ));
		m->pushVert(Vertex(-halfX, -halfY, -halfZ));
		m->setNormal(16, -1.0, 0.0, 0.0);
		m->setNormal(17, -1.0, 0.0, 0.0);
		m->setNormal(18, -1.0, 0.0, 0.0);
		m->setNormal(19, -1.0, 0.0, 0.0);
		m->setUV(16, 0.0, 0.0);
		m->setUV(17, 0.0, ROOM_HEIGHT);
		m->setUV(18, size.y, ROOM_HEIGHT);
		m->setUV(19, size.y, 0.0);
	}

	if(right){
		//Right
		m->pushVert(Vertex(halfX, halfY, halfZ));
		m->pushVert(Vertex(halfX, halfY, -halfZ));
		m->pushVert(Vertex(halfX, -halfY, -halfZ));
		m->pushVert(Vertex(halfX, -halfY, halfZ));
		m->setNormal(20, 1.0, 0.0, 0.0);
		m->setNormal(21, 1.0, 0.0, 0.0);
		m->setNormal(22, 1.0, 0.0, 0.0);
		m->setNormal(23, 1.0, 0.0, 0.0);
		m->setUV(20, 0.0, 0.0);
		m->setUV(21, 0.0, ROOM_HEIGHT);
		m->setUV(22, size.y, ROOM_HEIGHT);
		m->setUV(23, size.y, 0.0);
	}

	return m;
}

MeshInterface * RoomLayout::getRectRoom(glm::vec2 size){
	size.x = size.x * ROOM_UNIT;
	size.y = size.y * ROOM_UNIT;

	QuadMesh * m = new QuadMesh();
	m = box(m, size, glm::vec2(), true, true, true, true);

	return m;
}

MeshInterface * RoomLayout::getTRoom(glm::vec2 size){
	//3 blocks across, 4 blocks down?
	QuadMesh * m = new QuadMesh();

	glm::vec2 pos = glm::vec2();
	
	int middle = floor(size.x / 2);

	// across (min 3)
	for(unsigned int i = 0; i < size.x; ++i){
		if(i == 0){
			// left top of T
			m = box(m, size, pos, true, true, true, false);
		}else if(i == size.x - 1){
			// right top of T
			m = box(m, size, pos, true, true, false, true);
		}else if(i == middle){
			// top intersect with vertical part of T
			m = box(m, size, pos, false, true, false, false);
		}else{
			// top of T
			m = box(m, size, pos, true, true, false, false);
		}

		pos.x += ROOM_UNIT;
	}

	pos.x = (middle - 1) * ROOM_UNIT;

	// down (min 2, including top part)
	for(unsigned int i = 0; i < size.y - 1; ++i){
		pos.y += ROOM_UNIT;
		
		if(i == size.y - 2){
			// bottom of T
			m = box(m, size, pos, true, false, true, true);
		}else{
			// vertical part of T
			m = box(m, size, pos, false, false, true, true);
		}
	}

	return m;
}

MeshInterface * RoomLayout::getLRoom(glm::vec2 size){
	
	QuadMesh * m = new QuadMesh();

	glm::vec2 pos = glm::vec2();

	// down (min 2, including bottom part)
	for(unsigned int i = 0; i < size.y; ++i){
		
		if(i == 0){
			// top of L
			m = box(m, size, pos, false, true, true, true);
		}else if(i == size.y - 1){
			// bottom left corner of L
			m = box(m, size, pos, true, false, true, false);
		}else{
			// vertical part of L
			m = box(m, size, pos, false, false, true, true);
		}

		pos.y += ROOM_UNIT;
	}

	// across (min 2, including vertical part)
	for(unsigned int i = 0; i < size.x - 1; ++i){
		
		if(i == size.x - 2){
			// bottom right end of L
			m = box(m, size, pos, true, true, false, true);
		}else{
			// horizontal part of L
			m = box(m, size, pos, true, true, false, false);
		}

		pos.x += ROOM_UNIT;
	}

	return m;
}

RoomLayout::~RoomLayout(void){
}
