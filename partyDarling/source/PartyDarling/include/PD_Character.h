#pragma once

#include <RoomObject.h>

#include <Sprite.h>

#include <Animation.h>
#include <json/json.h>




#include <IkChain.h>
#include <BulletWorld.h>
#include <NodeBulletBody.h>
#include "PD_CharacterAnimationSet.h"
#include <Timeout.h>

#define CHARACTER_SCALE 0.001f

class Scenario;
class PD_CharacterAnimationStep;
class Conversation;
class PersonRenderer;
class AssetCharacter;
class CharacterComponentDefinition;
class PD_Palette;

class PersonComponent : public Sprite{
public:
	glm::vec2 in;
	std::vector<glm::vec2> out;
	std::vector<PersonComponent *> connections;
	
	bool flipped;

	glm::vec2 getOut(unsigned long int _index);

	PersonComponent(CharacterComponentDefinition * const _definition, Shader * _shader, Texture * _paletteTex, bool _flipped);
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
	std::string animation;
	PersonState(Json::Value _json);
};

class PersonRenderer : public Entity{
public:
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
		* eyebrowL,
		* eyebrowR,
		* eyeL,
		* eyeR,
		* pupilL,
		* pupilR,

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

	PD_Palette * paletteTex;

	bool randomAnimations;
	bool animate;

	PD_CharacterAnimationSet * currentAnimation;
	Sprite * emote;

	Timeout * emoteTimeout;

	virtual void setAnimation(std::string _name);
	virtual void setAnimation(std::vector<PD_CharacterAnimationStep> _steps);
	virtual void setEmote(std::string _id, float _duration);
	virtual void setEmoteNone();

	PersonRenderer(BulletWorld * _world, AssetCharacter * const _definition, Shader * _shader, Shader * _emoticonShder);
	~PersonRenderer();

	void setShader(Shader * _shader, bool _default) const;


	// creates a parent-child relationship between two components
	// uses one of the connection slots on _from
	// if behind, _to moves backward; otherwise, _to moves forward
	void connect(PersonComponent * _from, PersonComponent * _to, bool _behind = false);

	virtual void update(Step * _step) override;
};

class Person : public RoomObject {
public:

	// the character's current state
	const PersonState * state;
	
	AssetCharacter * const definition;

	std::vector<std::string> items;

	Person(BulletWorld * _world, AssetCharacter * const _definition, MeshInterface * _mesh, Shader * _shader, Shader * _emoticonShder, Anchor_t _anchor = Anchor_t::GROUND);

	//virtual void update(Step * _step) override;

	virtual void setShader(Shader * _shader, bool _default) override;
	
	PersonRenderer * pr;

	static Person * createRandomPerson(Scenario * _scenario, BulletWorld * _world, Shader * _shader, Shader * _emoticonShder);
};