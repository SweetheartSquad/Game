#pragma once

#include <PD_Furniture.h>
#include <PD_FurnitureParser.h>
#include <PD_FurnitureComponent.h>
#include <PD_FurnitureComponentDefinition.h>
#include <PD_ResourceManager.h>
#include <PD_Slot.h>
#include <PD_Masks.h>

#include <shader/Shader.h>
#include <NumberUtils.h>
#include <Easing.h>
#include <MeshDeformation.h>

PD_Furniture::PD_Furniture(BulletWorld * _bulletWorld, PD_FurnitureDefinition * _def, Shader * _shader, Anchor_t _anchor) :
	RoomObject(_bulletWorld, new TriMesh(true), _shader, _anchor)
{
	// make sure that there's only one root
	assert(_def->components.size() == 1);

	// build the furniture
	PD_BuildResult buildResult = _def->components.at(0)->build();
	lights = buildResult.lights;

	// get a texture for the furniture type
	Texture * tex = sweet::NumberUtils::randomItem(_def->textures);
	tex->load();
	mesh->pushTexture2D(tex);
	mesh->setScaleMode(GL_NEAREST);

	// copy the furniture mesh into this entity's mesh
	mesh->insertVertices(*buildResult.mesh);
	// delete the temporary mesh
	delete buildResult.mesh;

	meshTransform->scale(FURNITURE_SCALE, FURNITURE_SCALE, FURNITURE_SCALE);
	freezeTransformation();

	// Get type
	type = _def->type;
	parentDependent = _def->parentDependent;
	parentMax = _def->parentMax;

	// Get parent types
	parentTypes.insert(parentTypes.begin(), _def->parents.begin(), _def->parents.end());

	sweet::Box originalBoundingBox = mesh->calcBoundingBox();

	//Deformers
	float lowerFlareVal = sweet::NumberUtils::randomFloat(0.f,0.4f);
	float upperFlareVal = sweet::NumberUtils::randomFloat(0.f,(1.f - (0.5f+lowerFlareVal)));
	float lowerBoundVal = sweet::NumberUtils::randomFloat(0.2f,0.3f);

	if(_def->twist){
		MeshDeformation::twist(mesh, -upperFlareVal, upperFlareVal, lowerBoundVal, Easing::kLINEAR);
	}
	//MeshDeformation::bend(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kLINEAR);
	if(_def->flare){
		MeshDeformation::flare(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kLINEAR);
	}

	// Make the mesh dirty since the verts have changed
	// May be redunant but do it as a safe guard
	mesh->dirty = true;

	// we need to inform the RoomObject of the new bounding box here
	boundingBox = mesh->calcBoundingBox();

	for(unsigned long int i = 0; i < buildResult.lightParents.size(); ++i) {
		glm::vec3 w = buildResult.lights.at(i)->childTransform->getWorldPos() * FURNITURE_SCALE;
		Light * l = buildResult.lights.at(i);
		l->firstParent()->removeChild(l);
		delete buildResult.lightParents.at(i);
		meshTransform->addChild(l)->translate(w);
	}

	// create the bullet stuff
	if(_def->detailedCollider){
		shape = buildResult.collider;
	}else{
		delete buildResult.collider;
		setColliderAsBoundingBox();
	}
	createRigidBody(_def->mass * FURNITURE_MASS_SCALE, kENVIRONMENT);

	// position bottom on ground
	realign(); // update the current position from creation
	translatePhysical(glm::vec3(0, childTransform->getTranslationVector().y - boundingBox.y, 0.f), false); // false false false
	realign();

	originalPos = childTransform->getTranslationVector();

	// properly size and position the bounding box based on mesh origin and padding
	boundingBox.x = -boundingBox.width * (0.5f + _def->paddingLeft);;
	boundingBox.y = -childTransform->getTranslationVector().y;
	boundingBox.z = -boundingBox.depth * (0.5f + _def->paddingBack);

	boundingBox.width *= 1.f + _def->paddingLeft + _def->paddingRight;
	boundingBox.depth *= 1.f + _def->paddingFront + _def->paddingBack;

#ifdef _DEBUG
	for(auto &v :  boundingBoxMesh->mesh->vertices){
		v.x = boundingBox.x + (v.x > 0 ? boundingBox.width : 0);
		v.y = boundingBox.y + (v.y > 0 ? boundingBox.height: 0);
		v.z = boundingBox.z + (v.z > 0 ? boundingBox.depth : 0);
	}
	boundingBoxMesh->mesh->dirty = true;
#endif
	// Get the sides information
	if(_def->sides.front != PD_Side::kNONE){
		emptySlots[PD_Side::kFRONT] = new PD_Slot(_def->sides.front, boundingBox.width, _def->overflow);
	}
	if(_def->sides.back != PD_Side::kNONE){
		emptySlots[PD_Side::kBACK] = new PD_Slot(_def->sides.back, boundingBox.width, _def->overflow);
	}
	if(_def->sides.right != PD_Side::kNONE){
		emptySlots[PD_Side::kRIGHT] = new PD_Slot(_def->sides.right, boundingBox.depth, _def->overflow);
	}
	if(_def->sides.left != PD_Side::kNONE){
		emptySlots[PD_Side::kLEFT] = new PD_Slot(_def->sides.left, boundingBox.depth, _def->overflow);
	}
	if(_def->sides.top != PD_Side::kNONE){
		emptySlots[PD_Side::kTOP] = new PD_Slot(_def->sides.top, originalBoundingBox.width); // don't let TOP overflow
	}
}