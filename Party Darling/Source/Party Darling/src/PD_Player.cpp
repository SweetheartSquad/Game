#pragma once

#include <PD_Player.h>
#include <PD_Game.h>
#include <PD_ResourceManager.h>

#include <Resource.h>
#include <MeshInterface.h>
#include <Box2DWorld.h>
#include <Material.h>
#include <MeshFactory.h>

PD_Player::PD_Player(Box2DWorld * _world) :
	Box2DMeshEntity(_world, Resource::loadMeshFromObj("../assets/bunny.vox").at(0), b2_dynamicBody, false),
	NodeTransformable(new Transform()),
	playerMat(new Material(15, glm::vec3(1,1,1), true))
{
	b2Filter sf;
	sf.groupIndex = -1;
	sf.categoryBits = PD_Game::kPLAYER;

	//transform->scale(10.f, 10.f, 10.f);
	mesh->pushMaterial(playerMat);
	mesh->pushTexture2D(PD_ResourceManager::uvs);
	mesh->dirty = true;
	world->addToWorld(this);
	b2Fixture * f = createFixture(true);
	f->SetFilterData(sf);
	f->SetUserData(this);
	f->SetRestitution(0.0f);
	body->SetUserData(this);
}

void PD_Player::update(Step * _step){
	Box2DMeshEntity::update(_step);
}

void PD_Player::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	Box2DMeshEntity::render(_matrixStack, _renderOptions);
}

void PD_Player::setShader(Shader * _shader, bool _configureDefaultAttributes){
	Box2DMeshEntity::setShader(_shader, _configureDefaultAttributes);
}