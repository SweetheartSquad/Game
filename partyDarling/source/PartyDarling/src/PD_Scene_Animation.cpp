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
#include <Keyboard.h>


Effector::Effector(BulletWorld* _world, PersonLimbSolver * _solver) : NodeUI(_world) {
	solver = _solver;
	mouseEnabled = true;
	active = false;
	setBackgroundColour(1, 1, 1, 1);
	setWidth(25);
	setHeight(25);

	eventManager.addEventListener("click", [this](sweet::Event * event){
		active = !active;
		if(active) {
			setBackgroundColour(1, 0, 0);
		}else {
			setBackgroundColour(1, 1, 1);	
		}
	});

}

void Effector::update(Step * _step) {
	if(active) {
		glm::uvec2 sd = sweet::getWindowDimensions();
		
	}
	NodeUI::update(_step);
}

void Effector::setPos(glm::vec3 _mpos, glm::vec3 _pos) {
	if(active) {
		solver->target =  glm::vec2(_pos.x, _pos.y );
		firstParent()->translate(_mpos.x - 12.5, _mpos.y - 12.5f, 0.f, false);
	}
}

PD_Scene_Animation::PD_Scene_Animation(Game* _game) :
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

	//Set up debug camera
	OrthographicCamera * debugCam = new OrthographicCamera(-4, 4, -2.25, 2.25, 0.1, 1000);
	cameras.push_back(debugCam);
	childTransform->addChild(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(0.f, 1.5f, 2.f);
	debugCam->yaw = 90.0f;
	activeCamera = debugCam;

	auto charAsset = dynamic_cast<AssetCharacter *>(PD_ResourceManager::scenario->getAsset("character", "2"));
	character = new Person(bulletWorld, charAsset, MeshFactory::getPlaneMesh(3.f), characterShader);
	character->pr->randomAnimations = false;
	uiLayer.childTransform->addChild(character);
		
	for(auto solver : character->pr->solvers) {
		solver->setJointsVisible(true);
	}

	character->firstParent()->scale(100)->translate(sd.x * 0.5f, sd.y * 0.5f - 150.f, 0.f);

	leftArmEffector = new Effector(uiLayer.world, character->pr->solverArmR);
	uiLayer.childTransform->addChild(leftArmEffector);
	leftArmEffector->firstParent()->translate(sd.x * 0.5f - 300, 700, 0.f, false);

	rightArmEffector = new Effector(uiLayer.world, character->pr->solverArmL);
	uiLayer.childTransform->addChild(rightArmEffector);
	rightArmEffector->firstParent()->translate(sd.x * 0.5f + 300, 700, 0.f, false);

	rightLegEffector = new Effector(uiLayer.world, character->pr->solverLegR);
	uiLayer.childTransform->addChild(rightLegEffector);
	rightLegEffector->firstParent()->translate(sd.x * 0.5f + 300, 200, 0.f, false);

	leftLegEffector = new Effector(uiLayer.world, character->pr->solverLegL);
	uiLayer.childTransform->addChild(leftLegEffector);
	leftLegEffector->firstParent()->translate(sd.x * 0.5f - 300, 200, 0.f, false);

	bodyEffector = new Effector(uiLayer.world, character->pr->solverBod);
	uiLayer.childTransform->addChild(bodyEffector);
	bodyEffector->firstParent()->translate(sd.x * 0.5f - 12.5f, sd.y * 0.5f, 0.f, false);

	character->pr->setAnimation("test");

	uiLayer.addMouseIndicator();
}

PD_Scene_Animation::~PD_Scene_Animation() {
}

void PD_Scene_Animation::update(Step * _step) {

	glm::uvec2 sd = sweet::getWindowDimensions();

	glm::vec3 mPos = uiLayer.mouseIndicator->firstParent()->getTranslationVector();
	glm::vec3 pos =  glm::vec3(2.f * (mPos.x - sd.x * 0.5f), 4.f * (mPos.y - sd.y * 0.5f + 150.f), 0);

	leftArmEffector->setPos(mPos, pos);
	rightArmEffector->setPos(mPos, pos);
	rightLegEffector->setPos(mPos, pos);
	leftLegEffector->setPos(mPos, pos);
	bodyEffector->setPos(mPos, pos);

	if(keyboard->keyJustUp(GLFW_KEY_D)) {
		copyJsonToClipboard();
	}

	if(keyboard->keyJustUp(GLFW_KEY_F)) {
		character->pr->animate = !character->pr->animate; 
	}

	Scene::update(_step);

	uiLayer.resize(0,sd.x,0,sd.y);
	uiLayer.update(_step);
}

void PD_Scene_Animation::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) {
	_renderOptions->setClearColour(0,0,0,1);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_Scene_Animation::load() {

	Scene::load();
}

void PD_Scene_Animation::unload() {

	Scene::unload();
}

void PD_Scene_Animation::copyJsonToClipboard() const {
	std::stringstream json;
	json << "{" << std::endl;
	json << "\t" << "\"interpolation\" : LINEAR," << std::endl;
	json << "\t" << "\"time\" : 0.0," << std::endl;
	json << "\t" << "\"leftArm\" : [" << character->pr->solverArmL->target.x  << ", " << character->pr->solverArmL->target.y << "],"<< std::endl;
	json << "\t" << "\"rightArm\" : [" << character->pr->solverArmR->target.x  << ", " << character->pr->solverArmR->target.y << "],"<< std::endl;
	json << "\t" << "\"leftLeg\" : [" << character->pr->solverLegL->target.x  << ", " << character->pr->solverLegL->target.y << "],"<< std::endl;
	json << "\t" << "\"rightLeg\" : [" << character->pr->solverLegR->target.x  << ", " << character->pr->solverLegR->target.y << "]"<< std::endl;
	json << "\t" << "\"body\" : [" << character->pr->solverBod->target.x  << ", " << character->pr->solverBod->target.y << "]"<< std::endl;
	json << "}" << std::endl;
	glfwSetClipboardString(glfwGetCurrentContext(), json.str().c_str());
}
