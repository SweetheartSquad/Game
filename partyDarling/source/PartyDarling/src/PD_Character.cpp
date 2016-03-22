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
#include <PD_DissStats.h>
#include <PD_Game.h>

unsigned long int PD_Character::numRandomCharacters = 0;

PD_Character::PD_Character(BulletWorld * _world, AssetCharacter * const _definition, MeshInterface * _mesh, Shader * _shader, Shader * _emoticonShader, Anchor_t _anchor):
	RoomObject(_world, _mesh, _shader, _anchor),
	state(&_definition->states.at(_definition->defaultState)),
	room(nullptr),
	definition(_definition),
	pr(new CharacterRenderer(_world, _definition, _shader, _emoticonShader)),
	enabled(true),
	hidden(false),
	dissedAt(false),
	wonDissBattle(false),
	dissStats(new PD_DissStats())
{
	pr->owner = this;

	dissStats->incrementDefense(definition->defense);
	dissStats->incrementInsight(definition->insight);
	dissStats->incrementSass(definition->sass);
	dissStats->incrementStrength(definition->strength);

	billboarded = true;

	float w = (pr->solverArmL->getChainLength() + pr->solverArmR->getChainLength()) * CHARACTER_SCALE;
	float h = (pr->solverBod->getChainLength() + glm::max(pr->solverLegL->getChainLength(), pr->solverLegR->getChainLength()) + pr->head->meshTransform->getScaleVector().y + pr->jaw->meshTransform->getScaleVector().y + pr->footL->meshTransform->getScaleVector().y) * CHARACTER_SCALE;

	setColliderAsCapsule(w * 0.25f, h - w * 0.5f);

	boundingBox.width = w;
	boundingBox.height = h;
	boundingBox.depth = boundingBox.width;

	boundingBox.x = -boundingBox.width * 0.5f;
	boundingBox.y = -boundingBox.height * 0.5f;
	boundingBox.z = -boundingBox.depth * 0.5f;

#ifdef _DEBUG
	for(auto &v :  boundingBoxMesh->mesh->vertices){
		v.x = boundingBox.x + (v.x > 0 ? boundingBox.width : 0);
		v.y = boundingBox.y + (v.y > 0 ? boundingBox.height: 0);
		v.z = boundingBox.z + (v.z > 0 ? boundingBox.depth : 0);
	}
	boundingBoxMesh->mesh->dirty = true;
#endif

	createRigidBody(5);
	body->setAngularFactor(btVector3(0,0,0)); // prevent from rotating the physics body at all
	meshTransform->setVisible(false);

	childTransform->addChild(pr)->scale(CHARACTER_SCALE);

	translatePhysical(glm::vec3(0, boundingBox.height * 0.5f, 0.f), false);
	realign();

	originalPos = childTransform->getTranslationVector();

	/* I don't know how to get them centered/aligned with the bottom of the collider
	float offsetY = -pr->footL->getWorldPos().y + (pr->footL->meshTransform->getScaleVector().y * 0.5f * CHARACTER_SCALE);
	pr->firstParent()->translate(0, offsetY, 0);
	*/
	pr->setAnimation(state->animation);

	items = definition->items;

	if(!definition->enabled) {
		disable();
	}

	if(!definition->visible) {
		hide();
	}

	if(_definition->voice == "RANDOM"){
		voice = PD_ResourceManager::voices[std::to_string(sweet::NumberUtils::randomInt(1, 37))];
	}else {
		voice = PD_ResourceManager::voices[_definition->voice];
	}
}

PD_Character::~PD_Character(){
	delete dissStats;
	
	// deal with any items which the character is holding because they aren't actually in the scene
	for(auto i : items){
		delete PD_Listing::listings[definition->scenario]->items[i];
	}
}

void PD_Character::setShader(Shader * _shader, bool _configureDefault){
	RoomObject::setShader(_shader, _configureDefault);
	pr->setShader(_shader, _configureDefault);
}

