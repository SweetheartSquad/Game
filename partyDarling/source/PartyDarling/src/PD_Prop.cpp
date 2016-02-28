#pragma once

#include <PD_Prop.h>
#include <PD_PropDefinition.h>
#include <PD_Furniture.h>

#include <Resource.h>
#include <Easing.h>
#include <Texture.h>
#include <NumberUtils.h>
#include <MeshDeformation.h>


PD_Prop::PD_Prop(BulletWorld * _bulletWorld, PD_PropDefinition * _def, Shader * _shader, Anchor_t _anchor) :
	RoomObject(_bulletWorld, new TriMesh(true), _shader, _anchor)
{
	
	// copy the furniture mesh and texture into the prop
	mesh->insertVertices(*_def->mesh);
	mesh->pushTexture2D(_def->mesh->textures.at(0));
	mesh->textures.at(0)->load();


	
	
	//Deformers
	float lowerFlareVal = sweet::NumberUtils::randomFloat(0.f,0.4f);
	float upperFlareVal = sweet::NumberUtils::randomFloat(0.f,(1.f - (0.5f+lowerFlareVal)));
	float lowerBoundVal = sweet::NumberUtils::randomFloat(0.2f,0.3f);
		
	if(_def->twist){
		MeshDeformation::twist(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kLINEAR);
	}
		//MeshDeformation::bend(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kLINEAR);
	if(_def->flare){
		MeshDeformation::flare(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kLINEAR);
	}

	// Make the mesh dirty since the verts have changed
	// May be redunant but do it as a safe guard
	mesh->dirty = true;
	
	meshTransform->scale(0.15f, 0.15f, 0.15f);
	freezeTransformation();

	// we need to inform the RoomObject of the new bounding box here
	boundingBox = mesh->calcBoundingBox();


	
	setColliderAsBoundingBox();
	createRigidBody(_def->mass * FURNITURE_MASS_SCALE );
	
	translatePhysical(glm::vec3(0, -boundingBox.y, 0.f), false);

	parentDependent = _def->parentDependent;
	// Get parent types
	parentTypes.insert(parentTypes.begin(), _def->parents.begin(), _def->parents.end());

	// padding
	padding = _def->padding;

	boundingBox.width *= 1.f + padding * 2.f;
	boundingBox.depth *= 1.f + padding * 2.f;

	boundingBox.x = -boundingBox.width * 0.5f;
	boundingBox.z -= -boundingBox.depth * 0.5f;
}