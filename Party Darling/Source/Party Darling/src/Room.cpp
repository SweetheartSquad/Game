#include <Room.h>
#include <math.h>
#include <shader/ComponentShaderBase.h>

Room::Room(BulletWorld * _world, ComponentShaderBase * _shader, RoomLayout_t _type, glm::vec2 _size, Texture * _wallTexture)
{
	// Create room boundaries (walls, floor, ceiling)
	std::vector<MeshInterface *> m = RoomLayout::getWalls(_type, _size);

	for(unsigned int i = 0; i < m.size(); ++i){
		boundaries.push_back(new BulletMeshEntity(_world, m.at(i)));
		boundaries.at(i)->setColliderAsBoundingBox();
		boundaries.at(i)->createRigidBody(25);
		boundaries.at(i)->setShader(_shader, true);
		boundaries.at(i)->mesh->pushTexture2D(_wallTexture);
	}

}

Room::~Room(void){
}
