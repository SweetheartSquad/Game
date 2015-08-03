#pragma once

#include <RoomObject.h>

#include <Sprite.h>

#include <Animation.h>
#include <json/json.h>

class Person: public RoomObject {
public:
	Person(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor = Anchor_t::GROUND);

	//virtual void update(Step * _step) override;

	//virtual void setShader(Shader * _shader, bool _default) override;
};




class PersonComponent : public Sprite{
public:
	glm::vec2 in;
	std::vector<glm::vec2> out;
	std::vector<PersonComponent *> connections;
	
	PersonComponent(Texture * _tex, Texture * _paletteTex);
	PersonComponent(Json::Value _json, Texture * _paletteTex);
	virtual void render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};
class PersonRenderer : public Entity{
public:
	float talkHeight;
	Animation<float> * talk;
	PersonComponent
		* torso,
		* jaw,
		* head;

	PersonRenderer(Texture * _paletteTex);

	void setShader(Shader * _shader, bool _default);


	
	void connect(PersonComponent * _from, PersonComponent * _to);

	
	virtual void update(Step * _step) override;
	virtual void render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};