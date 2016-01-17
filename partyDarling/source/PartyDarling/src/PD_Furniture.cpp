#pragma once

#include <PD_Furniture.h>
#include <PD_FurnitureParser.h>
#include <PD_FurnitureComponent.h>
#include <shader/Shader.h>
#include <MeshEntity.h>
#include <PD_FurnitureComponentDefinition.h>
#include <PD_ResourceManager.h>

PD_Furniture::PD_Furniture(BulletWorld * _bulletWorld, Shader * _shader, PD_FurnitureDefinition * _def) :
	BulletMeshEntity(_bulletWorld, new TriMesh(), _shader)
{
	// make sure that there's only one root
	assert(_def->components.size() == 1);

	// build the furniture
	TriMesh * tempMesh = _def->components.at(0)->build();
	
	// copy the furniture mesh into this entity's mesh
	mesh->vertices.insert(mesh->vertices.end(), tempMesh->vertices.begin(), tempMesh->vertices.end());
	mesh->indices.insert(mesh->indices.end(), tempMesh->indices.begin(), tempMesh->indices.end());
	
	// delete the temporary mesh
	delete tempMesh;

	// Make the mesh dirty since the verts have changed
	// May be redunant but do it as a safe guard
	mesh->dirty = true;

}
