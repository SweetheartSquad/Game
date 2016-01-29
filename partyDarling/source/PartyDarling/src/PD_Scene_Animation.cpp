#pragma once

#include <PD_Scene_Animation.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentIndexedTexture.h>
#include <shader/ShaderComponentDepthOffset.h>
#include <MousePerspectiveCamera.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <MeshFactory.h>
#include <RenderOptions.h>
#include <shader/ShaderComponentTexture.h>

PD_AnimationScene::PD_AnimationScene(Game* _game) :
	Scene(_game), 
	characterShader(new ComponentShaderBase(false)),
	baseShader(new ComponentShaderBase(false)),
	uiLayer(0, 0, 0, 0),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr)
{
	characterShader->addComponent(new ShaderComponentMVP(characterShader));
	characterShader->addComponent(new ShaderComponentIndexedTexture(characterShader));
	characterShader->addComponent(new ShaderComponentDepthOffset(characterShader));
	characterShader->compileShader();

	baseShader->addComponent(new ShaderComponentMVP(baseShader));
	baseShader->addComponent(new ShaderComponentTexture(baseShader));
	baseShader->compileShader();

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);

	// remove initial camera
	childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();

	//Set up debug camera
	OrthographicCamera * debugCam = new OrthographicCamera(-4, 4, -2.25, 2.25, 0.1, 1000);
	cameras.push_back(debugCam);
	childTransform->addChild(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(0.f, 1.5f, 2.f);
	debugCam->yaw = 90.0f;
	activeCamera = debugCam;

	uiLayer.addMouseIndicator();

	auto charAsset = dynamic_cast<AssetCharacter *>(PD_ResourceManager::scenario->getAsset("character", "2"));
	character = new Person(bulletWorld, charAsset, MeshFactory::getPlaneMesh(3.f), characterShader);
	childTransform->addChild(character);
		
	for(auto solver : character->pr->solvers) {
		solver->setJointsVisible(true);
	}
}

PD_AnimationScene::~PD_AnimationScene() {
}

void PD_AnimationScene::update(Step * _step) {
	Scene::update(_step);
}

void PD_AnimationScene::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) {
	_renderOptions->setClearColour(0,0,0,1);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_AnimationScene::load() {

	Scene::load();
}

void PD_AnimationScene::unload() {

	Scene::unload();
}