void PD_Character::update(Step * _step){
	if(enabled) {
		int flags = body->getCollisionFlags();
		flags |= btCollisionObject::CF_STATIC_OBJECT;
		flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;
		body->setCollisionFlags(flags);
		body->activate();

		pr->eyeAnim->update(_step);
		pr->pelvisAnim->update(_step);
		pr->eyeL->childTransform->scale(pr->eyeScale, false);
		pr->eyeR->childTransform->scale(pr->eyeScale, false);
		pr->childTransform->scale(pr->pelvisScale, false);
	}else {
		int flags = body->getCollisionFlags();
		flags &= ~btCollisionObject::CF_STATIC_OBJECT;
		flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
		body->setCollisionFlags(flags);
		body->setActivationState(DISABLE_SIMULATION);
	}
	if(hidden) {
		pr->setVisible(false);
	}else {
		pr->setVisible(true);
	}

	RoomObject::update(_step);
}


void PD_Character::load(){
	if(!loaded){
		// deal with any items which the character is holding because they aren't actually in the scene
		for(auto i : items){
			PD_Listing::listings[definition->scenario]->items[i]->load();
		}
	}
	RoomObject::load();
}

void PD_Character::unload(){
	if(loaded){
		// deal with any items which the character is holding because they aren't actually in the scene
		for(auto i : items){
			PD_Listing::listings[definition->scenario]->items[i]->unload();
		}
	}
	RoomObject::unload();
}


PD_Character * PD_Character::createRandomPD_Character(Scenario * _scenario, BulletWorld * _world, Shader * _shader, Shader * _emoticonShader) {
	Json::Value charDef = genRandomComponents();

	std::string id = "RANDOM_CHARACTER_" + std::to_string(++numRandomCharacters);

	charDef["name"]         = sweet::NumberUtils::randomItem(PD_ResourceManager::characterNames);
	charDef["id"]			= id;
	charDef["defaultState"] = id;

	Json::Value stateDef;
	stateDef["id"] = id;
	stateDef["name"] = "defaultState";

	charDef["states"].append(stateDef);

	AssetCharacter * newChar = AssetCharacter::create(charDef, _scenario);
	_scenario->assets["character"][id] = newChar;

	PD_Character * p = new PD_Character(_world, newChar, MeshFactory::getPlaneMesh(3.f), _shader, _emoticonShader);

	if(PD_Game::progressManager != nullptr){
		p->dissStats->incrementDefense(sweet::NumberUtils::randomInt(0, PD_Game::progressManager->plotPosition));
		p->dissStats->incrementInsight(sweet::NumberUtils::randomInt(0, PD_Game::progressManager->plotPosition));
		p->dissStats->incrementStrength(sweet::NumberUtils::randomInt(0, PD_Game::progressManager->plotPosition));
		p->dissStats->incrementSass(sweet::NumberUtils::randomInt(0, PD_Game::progressManager->plotPosition));
	}

	if(PD_Listing::listings.find(_scenario) != PD_Listing::listings.end()){
		PD_Listing::listings[_scenario]->characters[id] = p;
	}

	return p;
}

Json::Value PD_Character::genRandomComponents(){
	Json::Value root;
	Json::Value pelvis;
	pelvis["src"] = sweet::NumberUtils::randomItem(PD_ResourceManager::characterDefinitions["PELVIS"]);
	Json::Value arm;
	arm["src"] = sweet::NumberUtils::randomItem(PD_ResourceManager::characterDefinitions["ARM"]);
	Json::Value leg;
	leg["src"] = sweet::NumberUtils::randomItem(PD_ResourceManager::characterDefinitions["LEG"]);
	Json::Value torso;
	torso["src"] = sweet::NumberUtils::randomItem(PD_ResourceManager::characterDefinitions["TORSO"]);
	Json::Value head;
	head["src"] = sweet::NumberUtils::randomItem(PD_ResourceManager::characterDefinitions["HEAD"]);

	torso ["components"].append(head);
	torso ["components"].append(arm);
	torso ["components"].append(arm);
	pelvis["components"].append(torso);
	pelvis["components"].append(leg);
	pelvis["components"].append(leg);
	root  ["components"].append(pelvis);

	return root;
}

