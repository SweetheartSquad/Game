#pragma once

#include <PD_Item.h>
#include <MeshFactory.h>
#include <MeshInterface.h>
#include <TextureUtils.h>

PD_Item::PD_Item(std::string _name, std::string _description, BulletWorld * _world, Texture * _texture, Shader * _shader, bool _collectable, bool _pixelPerfectInteraction) :
	BulletMeshEntity(_world, MeshFactory::getPlaneMesh(), _shader),
	collectable(_collectable),
	pixelPerfectInteraction(_pixelPerfectInteraction),
	name(_name),
	description(_description)
{
	mesh->pushTexture2D(_texture);
	mesh->setScaleMode(GL_NEAREST);
}

bool PD_Item::checkPixelPerfectCollision(glm::vec3 _position){
	// convert the world space position to object space by multiplying by the inverse model matrix
	// NOTE: since textures are 2D we discard the z and w after the matrix transformation
	glm::vec2 t(glm::inverse(childTransform->getCumulativeModelMatrix()) * glm::vec4(_position, 1));
	
	// convert to UV coordinates by normalizing within the mesh's bounding box
	// NOTE: if the UVs have been altered from the standard plane setup, this won't work
	const sweet::Box box = mesh->calcBoundingBox();
	t.x -= box.x;
	t.y -= box.y;
	t.x /= box.width;
	t.y /= box.height;

	// convert to texture coordinates by multiplying by the texture width and height
	t.y = 1 - t.y; // flip the V first though
	t.x *= mesh->textures.at(0)->width;
	t.y *= mesh->textures.at(0)->height;
	
	// return true if the pixel at the calculated coordinate is opaque (i.e. the alpha channel is 0xFF)
	return sweet::TextureUtils::getPixel(mesh->textures.at(0), t.x, t.y, 3) == 255;
}

bool PD_Item::interact(glm::vec3 _position){
	// if a pixel perfect interaction is expected and _position doesn't pass the check, return early without triggering an interaction
	if(pixelPerfectInteraction){
		if(!checkPixelPerfectCollision(_position)){
			return false;
		}
	}

	// trigger an interaction event
	return true;
}

void PD_Item::addToWorld(){	
	
	setColliderAsBoundingBox();
	createRigidBody(0);
}