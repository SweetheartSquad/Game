#pragma once

#include <PD_Furniture.h>
#include <PD_FurnitureParser.h>
#include <PD_FurnitureComponent.h>
#include <shader/Shader.h>
#include <MeshEntity.h>
#include <PD_FurnitureComponentDefinition.h>
#include <PD_ResourceManager.h>
#include <NumberUtils.h>

#define FURNITURE_MASS_SCALE 0.05

PD_Furniture::PD_Furniture(BulletWorld * _bulletWorld, PD_FurnitureDefinition * _def, Shader * _shader, Anchor_t _anchor) :
	RoomObject(_bulletWorld, new TriMesh(), _shader, _anchor)
{
	// make sure that there's only one root
	assert(_def->components.size() == 1);

	// build the furniture
	PD_BuildResult buildResult = _def->components.at(0)->build();
	
	// get a texture for the furniture type
	std::stringstream ss;
	ss << "assets/textures/furniture/" << _def->type << "_" << sweet::NumberUtils::randomInt(1, 2) << ".png";
	Texture * tex = new Texture(ss.str(), false, true, true);
	tex->load();
	mesh->pushTexture2D(tex);
	mesh->setScaleMode(GL_NEAREST);

	// copy the furniture mesh into this entity's mesh
	mesh->insertVertices(buildResult.mesh);
	
	// delete the temporary mesh
	delete buildResult.mesh;
	/**** Won't work, since I guess the stuff over the origin won't necessarily be the same height as the stuf below???? *****
	// move all of the vertices up so that the origin is at the base of the mesh
	float h = mesh->calcBoundingBox().height * 0.5f;
	for(Vertex & v : mesh->vertices){
		v.y += h;
	}
	*/

	// Make the mesh dirty since the verts have changed
	// May be redunant but do it as a safe guard
	mesh->dirty = true;
	
	meshTransform->scale(0.15f, 0.15f, 0.15f);
	freezeTransformation();

	// we need to inform the RoomObject of the new bounding box here
	boundingBox = mesh->calcBoundingBox();

	// create the bullet stuff
	shape = buildResult.collider;
	createRigidBody(_def->mass * FURNITURE_MASS_SCALE);
	
	translatePhysical(glm::vec3(0, mesh->calcBoundingBox().height * 0.5f, 0.f), false);
}
