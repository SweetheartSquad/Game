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
	
	// copy the furniture mesh into this entity's mesh
	{
		std::stringstream ss;
		ss << "assets/meshes/props/" << _def->src << ".obj";
		MeshInterface * m = Resource::loadMeshFromObj(ss.str(), false).at(0);
		mesh->setScaleMode(GL_NEAREST);
		mesh->insertVertices(m);
		delete m;
	}
	
	// get a texture for the furniture type
	{
		std::stringstream ss;
		ss << "assets/textures/props/" << _def->src << ".png";
		Texture * tex = new Texture(ss.str(), false, true, true);
		tex->load();
		mesh->pushTexture2D(tex);	
	}


	
	
	//Deformers
	if(_def->deformable){
		float lowerFlareVal = sweet::NumberUtils::randomFloat(0.f,0.4f);
		float upperFlareVal = sweet::NumberUtils::randomFloat(0.f,(1.f - (0.5f+lowerFlareVal)));
		float lowerBoundVal = sweet::NumberUtils::randomFloat(0.2f,0.3f);
		
		MeshDeformation::twist(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kEASE_IN_OUT_CUBIC);
		//MeshDeformation::bend(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kEASE_IN_OUT_CUBIC);
		MeshDeformation::flare(mesh, lowerFlareVal, upperFlareVal, lowerBoundVal, Easing::kEASE_IN_OUT_CUBIC);
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

	// Get parent types
	parentTypes.insert(parentTypes.begin(), _def->parents.begin(), _def->parents.end());
}