void PD_Character::disable(){
	enabled = false;
}

void PD_Character::hide() {
	hidden = true;
	disable();
}

void PD_Character::show() {
	hidden = false;
	enable();
}

bool PD_Character::isHidden() const {
	return hidden;
}

void PD_Character::enable(){
	enabled = true;
}

bool PD_Character::isEnabled() const {
	return enabled;
}

CharacterComponent::CharacterComponent(CharacterComponentDefinition * const _definition, Shader * _shader, Texture * _paletteTex, bool _flipped) :
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

glm::vec2 CharacterComponent::getOut(unsigned long int _index){
	return (out.size() > 0 ? out.at(_index) : glm::vec2(0,0)) - in;
}

CharacterLimbSolver::CharacterLimbSolver(glm::vec2 _pos) :
	IkChain_CCD(_pos)
{
}

void CharacterLimbSolver::addComponent(CharacterComponent * _component, float _weight){
	AnimationJoint * j = new AnimationJoint(_component->getOut(0));
	jointsLocal.back()->childTransform->addChild(_component);
	addJointToChain(j);
	components.push_back(_component);
}

CharacterState::CharacterState(Json::Value _json) :
	id(_json.get("id", "NO_ID").asString()),
	name(_json.get("name", "NO_NAME").asString()),
	conversation(_json.get("convo", "NO_CONVO").asString()),
	animation(_json.get("animation", "RANDOM").asString())
{
}

