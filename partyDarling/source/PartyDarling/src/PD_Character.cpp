#pragma once

#include <PD_Character.h>
#include <PD_Assets.h>

#include <MeshInterface.h>
#include <MeshFactory.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>
#include <PD_Character.h>
#include <NumberUtils.h>
#include <PD_Palette.h>
#include <PD_CharacterAnimationSet.h>
#include <SpriteSheet.h>
#include <SpriteSheetAnimation.h>

#include <sweet/Input.h>
#include <PD_Listing.h>

Person::Person(BulletWorld * _world, AssetCharacter * const _definition, MeshInterface * _mesh, Shader * _shader, Shader * _emoticonShader, Anchor_t _anchor):
	RoomObject(_world, _mesh, _shader, _anchor),
	pr(new PersonRenderer(_world, _definition, _shader, _emoticonShader)),
	state(&_definition->states.at(_definition->defaultState)),
	definition(_definition),
	room(nullptr),
	enabled(true)
{
	setColliderAsCapsule((pr->solverArmL->getChainLength() + pr->solverArmR->getChainLength())*0.25 *CHARACTER_SCALE, (pr->solverBod->getChainLength() + glm::max(pr->solverLegL->getChainLength(), pr->solverLegR->getChainLength())) * CHARACTER_SCALE);
	
	boundingBox.width = ((pr->solverArmL->getChainLength() + pr->solverArmR->getChainLength())*0.25 *CHARACTER_SCALE) * 2.f;
	boundingBox.height = (pr->solverBod->getChainLength() + glm::max(pr->solverLegL->getChainLength(), pr->solverLegR->getChainLength())) * CHARACTER_SCALE;
	boundingBox.depth = boundingBox.width;

	createRigidBody(5);
	body->setAngularFactor(btVector3(0,1,0)); // prevent from falling over
	meshTransform->setVisible(false);

	childTransform->addChild(pr)->scale(CHARACTER_SCALE);

	translatePhysical(glm::vec3(0, (boundingBox.height+boundingBox.width) * 0.5f, 0.f), false);

	pr->setAnimation(state->animation);

	items = definition->items;
}

void Person::setShader(Shader * _shader, bool _configureDefault){
	RoomObject::setShader(_shader, _configureDefault);
	pr->setShader(_shader, _configureDefault);
}

void Person::update(Step * _step){
	if(enabled) {
		int flags = body->getCollisionFlags();
		flags |= btCollisionObject::CF_STATIC_OBJECT;
		flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;
		body->setCollisionFlags(flags);
		body->activate();
		pr->setVisible(true);
	}else {
		int flags = body->getCollisionFlags();
		flags &= ~btCollisionObject::CF_STATIC_OBJECT;
		flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
		body->setCollisionFlags(flags);
		body->setActivationState(DISABLE_SIMULATION);
		pr->setVisible(false);
	}
	RoomObject::update(_step);
}

Person * Person::createRandomPerson(Scenario * _scenario, BulletWorld * _world, Shader * _shader, Shader * _emoticonShader) {
	Json::Value charDef= genRandomComponents();
	
	std::string id = std::to_string(sweet::NumberUtils::randomFloat(100000, 999999));
	id += std::to_string(sweet::step.time);

	charDef["name"]         = PD_ResourceManager::characterNames.pop();
	charDef["id"]			= id;
	charDef["defaultState"] = id;

	Json::Value stateDef;
	stateDef["id"] = id;
	stateDef["name"] = "defaultState";

	charDef["states"].append(stateDef);

	AssetCharacter * newChar = AssetCharacter::create(charDef, _scenario);
	
	auto p = new Person(_world, newChar, MeshFactory::getPlaneMesh(3.f), _shader, _emoticonShader);

	PD_Listing::listings[_scenario]->characters[id] = p;

	return p;
}

Json::Value Person::genRandomComponents(){
	Json::Value root;
	Json::Value pelvis;
	pelvis["src"] = PD_ResourceManager::characterDefinitions["PELVIS"].pop();
	Json::Value arm;
	arm["src"] = PD_ResourceManager::characterDefinitions["ARM"].pop();
	Json::Value leg;	
	leg["src"] = PD_ResourceManager::characterDefinitions["LEG"].pop();
	Json::Value torso;  
	torso["src"] = PD_ResourceManager::characterDefinitions["TORSO"].pop();
	Json::Value head;  
	head["src"] = PD_ResourceManager::characterDefinitions["HEAD"].pop();

	torso ["components"].append(head);
	torso ["components"].append(arm);
	torso ["components"].append(arm);
	pelvis["components"].append(torso);
	pelvis["components"].append(leg);
	pelvis["components"].append(leg);
	root  ["components"].append(pelvis);

	return root;
}

