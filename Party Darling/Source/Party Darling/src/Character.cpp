#pragma once

#include <Character.h>

#include <MeshInterface.h>
#include <MeshFactory.h>
#include <PD_ResourceManager.h>
#include <NumberUtils.h>
#include <TextureColourTable.h>

#include <sweet/Input.h>

PersonButt::PersonButt(BulletWorld * _world, PersonRenderer * _person) :
	NodeBulletBody(_world),
	person(_person)
{

}

Person::Person(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor):
	RoomObject(_world, _mesh, _anchor),
	pr(new PersonRenderer(_world))
{
	setColliderAsCapsule(0.5f,1.f);
	createRigidBody(25);
	body->setAngularFactor(btVector3(0,1,0)); // prevent from falling over

	childTransform->addChild(pr)->scale(0.001);
}

std::vector<PersonComponent *> PersonComponent::getComponentsFromJson(Json::Value _json, Texture * _paletteTex, bool _flipped){
	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = FileUtils::readFile("assets/"+_json["id"].asString());
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}

	std::vector<PersonComponent *> res;

	if(root.isMember("components")){
		Json::Value componentsJson = root["components"];
		for(auto i = 0; i < componentsJson.size(); ++i) {
			res.push_back(new PersonComponent(componentsJson[i], _paletteTex, _flipped));
		}
	}else{
		res.push_back(new PersonComponent(root, _paletteTex, _flipped));
	}

	return res;
}