CharacterRenderer::CharacterRenderer(BulletWorld * _world, AssetCharacter * const _definition, Shader * _shader, Shader * _emoticonShder) :
	paletteTex(new PD_Palette(true)),
	animate(true),
	currentAnimation(nullptr),
	emote(nullptr),
	emoteTimeout(nullptr),
	talking(false),
	talk(nullptr),
	owner(nullptr)
{
	++paletteTex->referenceCount;
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

	pelvis = new CharacterComponent(pelvisDef, _shader, paletteTex, false);

	torso = new CharacterComponent(torsoDef, _shader, paletteTex, false);

	jaw = new CharacterComponent(jawDef, _shader, paletteTex, false);
	head = new CharacterComponent(headDef, _shader, paletteTex, false);

	nose = new CharacterComponent(noseDef, _shader, paletteTex, false);
	eyebrowL = new CharacterComponent(eyebrowLDef, _shader, paletteTex, false);
	eyebrowR = new CharacterComponent(eyebrowRDef, _shader, paletteTex, true);
	eyeL = new CharacterComponent(eyeLDef, _shader, paletteTex, false);
	eyeR = new CharacterComponent(eyeRDef, _shader, paletteTex, true);
	pupilL = new CharacterComponent(pupilLDef, _shader, paletteTex, false);
	pupilR = new CharacterComponent(pupilRDef, _shader, paletteTex, false);

	armR = new CharacterComponent(armRDef, _shader, paletteTex, true);
	forearmR = new CharacterComponent(forearmRDef, _shader, paletteTex, true);
	handR = new CharacterComponent(handRDef, _shader, paletteTex, true);

	armL = new CharacterComponent(armLDef, _shader, paletteTex, false);
	forearmL = new CharacterComponent(forearmLDef, _shader, paletteTex, false);
	handL = new CharacterComponent(handLDef, _shader, paletteTex, false);

	legR = new CharacterComponent(legRDef, _shader, paletteTex, true);
	forelegR = new CharacterComponent(forelegRDef, _shader, paletteTex, true);
	footR = new CharacterComponent(footRDef, _shader, paletteTex, true);

	legL = new CharacterComponent(legLDef, _shader, paletteTex, false);
	forelegL = new CharacterComponent(forelegLDef, _shader, paletteTex, false);
	footL = new CharacterComponent(footLDef, _shader, paletteTex, false);

	solverArmR = new CharacterLimbSolver(torso->getOut(1));
	solverArmL = new CharacterLimbSolver(torso->getOut(2));
	solverLegR = new CharacterLimbSolver(pelvis->getOut(1));
	solverLegL = new CharacterLimbSolver(pelvis->getOut(2));
	solverBod = new CharacterLimbSolver(glm::vec2(0));

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
	connect(head, eyeL);
	connect(head, eyeR);
	connect(eyeL, pupilL);
	connect(eyeR, pupilR);
	connect(head, eyebrowL);
	connect(head, eyebrowR);

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
	talk->tweens.push_back(new Tween<float>(0.075, head->mesh->textures.at(1)->height*0.2, Easing::kEASE_IN_OUT_CIRC));
	talk->tweens.push_back(new Tween<float>(0.075, -head->mesh->textures.at(1)->height*0.2, Easing::kEASE_IN_OUT_CIRC));
	talk->loopType = Animation<float>::LoopType::kLOOP;
	talk->hasStart = true;

	// blink
	eyeScale = glm::vec3(1);
	eyeAnim = new Animation<glm::vec3>(&eyeScale);
	eyeAnim->tweens.push_back(new Tween<glm::vec3>(sweet::NumberUtils::randomFloat(1.5, 5.5), glm::vec3(0,0,0), Easing::kLINEAR));
	eyeAnim->tweens.push_back(new Tween<glm::vec3>(sweet::NumberUtils::randomFloat(0.05, 0.15), glm::vec3(0,-1,0), Easing::kEASE_IN_CUBIC));
	eyeAnim->tweens.push_back(new Tween<glm::vec3>(sweet::NumberUtils::randomFloat(0.05, 0.15), glm::vec3(0,1,0), Easing::kEASE_OUT_CUBIC));
	eyeAnim->hasStart = true;

	// breathing
	pelvisScale = glm::vec3(1);
	pelvisAnim = new Animation<glm::vec3>(&pelvisScale);
	float torsoScaleDelta = sweet::NumberUtils::randomFloat(0.025, 0.1f);
	pelvisAnim->tweens.push_back(new Tween<glm::vec3>(sweet::NumberUtils::randomFloat(0.5, 2.15), glm::vec3(0,torsoScaleDelta,0), Easing::kEASE_IN_OUT_CUBIC));
	pelvisAnim->tweens.push_back(new Tween<glm::vec3>(sweet::NumberUtils::randomFloat(0.05, 0.5), glm::vec3(0,0,0), Easing::kLINEAR));
	pelvisAnim->tweens.push_back(new Tween<glm::vec3>(sweet::NumberUtils::randomFloat(0.5, 2.15), glm::vec3(0,-torsoScaleDelta,0), Easing::kEASE_IN_OUT_CUBIC));
	pelvisAnim->hasStart = true;

	emote = new Sprite(_emoticonShder);
	emote->setVisible(false);
	head->childTransform->addChild(emote);
	emote->firstParent()->scale(1/CHARACTER_SCALE);
}

CharacterRenderer::~CharacterRenderer(){
	paletteTex->decrementAndDelete();
	delete currentAnimation;
	delete emoteTimeout;
	delete talk;
	delete eyeAnim;
	delete pelvisAnim;
}

void CharacterRenderer::setAnimation(std::string _name) {
	if(_name == "RANDOM"){
		bool validAnimation = false;
		std::vector<PD_CharacterAnimationStep> anim;
		while(!validAnimation){
			auto it = PD_ResourceManager::characterAnimations.begin();
			int idx = sweet::NumberUtils::randomInt(0, PD_ResourceManager::characterAnimations.size() - 1);
			std::advance(it, idx);
			anim = it->second.steps;
			validAnimation = it->second.canBeRandom;
		}
		setAnimation(anim);
	}else {
		if(PD_ResourceManager::characterAnimations.find(_name) != PD_ResourceManager::characterAnimations.end()) {
			setAnimation(PD_ResourceManager::characterAnimations[_name].steps);
		}else {
			ST_LOG_ERROR("Animation " + _name + " does not exist");
		}
	}
}