void Person::disable(){
	enabled = false;
}

void Person::enable(){
	enabled = true;
}

bool Person::isEnabled(){
	return enabled;
}


PersonComponent::PersonComponent(CharacterComponentDefinition * const _definition, Shader * _shader, Texture * _paletteTex, bool _flipped) :
	Sprite(_shader),
	flipped(_flipped)
{
	// get texture
	AssetTexture * tex = PD_ResourceManager::componentTextures->getTexture(_definition->texture);
	tex->load();

	// apply palette + texture
	mesh->pushTexture2D(_paletteTex);
	mesh->pushTexture2D(tex->texture);
	
	in = _definition->in;
	out = _definition->out;
	// handle flipping
	if(flipped){
		meshTransform->scale(-1, 1, 1, false);
		in.x = 1 - in.x;
		for(glm::vec2 & o : out){
			o.x = 1 - o.x;
		}
	}
	
	// multiply percentage coordinates by width/height to corresponding to specific texture
	in.x *= tex->texture->width;
	in.y *= tex->texture->height;
	for(glm::vec2 & o : out){
		o.x *= tex->texture->width;
		o.y *= tex->texture->height;
	}
	
	// scale and translate the mesh into position
	meshTransform->scale(tex->texture->width, tex->texture->height, 1);
	meshTransform->translate(tex->texture->width*0.5f -in.x, tex->texture->height*0.5f -in.y, 0, false);

	mesh->setScaleMode(GL_NEAREST);
}

glm::vec2 PersonComponent::getOut(unsigned long int _index){
	return (out.size() > 0 ? out.at(_index) : glm::vec2(0,0)) - in;
}

PersonLimbSolver::PersonLimbSolver(glm::vec2 _pos) :
	IkChain_CCD(_pos)
{
}

void PersonLimbSolver::addComponent(PersonComponent * _component, float _weight){
	AnimationJoint * j = new AnimationJoint(_component->getOut(0));
	jointsLocal.back()->childTransform->addChild(_component);
	addJointToChain(j);
	components.push_back(_component);
}

PersonState::PersonState(Json::Value _json) :
	id(_json.get("id", "NO_ID").asString()),
	name(_json.get("name", "NO_NAME").asString()),
	conversation(_json.get("convo", "NO_CONVO").asString()),
	animation(_json.get("animation", "RANDOM").asString())
{
}

