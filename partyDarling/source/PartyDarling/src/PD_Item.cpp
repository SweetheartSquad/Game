#pragma once

#include <PD_Item.h>
#include <MeshFactory.h>
#include <MeshInterface.h>
#include <TextureUtils.h>

#include <PD_Assets.h>
#include <PD_ResourceManager.h>

PD_Item::PD_Item(const AssetItem * const _definition, BulletWorld * _world, Shader * _shader, Anchor_t _anchor) :
	RoomObject(_world, MeshFactory::getPlaneMesh(), _shader, _anchor),
	definition(_definition)
{
	billboarded = true;

	AssetTexture * tex = PD_ResourceManager::itemTextures->getTexture(definition->texture);
	tex->load();
	mesh->pushTexture2D(tex->texture);
	mesh->setScaleMode(GL_NEAREST);
	mesh->uvEdgeMode = GL_CLAMP;
	//meshTransform->translate(0, tex->texture->height, 0);
	meshTransform->scale(tex->texture->width, tex->texture->height, 1);
	meshTransform->scale(ITEM_SCALE, ITEM_SCALE, 1);
	freezeTransformation();

	boundingBox = mesh->calcBoundingBox();

	addToWorld();

	translatePhysical(glm::vec3(0, ITEM_POS_Y, 0), false);
	realign();

	originalPos = childTransform->getTranslationVector();

	boundingBox.depth = boundingBox.width*0.1f;
	boundingBox.z = -boundingBox.depth * 0.5;

#ifdef _DEBUG
	for(auto &v :  boundingBoxMesh->mesh->vertices){
		v.x = boundingBox.x + (v.x > 0 ? boundingBox.width : 0);
		v.y = 0 + (v.y > 0 ? boundingBox.height: 0);
		v.z = boundingBox.z + (v.z > 0 ? boundingBox.depth : 0);
	}	
#endif
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

	// account for errors
	t.x = std::min(mesh->textures.at(0)->width-1.f, std::max(0.f, t.x));
	t.y = std::min(mesh->textures.at(0)->height-1.f, std::max(0.f, t.y));
	
	// return true if the pixel at the calculated coordinate is opaque (i.e. the alpha channel is 0xFF)
	return sweet::TextureUtils::getPixel(mesh->textures.at(0), t.x, t.y, 3) == 255;
}

bool PD_Item::actuallyHovered(glm::vec3 _position){
	if(definition->pixelPerfectInteraction){
		return checkPixelPerfectCollision(_position);
	}
	return true;
}

void PD_Item::addToWorld(){	
	setColliderAsBoundingBox();
	createRigidBody(0);
	body->setAngularFactor(btVector3(1,1,1)); // prevent from rotating the physics body at all
}

void PD_Item::triggerPickup(){
	for(auto e : definition->pickupEffects){
		sweet::Event * e2 = new sweet::Event(e);
		e2->setStringData("scenario", definition->scenario->id);
		PD_ResourceManager::scenario->eventManager->triggerEvent(e2);
	}
}
void PD_Item::triggerInteract(){
	if(definition->effects.size() > 0){
		for(auto e : definition->effects){
			sweet::Event * e2 = new sweet::Event(e);
			e2->setStringData("scenario", definition->scenario->id);
			PD_ResourceManager::scenario->eventManager->triggerEvent(e2);
		}
	}else{
		if(definition->name != "Door"){
			PD_ResourceManager::scenario->eventManager->triggerEvent("itemNothingHappened");
		}
	}
}