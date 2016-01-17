#pragma once

#include <RoomObject.h>

#include <Sprite.h>

#include <Animation.h>
#include <json/json.h>




#include <IkChain.h>
#include <BulletWorld.h>
#include <NodeBulletBody.h>

class Conversation;
class PersonRenderer;

class PersonButt : public NodeBulletBody{
public:
	PersonRenderer * person;
	PersonButt(BulletWorld * _world, PersonRenderer * _person);
};


class PersonComponent : public Sprite{
public:
	glm::vec2 in;
	std::vector<glm::vec2> out;
	std::vector<PersonComponent *> connections;
	
	bool flipped;

	static std::vector<PersonComponent *> getComponentsFromJson(Json::Value _json, Texture * _paletteTex, bool _flipped = false);

	glm::vec2 getOut(unsigned long int _index);
private:
	PersonComponent(Json::Value _json, Texture * _paletteTex, bool _flipped);
};

class PersonLimbSolver : public IkChain_CCD{
public:
	std::vector<PersonComponent *> components;

	PersonLimbSolver(glm::vec2 _pos);

	void addComponent(PersonComponent * _component, float _weight = 1.f);
};

class PersonState : public Node{
public:
	// name of this state
	std::string name;
	// reference to the conversation which will launch
	// if the character is spoken to while in this state
	std::string conversation;
	PersonState(Json::Value _json);
};

class PersonRenderer : public Entity{
public:
	PersonButt * butt;
	PersonState * state;
	float timer;

	PersonLimbSolver * solverArmR;
	PersonLimbSolver * solverArmL;
	PersonLimbSolver * solverLegR;
	PersonLimbSolver * solverLegL;
	PersonLimbSolver * solverBod;
	std::vector<PersonLimbSolver *> solvers;
	PersonLimbSolver * currentSolver;

	float talkHeight;
	Animation<float> * talk;
	std::vector<Transform *> joints;
	PersonComponent
		* pelvis,
		* torso,
		* jaw,
		* head,
		* nose,
		* eyes,
		* armL,
		* armR,
		* forearmL,
		* forearmR,
		* handL,
		* handR,
		
		* legL,
		* legR,
		* forelegL,
		* forelegR,
		* footL,
		* footR;

	Texture * paletteTex;
	PersonRenderer(BulletWorld * _world);
	~PersonRenderer();

	void setShader(Shader * _shader, bool _default);


	// creates a parent-child relationship between two components
	// uses one of the connection slots on _from
	// if behind, _to moves backward; otherwise, _to moves forward
	void connect(PersonComponent * _from, PersonComponent * _to, bool _behind = false);

	virtual void update(Step * _step) override;
};

class Person : public RoomObject {
public:
	Person(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor = Anchor_t::GROUND);

	//virtual void update(Step * _step) override;

	//virtual void setShader(Shader * _shader, bool _default) override;
	
	PersonRenderer * pr;
};