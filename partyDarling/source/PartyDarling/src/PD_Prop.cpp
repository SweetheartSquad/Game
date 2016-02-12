#pragma once

#include <PD_Prop.h>
#include <PD_PropDefinition.h>

#include <Resource.h>
#include <Easing.h>
#include <Texture.h>
#include <NumberUtils.h>

#include <PD_Furniture.h>

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
		sweet::Box deformerBoundingBox = mesh->calcBoundingBox();
	
	
		float lowerFlareVal = sweet::NumberUtils::randomFloat(0.f,0.4f);
		float upperFlareVal = sweet::NumberUtils::randomFloat(0.f,(1.f - (0.5f+lowerFlareVal)));
		float lowerBoundVal = sweet::NumberUtils::randomFloat(0.2f,0.3f);

		for(auto & v : mesh->vertices){
			//normalize vertex position by the bounding box of the mesh
			float vertX = (v.x) / deformerBoundingBox.width;
			float vertY = (v.y - deformerBoundingBox.y - lowerBoundVal) / deformerBoundingBox.height;
			float vertZ = (v.z) / deformerBoundingBox.depth;

			//create a vec4 of vertex position
			glm::vec4 vertVector4 = glm::vec4( vertX, vertY, vertZ, 1.0f);
			//flare deformation matrix
			glm::mat4 flareMatrix =  glm::mat4 (Easing::easeInOutCubic(vertY,0.5f+lowerFlareVal,0.5f+upperFlareVal,1.f),0.f,0.f,0.f,
												0.f,1.f,0.f,0.f,
												0.f,0.f,Easing::easeInOutCubic(vertY,0.5f+lowerFlareVal,0.5f+upperFlareVal,1.f),0.f,
												0.f,0.f,0.f,1.f);

			//bend deformer matrix
			glm::mat4 bendMatrix = glm::mat4 (glm::cos(0.25f*vertY),glm::sin(0.25f*vertY),0.f,0.f,
												Easing::easeInOutCubic(vertY,0.5f,0.1f,1.f),Easing::easeInOutCubic(vertY,0.5f,0.1f,1.f),0.f,0.f,
												0.f,0.f,1.f,0.f,
												0.f,0.f,0.f,1.f);

			//twist deformer matrix
			glm::mat4 twistMatrix = glm::mat4 (glm::cos(0.25f*vertY),0.f,glm::sin(0.25f*vertY),0.f,
												0.f,1.f,0.f,0.f,
												-1.0f*(glm::sin(0.25f*vertY)),0.f,glm::cos(0.25f*vertY),0.f,
												0.f,0.f,0.f,1.f);

			//multiply matrix by normalize vertex position vector
			glm::vec4 newVertVector4 = flareMatrix /*bendMatrix*/ * twistMatrix * vertVector4;

			//change vertex positions, scale them up by bounding box dimensions
			v.x = newVertVector4.x  * deformerBoundingBox.width;
			v.y = newVertVector4.y * deformerBoundingBox.height + deformerBoundingBox.y + lowerBoundVal;
			v.z = newVertVector4.z * deformerBoundingBox.depth;
		}
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