PersonRenderer::PersonRenderer(BulletWorld * _world, AssetCharacter * const _definition, Shader * _shader, Shader * _emoticonShder) :
	paletteTex(new PD_Palette(false)),
	timer(0),
	randomAnimations(false),
	animate(true),
	currentAnimation(nullptr),
	emote(nullptr),
	emoteTimeout(nullptr),
	talking(false)
{
	paletteTex->generateRandomTable();
	paletteTex->load();
	
	CharacterComponentDefinition
		* pelvisDef			= &_definition->root,
		* torsoDef			= &pelvisDef->components.at(0),

		* jawDef			= &torsoDef->components.at(0),
		* headDef			= &jawDef->components.at(0),
		* noseDef			= &headDef->components.at(0),
		* eyeLDef			= &headDef->components.at(1),
		* eyeRDef			= &headDef->components.at(2),
		* eyebrowLDef		= &headDef->components.at(3),
		* eyebrowRDef		= &headDef->components.at(4),
		* pupilLDef			= &eyeLDef->components.at(0),
		* pupilRDef			= &eyeRDef->components.at(0),

		* armLDef			= &torsoDef->components.at(2),
		* armRDef			= &torsoDef->components.at(1),
		* forearmLDef		= &armLDef->components.at(0),
		* forearmRDef		= &armRDef->components.at(0),
		* handLDef			= &forearmLDef->components.at(0),
		* handRDef			= &forearmRDef->components.at(0),
		
		* legLDef			= &pelvisDef->components.at(2),
		* legRDef			= &pelvisDef->components.at(1),
		* forelegLDef		= &legLDef->components.at(0),
		* forelegRDef		= &legRDef->components.at(0),
		* footLDef			= &forelegLDef->components.at(0),
		* footRDef			= &forelegRDef->components.at(0);


	pelvis = new PersonComponent(pelvisDef, _shader, paletteTex, false);

	torso = new PersonComponent(torsoDef, _shader, paletteTex, false);

	jaw = new PersonComponent(jawDef, _shader, paletteTex, false);
	head = new PersonComponent(headDef, _shader, paletteTex, false);

	nose = new PersonComponent(noseDef, _shader, paletteTex, false);
	eyebrowL = new PersonComponent(eyebrowLDef, _shader, paletteTex, false);
	eyebrowR = new PersonComponent(eyebrowRDef, _shader, paletteTex, false);
	eyeL = new PersonComponent(eyeLDef, _shader, paletteTex, false);
	eyeR = new PersonComponent(eyeRDef, _shader, paletteTex, false);
	pupilL = new PersonComponent(pupilLDef, _shader, paletteTex, false);
	pupilR = new PersonComponent(pupilRDef, _shader, paletteTex, false);

	armR = new PersonComponent(armRDef, _shader, paletteTex, true);
	forearmR = new PersonComponent(forearmRDef, _shader, paletteTex, true);
	handR = new PersonComponent(handRDef, _shader, paletteTex, true);

	armL = new PersonComponent(armLDef, _shader, paletteTex, false);
	forearmL = new PersonComponent(forearmLDef, _shader, paletteTex, false);
	handL = new PersonComponent(handLDef, _shader, paletteTex, false);

	legR = new PersonComponent(legRDef, _shader, paletteTex, true);
	forelegR = new PersonComponent(forelegRDef, _shader, paletteTex, true);
	footR = new PersonComponent(footRDef, _shader, paletteTex, true);
					
	legL = new PersonComponent(legLDef, _shader, paletteTex, false);
	forelegL = new PersonComponent(forelegLDef, _shader, paletteTex, false);
	footL = new PersonComponent(footLDef, _shader, paletteTex, false);

	solverArmR = new PersonLimbSolver(torso->getOut(1));
	solverArmL = new PersonLimbSolver(torso->getOut(2));
	solverLegR = new PersonLimbSolver(pelvis->getOut(1));
	solverLegL = new PersonLimbSolver(pelvis->getOut(2));
	solverBod = new PersonLimbSolver(glm::vec2(0));

	// implicitly create skeletal structure by adding components in the correct order
	solverArmR->addComponent(armR);
	solverArmR->addComponent(forearmR);
	connect(forearmR, handR);
	//solverArmR->addComponent(handR);
	
	solverArmL->addComponent(armL);
	solverArmL->addComponent(forearmL);
	connect(forearmL, handL);
	//solverArmL->addComponent(handL);
	
	solverLegR->addComponent(legR);
	solverLegR->addComponent(forelegR);
	connect(forelegR, footR);
	//solverLegR->addComponent(footR);
	
	solverLegL->addComponent(legL);
	solverLegL->addComponent(forelegL);
	connect(forelegL, footL);
	//solverLegL->addComponent(footL);
	
	solverBod->addComponent(pelvis);
	solverBod->addComponent(torso);
	connect(torso, jaw);
	connect(jaw, head);
	//solverBod->addComponent(jaw);
	//solverBod->addComponent(head);

	// no point in putting the nose/eyes into the skeletal structure
	connect(head, nose);
	connect(head, eyebrowL);
	connect(head, eyebrowR);
	connect(head, eyeL);
	connect(head, eyeR);
	connect(eyeL, pupilL);
	connect(eyeR, pupilR);

	
	// attach the arms/legs to the spine
	solverBod->jointsLocal.at(1)->addJoint(solverArmR);
	solverBod->jointsLocal.at(1)->addJoint(solverArmL);
	solverBod->jointsLocal.at(0)->addJoint(solverLegR, true);
	solverBod->jointsLocal.at(0)->addJoint(solverLegL, true);
	childTransform->addChild(solverBod, false);

	
	solvers.push_back(solverBod);
	solvers.push_back(solverArmR);
	solvers.push_back(solverArmL);
	solvers.push_back(solverLegR);
	solvers.push_back(solverLegL);
	currentSolver = solvers.front();
	
	// pre-initialize the effectors to a T-pose type thing
	solverArmR->target = glm::vec2(-solverArmR->getChainLength(), 0);
	solverArmL->target = glm::vec2(solverArmL->getChainLength(), 0);
	solverLegR->target = glm::vec2(0, -solverLegR->getChainLength());
	solverLegL->target = glm::vec2(0, -solverLegL->getChainLength());
	solverBod->target = glm::vec2(0, solverBod->getChainLength());
	
	
	// talking thing
	talkHeight = head->parents.at(0)->getTranslationVector().y;
	talk = new Animation<float>(&talkHeight);
	talk->tweens.push_back(new Tween<float>(0.1, head->mesh->textures.at(1)->height*0.4, Easing::kEASE_IN_OUT_CIRC));
	talk->tweens.push_back(new Tween<float>(0.1, -head->mesh->textures.at(1)->height*0.4, Easing::kEASE_IN_OUT_CIRC));
	talk->loopType = Animation<float>::LoopType::kLOOP;
	talk->hasStart = true;

	emote = new Sprite(_emoticonShder);
	emote->setVisible(false);
	head->childTransform->addChild(emote);
	emote->firstParent()->scale(1/CHARACTER_SCALE);
}