PersonComponent::PersonComponent(Json::Value _json, Texture * _paletteTex, bool _flipped) :
	Sprite(),
	flipped(_flipped)
{
	// get texture
	Texture * tex = new Texture("assets/textures/character components/" + _json["src"].asString(), true, true);
	tex->load();

	// apply palette + texture
	mesh->pushTexture2D(_paletteTex);
	mesh->pushTexture2D(tex);
	
	// parse coordinates
	in = glm::vec2(_json["in"][0].asFloat(), _json["in"][1].asFloat());
	Json::Value outJson = _json["out"];
	for(auto i = 0; i < outJson.size(); ++i){
		out.push_back(glm::vec2(outJson[i][0].asFloat(), outJson[i][1].asFloat()));
	}

	// handle flipping
	if(flipped){
		meshTransform->scale(-1, 1, 1);
		in.x = 1 - in.x;
		for(glm::vec2 & o : out){
			o.x = 1 - o.x;
		}
	}
	
	// multiply percentage coordinates by width/height to corresponding to specific texture
	in.x *= tex->width;
	in.y *= tex->height;
	for(glm::vec2 & o : out){
		o.x *= tex->width;
		o.y *= tex->height;
	}
	
	// scale and translate the mesh into position
	meshTransform->scale(tex->width, tex->height, 1);
	meshTransform->translate(tex->width*0.5f, tex->height*0.5f, 0);
	meshTransform->translate(-in.x, -in.y, 0);

	mesh->scaleModeMag = GL_NEAREST;
	mesh->scaleModeMin = GL_NEAREST;
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

PersonState::PersonState(){
	conversation = PD_ResourceManager::scenario->conversations["intro"];
}

PersonRenderer::PersonRenderer(BulletWorld * _world) :
	paletteTex(new TextureColourTable(false)),
	timer(0)
{
	paletteTex->load();

	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = FileUtils::readFile("assets/skeletal_structure.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}else{
		 Json::Value charactersJson = root["characters"];
		 for(auto i = 0; i < charactersJson.size(); ++i) {
			Json::Value assets = charactersJson[i]["assets"];
			for(auto j = 0; j < assets.size(); ++j) {
				Json::Value asset = assets[j];

				if(asset["category"].asString() == "pelvis"){
					pelvis = PersonComponent::getComponentsFromJson(asset, paletteTex).at(0);
				}else if(asset["category"].asString() == "torso"){
					torso = PersonComponent::getComponentsFromJson(asset, paletteTex).at(0);
				}else if(asset["category"].asString() == "head"){
					std::vector<PersonComponent *> components = PersonComponent::getComponentsFromJson(asset, paletteTex);
					jaw = components.at(0);
					head = components.at(1);
					nose = components.at(2);
					eyes = components.at(3);
				}else if(asset["category"].asString() == "arms"){
					std::vector<PersonComponent *> components = PersonComponent::getComponentsFromJson(asset, paletteTex);
					armR = components.at(0);
					forearmR = components.at(1);
					handR = components.at(2);

					components = PersonComponent::getComponentsFromJson(asset, paletteTex, true);
					armL = components.at(0);
					forearmL = components.at(1);
					handL = components.at(2);
				}else if(asset["category"] == "legs"){
					std::vector<PersonComponent *> components = PersonComponent::getComponentsFromJson(asset, paletteTex);
					legR = components.at(0);
					forelegR = components.at(1);
					footR = components.at(2);

					components = PersonComponent::getComponentsFromJson(asset, paletteTex, true);
					legL = components.at(0);
					forelegL = components.at(1);
					footL = components.at(2);
				}
			}
		}
	}
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
	connect(head, eyes);
	
	// attach the arms/legs to the spine
	solverBod->jointsLocal.at(1)->addJoint(solverArmR);
	solverBod->jointsLocal.at(1)->addJoint(solverArmL);
	solverBod->jointsLocal.at(0)->addJoint(solverLegR);
	solverBod->jointsLocal.at(0)->addJoint(solverLegL);
	childTransform->addChild(solverBod);

	
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


	butt = new PersonButt(_world, this);
	childTransform->addChild(butt);
	butt->setColliderAsBox((torso->getOut(2).x - torso->getOut(1).x) * 0.001f, solverBod->getChainLength() * 0.001f, 0.001f); // TODO: make this scale factor not hard-coded
	butt->createRigidBody(0);
}

PersonRenderer::~PersonRenderer(){
	delete paletteTex;
}

void PersonRenderer::connect(PersonComponent * _from, PersonComponent * _to, bool _behind){
	joints.push_back(_from->childTransform->addChild(_to));
	joints.back()->translate(
		_from->out.at(_from->connections.size()).x - _from->in.x,
		_from->out.at(_from->connections.size()).y - _from->in.y,
		0); // use a small z translation to give some idea of layers until we implement a proper fix for z-fighting
	_from->connections.push_back(_to);
}

void PersonRenderer::setShader(Shader * _shader, bool _default){
	pelvis->setShader(_shader, _default);
	torso->setShader(_shader, _default);
	jaw->setShader(_shader, _default);
	head->setShader(_shader, _default);
	
	nose->setShader(_shader, _default);
	eyes->setShader(_shader, _default);

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
	

	timer += _step->deltaTime;

	if(timer > 1){
		timer = 0;
		float l;

		l = solverArmR->getChainLength();
		solverArmR->target.x = sweet::NumberUtils::randomFloat(-solverArmR->getChainLength(), 0);
		solverArmR->target.y = sweet::NumberUtils::randomFloat(-solverArmR->getChainLength(), solverArmR->getChainLength());
		
		l = solverArmL->getChainLength();
		solverArmL->target.x = sweet::NumberUtils::randomFloat(solverArmL->getChainLength(), 0);
		solverArmL->target.y = sweet::NumberUtils::randomFloat(-solverArmR->getChainLength(), solverArmL->getChainLength());
		
		l = solverLegR->getChainLength();
		solverLegR->target.x = sweet::NumberUtils::randomFloat(-solverLegL->getChainLength()*0.5, 0);
		solverLegR->target.y = sweet::NumberUtils::randomFloat(-solverLegR->getChainLength(), -solverLegL->getChainLength()*0.8);
		
		l = solverLegL->getChainLength();
		solverLegL->target.x = sweet::NumberUtils::randomFloat(0, solverLegR->getChainLength()*0.5);
		solverLegL->target.y = sweet::NumberUtils::randomFloat(-solverLegL->getChainLength(), -solverLegL->getChainLength()*0.8);
		
		l = solverBod->getChainLength();
		solverBod->target.x = sweet::NumberUtils::randomFloat(-solverBod->getChainLength()*0.5, solverBod->getChainLength()*0.5);
		solverBod->target.y = sweet::NumberUtils::randomFloat(solverBod->getChainLength()*0.95, solverBod->getChainLength());

		/*solverArmL->target = glm::vec2(solverArmL->getChainLength(), 0);
		solverLegR->target = glm::vec2(0, -solverLegR->getChainLength());
		solverLegL->target = glm::vec2(0, -solverLegL->getChainLength());
		solverBod->target = glm::vec2(0, solverBod->getChainLength());
		
		for(unsigned long int s = 1; s < solvers.size(); ++s){
			float l = solvers.at(s)->getChainLength();
			solvers.at(s)->target.x = sweet::NumberUtils::randomFloat(-l, l);
			solvers.at(s)->target.y = sweet::NumberUtils::randomFloat(-l, l);
		}*/
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
	
	// talking
	talk->update(_step);
	glm::vec3 v = head->parents.at(0)->getTranslationVector();
	head->parents.at(0)->translate(v.x, talkHeight, v.z, false);

	Entity::update(_step);
}