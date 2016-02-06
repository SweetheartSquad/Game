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


Keyframe::Keyframe(BulletWorld * _bulletWorld, Person * _character) : 
	NodeUI(_bulletWorld),
	character(_character)
{
	step = new PD_CharacterAnimationStep();
	setWidth(20.f);
	setHeight(20.f);
	setBackgroundColour(1.f, 0.f, 0.f);
	setMarginRight(5.f);
	mouseEnabled = true;
}

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
	debugDrawer(nullptr),
	currentKeyFrame(nullptr)
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

	keyFrameLayout = new HorizontalLinearLayout(uiLayer.world);
	keyFrameLayout->setRationalWidth(1.0f, &uiLayer);
	keyFrameLayout->setPixelHeight(20.0f);
	keyFrameLayout->invalidateLayout();

	uiLayer.addChild(keyFrameLayout);

	uiLayer.addMouseIndicator();

	uiLayer.bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);

	character->pr->animate = false;
	
	loadFromTestFile();
}

PD_Scene_Animation::~PD_Scene_Animation() {
}

void PD_Scene_Animation::update(Step * _step) {

	updateEffectors();

	if(keyboard->keyJustUp(GLFW_KEY_D)) {
		if(!character->pr->animate){
			writeToFile();
			loadFromTestFile();
			character->pr->animate = true;
		}else {
			character->pr->animate = false;
		}
	}

	if(keyboard->keyJustUp(GLFW_KEY_L)) {
		loadFromTestFile();
		character->pr->animate = true;
	}

	if(keyboard->keyJustUp(GLFW_KEY_M)) {
		character->pr->animate = false;
	}

	if(keyboard->keyJustUp(GLFW_KEY_F)) {
		character->pr->animate = !character->pr->animate; 
	}

	if(keyboard->keyJustUp(GLFW_KEY_K)) {
		
		Keyframe * keyframe;

		if(currentKeyFrame != nullptr){
			currentKeyFrame->setBackgroundColour(1.f, 0.f, 0.f);
			keyframe = currentKeyFrame;
		}else {
			keyframe = new Keyframe(uiLayer.world, character);
		}			

		keyframe->step->leftArm = glm::vec2(
			character->pr->solverArmL->target.x,
			character->pr->solverArmL->target.y);

		keyframe->step->rightArm = glm::vec2(
			character->pr->solverArmR->target.x,
			character->pr->solverArmR->target.y);

		keyframe->step->leftLeg = glm::vec2(
			character->pr->solverLegL->target.x,
			character->pr->solverLegL->target.y);

		keyframe->step->rightLeg = glm::vec2(
			character->pr->solverLegR->target.x,
			character->pr->solverLegR->target.y);

		keyframe->step->body = glm::vec2(
			character->pr->solverBod->target.x,
			character->pr->solverBod->target.y);
		
		keyframe->eventManager.addEventListener("click", [this, keyframe](sweet::Event * _event){
			if(currentKeyFrame != nullptr){
				currentKeyFrame->setBackgroundColour(1.f, 0.f, 0.f);
			}
			character->pr->solverArmL->target = keyframe->step->leftArm;
			character->pr->solverArmR->target = keyframe->step->rightArm;
			character->pr->solverLegL->target = keyframe->step->leftLeg;
			character->pr->solverLegR->target = keyframe->step->rightLeg;
			character->pr->solverBod->target =  keyframe->step->body;
			currentKeyFrame = keyframe;
			currentKeyFrame->setBackgroundColour(1.f, 1.f, 1.f);
		});

		if(currentKeyFrame == nullptr) {
			keyframes.push_back(keyframe);
			keyFrameLayout->addChild(keyframe);
		}else {
			currentKeyFrame = nullptr;
		}
	}

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
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

void PD_Scene_Animation::updateEffectors() const {
	
	glm::uvec2 sd = sweet::getWindowDimensions();

	glm::vec3 mPos = uiLayer.mouseIndicator->firstParent()->getTranslationVector();
	glm::vec3 pos =  glm::vec3(4.f * (mPos.x - sd.x * 0.5f), 8.f * (mPos.y - sd.y * 0.5f + 150.f), 0);

	leftArmEffector->setPos(mPos, pos);
	rightArmEffector->setPos(mPos, pos);
	rightLegEffector->setPos(mPos, pos);
	leftLegEffector->setPos(mPos, pos);
	bodyEffector->setPos(mPos, pos);
}

void PD_Scene_Animation::writeToFile() const {

	PD_CharacterAnimationStep * lastStep = new PD_CharacterAnimationStep();

	float lenAL = character->pr->solverArmL->getChainLength();
	float lenAR = character->pr->solverArmR->getChainLength();
	float lenLL = character->pr->solverLegL->getChainLength();
	float lenLR = character->pr->solverLegR->getChainLength();
	float lenB = character->pr->solverBod->getChainLength();

	std::stringstream json;
	json << "[" << std::endl;
	float time = 0.0f;
	for(auto key : keyframes){
		json << "{" << std::endl;
		json << "\t" << "\"interpolation\" : \"LINEAR\"," << std::endl;
		json << "\t" << "\"time\" : " << time << "," << std::endl;
		json << "\t" << "\"leftArm\" : [" << (key->step->leftArm.x - lastStep->leftArm.x)/lenAL << ", " << (key->step->leftArm.y - lastStep->leftArm.y)/lenAL << "],"<< std::endl;
		json << "\t" << "\"rightArm\" : [" << (key->step->rightArm.x - lastStep->rightArm.x)/lenAR  << ", " << (key->step->rightArm.y - lastStep->rightArm.y)/lenAR << "],"<< std::endl;
		json << "\t" << "\"leftLeg\" : [" << (key->step->leftLeg.x - lastStep->leftLeg.x)/lenLL << ", " << (key->step->leftLeg.y - lastStep->leftLeg.y)/lenLL << "],"<< std::endl;
		json << "\t" << "\"rightLeg\" : [" << (key->step->rightLeg.x - lastStep->rightLeg.x)/lenLR << ", " << (key->step->rightLeg.y - lastStep->rightLeg.y)/lenLR << "],"<< std::endl;
		json << "\t" << "\"body\" : [" << (key->step->body.x - lastStep->body.x)/lenB  << ", " << (key->step->body.y - lastStep->body.y)/lenB << "]"<< std::endl;
		if(key == keyframes.back()){
			json << "}" << std::endl;
		}else {
			json << "}," << std::endl;
		}
		lastStep = key->step;
		time += 1.0f;
	}
	json << "]";

	std::ofstream file("assets/animations/test.json");
	file << json.str();
	file.close();
}

void PD_Scene_Animation::loadFromTestFile() {
	testSteps.clear();
	sweet::FileUtils::createFileIfNotExists("assets/animations/test.json");
	std::string json = sweet::FileUtils::readFile("assets/animations/test.json");
	if(json == "") {
		json = "[]";
	}	
	Json::Reader reader;
	Json::Value animStep;
	bool parsingSuccessful = reader.parse( json, animStep );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
	}else{					
		for(auto step : animStep) {
			testSteps.push_back(PD_CharacterAnimationStep(step));	
		}
	}
	character->pr->setAnimation(testSteps);
}
