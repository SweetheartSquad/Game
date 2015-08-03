#pragma once

#include <Character.h>

#include <MeshInterface.h>
#include <MeshFactory.h>
#include <PD_ResourceManager.h>

Person::Person(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor):
	RoomObject(_world, _mesh, _anchor)
{
	setColliderAsBoundingBox();
	createRigidBody(25);
}

/*void Person::update(Step * _step){
	RoomObject::update(_step);

	//headLower->parents.at(0)->translate(0,0.01,0);
}*/


PersonComponent::PersonComponent(Texture * _tex, Texture * _paletteTex) :
	Sprite()
{
	mesh->pushTexture2D(_paletteTex);
	mesh->pushTexture2D(_tex);

	meshTransform->scale(_tex->width, _tex->height, 1);
	mesh->scaleModeMag = GL_NEAREST;
	mesh->scaleModeMin = GL_NEAREST;

	meshTransform->translate(_tex->width*0.5f, _tex->height*0.5f, 0);
}
PersonComponent::PersonComponent(Json::Value _json, Texture * _paletteTex) :
	Sprite()
{
	Texture * tex = new Texture("assets/textures/character components/" + _json["src"].asString(), true, true);
	tex->load();


	mesh->pushTexture2D(_paletteTex);
	mesh->pushTexture2D(tex);

					
					
	in = glm::vec2(_json["inX"].asFloat(), _json["inY"].asFloat());
	
	Json::Value outJson = _json["out"];
	for(auto i = 0; i < outJson.size(); ++i){
		out.push_back(glm::vec2(outJson[i]["x"].asFloat(), outJson[i]["y"].asFloat()));
	}

	meshTransform->scale(tex->width, tex->height, 1);

	meshTransform->translate(tex->width*0.5f, tex->height*0.5f, 0);
	meshTransform->translate(-in.x, -in.y, 0);

	mesh->scaleModeMag = GL_NEAREST;
	mesh->scaleModeMin = GL_NEAREST;
}

void PersonComponent::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	Sprite::render(_matrixStack, _renderOptions);
}


PersonRenderer::PersonRenderer(Texture * _paletteTex){
	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = FileUtils::voxReadFile("assets/skeletal_structure.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}else{
		 Json::Value charactersJson = root["characters"];
		 for(auto i = 0; i < charactersJson.size(); ++i) {
			Json::Value assets = charactersJson[i]["assets"];
			for(auto j = 0; j < assets.size(); ++j) {
				Json::Value asset = assets[j];

				if(asset["category"].asString() == "torso"){
					torso = new PersonComponent(asset, _paletteTex);
					
				}else if(asset["category"].asString() == "head"){
					Json::Value components = asset["components"];
					
					jaw = new PersonComponent(components[0], _paletteTex);
					head = new PersonComponent(components[1], _paletteTex);
				}else if(asset["category"].asString() == "arms"){
					Json::Value components = asset["components"][0]["components"];
					
					armL = new PersonComponent(components[0], _paletteTex);
					forearmL = new PersonComponent(components[1], _paletteTex);
					handL = new PersonComponent(components[2], _paletteTex);

					//components = asset["components"][1]["components"];
					
					armR = new PersonComponent(components[0], _paletteTex);
					forearmR = new PersonComponent(components[1], _paletteTex);
					handR = new PersonComponent(components[2], _paletteTex);

					armR->childTransform->scale(-1,1,1);
				}
			}
		}
	}
	
	childTransform->addChild(torso)->translate(0,1,0);
	connect(torso, jaw);
	connect(jaw, head);
	
	connect(torso, armL, true);
	connect(armL, forearmL);
	connect(forearmL, handL);
	
	connect(torso, armR, true);
	connect(armR, forearmR);
	connect(forearmR, handR);

	talkHeight = head->parents.at(0)->getTranslationVector().y;
	talk = new Animation<float>(&talkHeight);
	talk->tweens.push_back(new Tween<float>(0.1, head->mesh->textures.at(1)->height*0.4, Easing::kEASE_IN_OUT_CIRC));
	talk->tweens.push_back(new Tween<float>(0.1, -head->mesh->textures.at(1)->height*0.4, Easing::kEASE_IN_OUT_CIRC));
	talk->loopType = Animation<float>::LoopType::kLOOP;
	talk->hasStart = true;
}

void PersonRenderer::connect(PersonComponent * _from, PersonComponent * _to, bool _behind){
	_from->childTransform->addChild(_to)->translate(
		_from->out.at(_from->connections.size()).x - _from->in.x,
		_from->out.at(_from->connections.size()).y - _from->in.y,
		_behind ? 0.1 : -0.1); // use a small z translation to give some idea of layers until we implement a proper fix for z-fighting
	_from->connections.push_back(_to);
}

void PersonRenderer::setShader(Shader * _shader, bool _default){
	torso->setShader(_shader, _default);
	jaw->setShader(_shader, _default);
	head->setShader(_shader, _default);

	armL->setShader(_shader, _default);
	forearmL->setShader(_shader, _default);
	handL->setShader(_shader, _default);

	armR->setShader(_shader, _default);
	forearmR->setShader(_shader, _default);
	handR->setShader(_shader, _default);
}

void PersonRenderer::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	Entity::render(_matrixStack, _renderOptions);
}
void PersonRenderer::update(Step * _step){
	// talking
	talk->update(_step);
	glm::vec3 v = head->parents.at(0)->getTranslationVector();
	head->parents.at(0)->translate(v.x, talkHeight, v.z, false);


	Entity::update(_step);
}