void CharacterRenderer::setAnimation(std::vector<PD_CharacterAnimationStep> _steps) {
	delete currentAnimation;
	currentAnimation = new PD_CharacterAnimationSet(this);

	for(auto step : _steps) {
		currentAnimation->leftArm->tweens.push_back(new Tween<glm::vec2>(step.time, step.leftArm * solverArmL->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->rightArm->tweens.push_back(new Tween<glm::vec2>(step.time, step.rightArm * solverArmR->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->leftLeg->tweens.push_back(new Tween<glm::vec2>(step.time, step.leftLeg * solverLegL->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->rightLeg->tweens.push_back(new Tween<glm::vec2>(step.time, step.rightLeg * solverLegR->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->body->tweens.push_back(new Tween<glm::vec2>(step.time, step.body * solverBod->getChainLength(), Easing::getTypeByName(step.interpolation)));
		currentAnimation->translation->tweens.push_back(new Tween<glm::vec3>(step.time, step.translation  * solverBod->getChainLength(), Easing::getTypeByName(step.interpolation)));
	}

	if(_steps.size() > 0) {
		solverArmL->target = glm::vec2();
		solverArmR->target = glm::vec2();
		solverLegL->target = glm::vec2();
		solverLegR->target = glm::vec2();
		solverBod->target  = glm::vec2();
	}

	Step s;
	s.deltaTime = sweet::NumberUtils::randomFloat(0, 10);
	currentAnimation->leftArm->update(&s);
	currentAnimation->rightArm->update(&s);
	currentAnimation->leftLeg->update(&s);
	currentAnimation->rightLeg->update(&s);
	currentAnimation->body->update(&s);
	currentAnimation->translation->update(&s);
}

void CharacterRenderer::setEmote(std::string _id, float _duration) {
	delete emoteTimeout;
	emoteTimeout = nullptr;

	if(_id == "NONE") {
		emote->setVisible(false);
		return;
	}

	auto eDef = PD_ResourceManager::emotes[_id];
	emote->setVisible(true);
	emote->setSpriteSheet(eDef->spriteSheet, "main");
	auto b = head->mesh->calcBoundingBox();
	float height = b.height/CHARACTER_SCALE;
	float width  = b.width/CHARACTER_SCALE;
	emote->firstParent()->translate(width * eDef->offset.x, height * eDef->offset.y, 0.f, false);

	if(_duration > 0.f){
		emoteTimeout = new Timeout(_duration, [this](sweet::Event * _event){
			setEmoteNone();
		});
		emoteTimeout->start();
	}
}

void CharacterRenderer::setEmoteNone() {
	setEmote("NONE", -1.f);
}

void CharacterRenderer::connect(CharacterComponent * _from, CharacterComponent * _to, bool _behind){
	joints.push_back(_from->childTransform->addChild(_to));
	joints.back()->translate(
		_from->out.at(_from->connections.size()).x - _from->in.x,
		_from->out.at(_from->connections.size()).y - _from->in.y,
		0, false);
	_from->connections.push_back(_to);
}

void CharacterRenderer::setShader(Shader * _shader, bool _default) const {
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

void CharacterRenderer::update(Step * _step){
	if(Keyboard::getInstance().keyJustDown(GLFW_KEY_Y)){
		paletteTex->generateRandomTable();
		paletteTex->bufferData();
	}

	if(emoteTimeout != nullptr) {
		emoteTimeout->update(_step);
	}

	if(animate){
		if(currentAnimation != nullptr) {
			currentAnimation->update(_step);
			childTransform->translate(currentAnimation->translationVec, false);
		}
	}

	glm::vec3 v = head->firstParent()->getTranslationVector();
	if(talking){
		// talking
		talk->update(_step);
		head->firstParent()->translate(v.x, talkHeight, v.z, false);
	}else{
		head->firstParent()->translate(v.x, jaw->getOut(0).y, v.z, false);
	}

	Entity::update(_step);
}