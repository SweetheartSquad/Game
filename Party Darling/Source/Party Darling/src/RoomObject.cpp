#include <RoomObject.h>
#include <MeshInterface.h>
#include <Box.h>

Slot::Slot(float _loc, float _length) :
	loc(_loc),
	length(_length)
{
}

RoomObject::RoomObject(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor):
	BulletMeshEntity(_world, _mesh),
	anchor(_anchor),
	boundingBox(mesh->calcBoundingBox()),
	parent(nullptr)
{
	 
}

RoomObject::~RoomObject(void){
}

bool RoomObject::addComponent(RoomObject * obj){
	if(obj->parent == nullptr){
		obj->parent = this;
		components.push_back(obj);
		return true;
	}else{
		return false;
	}
}

bool RoomObject::removeComponent(RoomObject * obj){
	for(int i = 0; i < components.size(); ++i){
		if(obj == components.at(i)){
			components.erase(components.begin() + i);
			return true;
		}
	}
	return false;
}

std::vector<RoomObject *> RoomObject::getAllComponents(){
	std::vector<RoomObject *> _components;

	for(unsigned int i = 0; i < components.size(); ++i){
		_components.push_back(components.at(i));
		if(components.at(i)->components.size() > 0){
			std::vector<RoomObject *> c = components.at(i)->getAllComponents();
			_components.insert(_components.end(), c.begin(), c.end());
		}
	}

	return _components;
}

void RoomObject::setShader(Shader * _shader, bool _default){
	MeshEntity::setShader(_shader, _default);
	for(unsigned int i = 0; i < components.size(); ++i){
		RoomObject * obj = components.at(i);
		obj->setShader(_shader, _default);
	}
}

void RoomObject::translatePhysical(glm::vec3 _v, bool _relative){
	setTranslationPhysical(_v, _relative);
	for(unsigned int i = 0; i < components.size(); ++i){
		components.at(i)->translatePhysical(_v);
	}
}