PersonRenderer::~PersonRenderer(){
	// TODO: Prevent memory leak here without just deleting the paletteTex here
	//delete paletteTex;
	delete currentAnimation;
}

void PersonRenderer::setAnimation(std::string _name) {
	if(_name == "RANDOM"){
		auto it = PD_ResourceManager::characterAnimations.begin();
		int idx = sweet::NumberUtils::randomInt(0, PD_ResourceManager::characterAnimations.size() - 1);
		std::advance(it, idx);
		setAnimation(it->second);
	}else {
		if(PD_ResourceManager::characterAnimations.find(_name) != PD_ResourceManager::characterAnimations.end()) {
			setAnimation(PD_ResourceManager::characterAnimations[_name]);
		}else {
			ST_LOG_ERROR("Animation " + _name + " does not exist");
		}
	}
}

void PersonRenderer::setAnimation(std::vector<PD_CharacterAnimationStep> _steps) {
	delete currentAnimation;
	currentAnimation = new PD_CharacterAnimationSet(this);
	
	for(auto step : _steps) {
		currentAnimation->leftArm.tweens.push_back(new Tween<glm::vec2>(step.time, step.leftArm * solverArmL->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->rightArm.tweens.push_back(new Tween<glm::vec2>(step.time, step.rightArm * solverArmR->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->leftLeg.tweens.push_back(new Tween<glm::vec2>(step.time, step.leftLeg * solverLegL->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->rightLeg.tweens.push_back(new Tween<glm::vec2>(step.time, step.rightLeg * solverLegR->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->body.tweens.push_back(new Tween<glm::vec2>(step.time, step.body * solverBod->getChainLength(), Easing::getTypeByName(step.interpolation)));
	}	

	if(_steps.size() > 0) {
		solverArmL->target = glm::vec2();
		solverArmR->target = glm::vec2();
		solverLegL->target = glm::vec2();
		solverLegR->target = glm::vec2();
		solverBod->target  = glm::vec2();
	}
}

void PersonRenderer::setEmote(std::string _id, float _duration) {
	delete emoteTimeout;
	emoteTimeout = nullptr;

	if(_id == "NONE") {
		emote->setVisible(false);
		return;
	}

	auto eDef = PD_ResourceManager::emotes[_id];
	emote->setVisible(true);
	emote->setSpriteSheet(eDef.spriteSheet, "main");
	auto b = head->mesh->calcBoundingBox();
	float height = b.height/CHARACTER_SCALE;
	float width  = b.width/CHARACTER_SCALE;
	emote->firstParent()->translate(width * eDef.offset.x, height * eDef.offset.y, 0.f, false);
	
	if(_duration > 0.f){
		emoteTimeout = new Timeout(_duration, [this](sweet::Event * _event){
			setEmoteNone();
		});
		emoteTimeout->start();
	}
}

void PersonRenderer::setEmoteNone() {
	setEmote("NONE", -1.f);
}

void PersonRenderer::connect(PersonComponent * _from, PersonComponent * _to, bool _behind){
	joints.push_back(_from->childTransform->addChild(_to));
	joints.back()->translate(
		_from->out.at(_from->connections.size()).x - _from->in.x,
		_from->out.at(_from->connections.size()).y - _from->in.y,
		0, false);
	_from->connections.push_back(_to);
}

void PersonRenderer::setShader(Shader * _shader, bool _default) const {
	pelvis->setShader(_shader, _default);
	torso->setShader(_shader, _default);
	jaw->setShader(_shader, _default);
	head->setShader(_shader, _default);
	
	nose->setShader(_shader, _default);
	eyebrowL->setShader(_shader, _default);
	eyebrowR->setShader(_shader, _default);
	eyeL->setShader(_shader, _default);
	eyeR->setShader(_shader, _default);
	pupilL->setShader(_shader, _default);
	pupilR->setShader(_shader, _default);

	armL->setShader(_shader, _default);
	forearmL->setShader(_shader, _default);
	handL->setShader(_shader, _default);

	armR->setShader(_shader, _default);
	forearmR->setShader(_shader, _default);
	handR->setShader(_shader, _default);

	legL->setShader(_shader, _default);
	forelegL->setShader(_shader, _default);
	footL->setShader(_shader, _default);

	legR->setShader(_shader, _default);
	forelegR->setShader(_shader, _default);
	footR->setShader(_shader, _default);
}

void PersonRenderer::update(Step * _step){
	
	
	if(Keyboard::getInstance().keyJustDown(GLFW_KEY_Y)){
		paletteTex->generateRandomTable();
		paletteTex->bufferData();
	}

	timer += _step->deltaTime;

	if(emoteTimeout != nullptr) {
		emoteTimeout->update(_step);
	}

	if(animate){
		if(randomAnimations && timer > 1){
			timer = 0;
			float l;

			l = solverArmR->getChainLength();
			solverArmR->target.x = sweet::NumberUtils::randomFloat(-l, 0);
			solverArmR->target.y = sweet::NumberUtils::randomFloat(-l, l);
		
			l = solverArmL->getChainLength();
			solverArmL->target.x = sweet::NumberUtils::randomFloat(l, 0);
			solverArmL->target.y = sweet::NumberUtils::randomFloat(-l, l);
		
			l = solverLegR->getChainLength();
			solverLegR->target.x = sweet::NumberUtils::randomFloat(0, l*0.5);
			solverLegR->target.y = sweet::NumberUtils::randomFloat(-l, -l*0.8);
		
			l = solverLegL->getChainLength();
			solverLegL->target.x = sweet::NumberUtils::randomFloat(-l*0.5, 0);
			solverLegL->target.y = sweet::NumberUtils::randomFloat(-l, -l*0.8);
		
			l = solverBod->getChainLength();
			solverBod->target.x = sweet::NumberUtils::randomFloat(-l*0.5, l*0.5);
			solverBod->target.y = sweet::NumberUtils::randomFloat(l*0.95, l);

			/*solverArmL->target = glm::vec2(solverArmL->getChainLength(), 0);
			solverLegR->target = glm::vec2(0, -solverLegR->getChainLength());
			solverLegL->target = glm::vec2(0, -solverLegL->getChainLength());
			solverBod->target = glm::vec2(0, solverBod->getChainLength());
		
			for(unsigned long int s = 1; s < solvers.size(); ++s){
				float l = solvers.at(s)->getChainLength();
				solvers.at(s)->target.x = sweet::NumberUtils::randomFloat(-l, l);
				solvers.at(s)->target.y = sweet::NumberUtils::randomFloat(-l, l);
			}*/

		}else if(currentAnimation != nullptr) {
			currentAnimation->update(_step);
		}
	}


	Keyboard & k = Keyboard::getInstance();

	glm::vec2 test(0);
	if(k.keyDown(GLFW_KEY_I)){
		test.y += 50;
	}if(k.keyDown(GLFW_KEY_J)){
		test.x -= 50;
	}if(k.keyDown(GLFW_KEY_K)){
		test.y -= 50;
	}if(k.keyDown(GLFW_KEY_L)){
		test.x += 50;
	}

	currentSolver->target += test;
	
	if(k.keyJustDown(GLFW_KEY_U)){
		if(currentSolver == solvers.back()){
			currentSolver = solvers.at(0);
		}else{
			for(unsigned long int i = 0; i < solvers.size()-1; ++i){
				if(currentSolver == solvers.at(i)){
					currentSolver = solvers.at(i+1);
					break;
				}
			}
		}
	}

	if(talking){
		// talking
		talk->update(_step);
		glm::vec3 v = head->parents.at(0)->getTranslationVector();
		head->parents.at(0)->translate(v.x, talkHeight, v.z, false);
	}else {
		head->parents.at(0)->translate(head->parents.at(0)->getTranslationVector().x, 
			0.2/CHARACTER_SCALE, 
			head->parents.at(0)->getTranslationVector().z, false);
	}

	Entity::update(_step);
}