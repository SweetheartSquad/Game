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
#include <NumberUtils.h>
#include "Room.h"
#include <OpenALSound.h>

#define CHARACTER_SCALE 0.00085f

class Scenario;
class PD_CharacterAnimationStep;
class Conversation;
class CharacterRenderer;
class AssetCharacter;
class CharacterComponentDefinition;
class PD_Palette;
class PD_DissStats;

class CharacterComponent : public Sprite{
public:
	glm::vec2 in;
	std::vector<glm::vec2> out;
	std::vector<CharacterComponent *> connections;
	
	bool flipped;

	glm::vec2 getOut(unsigned long int _index);

	CharacterComponent(CharacterComponentDefinition * const _definition, Shader * _shader, Texture * _paletteTex, bool _flipped);
};

class CharacterLimbSolver : public IkChain_CCD{
public:
	std::vector<CharacterComponent *> components;

	CharacterLimbSolver(glm::vec2 _pos);

	void addComponent(CharacterComponent * _component, float _weight = 1.f);
};

class CharacterState : public Node{
public:
	// id of this state
	std::string id;
	// name of this state
	std::string name;
	// reference to the conversation which will launch
	// if the character is spoken to while in this state
	std::string conversation;
	std::string animation;
	CharacterState(Json::Value _json);
};

class CharacterRenderer : public Entity{
public:
	bool talking;

	CharacterLimbSolver * solverArmR;
	CharacterLimbSolver * solverArmL;
	CharacterLimbSolver * solverLegR;
	CharacterLimbSolver * solverLegL;
	CharacterLimbSolver * solverBod;
	std::vector<CharacterLimbSolver *> solvers;
	CharacterLimbSolver * currentSolver;

	float talkHeight;
	Animation<float> * talk;

	glm::vec3 eyeScale;
	Animation<glm::vec3> * eyeAnim;

	glm::vec3 pelvisScale;
	Animation<glm::vec3> * pelvisAnim;

	std::vector<Transform *> joints;
	CharacterComponent
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

	bool animate;

	PD_CharacterAnimationSet * currentAnimation;
	Sprite * emote;

	Timeout * emoteTimeout;

	virtual void setAnimation(std::string _name);
	virtual void setAnimation(std::vector<PD_CharacterAnimationStep> _steps);
	virtual void setEmote(std::string _id, float _duration);
	virtual void setEmoteNone();

	CharacterRenderer(BulletWorld * _world, AssetCharacter * const _definition, Shader * _shader, Shader * _emoticonShder);
	~CharacterRenderer();

	void setShader(Shader * _shader, bool _default) const;


	// creates a parent-child relationship between two components
	// uses one of the connection slots on _from
	// if behind, _to moves backward; otherwise, _to moves forward
	void connect(CharacterComponent * _from, CharacterComponent * _to, bool _behind = false);

	virtual void update(Step * _step) override;
};

class PD_Character : public RoomObject {
private:
	static unsigned long int numRandomCharacters;
public:
	sweet::ShuffleVector<std::string> incidentalLineWon;
	sweet::ShuffleVector<std::string> incidentalLineLost;
	sweet::ShuffleVector<std::string> incidentalLineNormal;

	PD_DissStats * dissStats;
	
	// if false, you haven't had a diss battle with this character yet
	// if true, you can't have a diss battle them again
	bool dissedAt;
	// if true, this character won their contest against you
	// if false, this character lost their contest against you
	bool wonDissBattle;

	// the character's current state
	const CharacterState * state;
	
	// The room the character is in
	Room * room;

	AssetCharacter * const definition;

	OpenAL_Sound * voice; 

	std::vector<std::string> items;

	PD_Character(BulletWorld * _world, AssetCharacter * const _definition, MeshInterface * _mesh, Shader * _shader, Shader * _emoticonShder, Anchor_t _anchor = Anchor_t::GROUND);
	~PD_Character();

	//virtual void update(Step * _step) override;

	virtual void setShader(Shader * _shader, bool _default) override;
	virtual void update(Step * _step) override;

	void disable();
	void enable();
	bool isEnabled();

	
	CharacterRenderer * pr;

	static PD_Character * createRandomPD_Character(Scenario * _scenario, BulletWorld * _world, Shader * _shader, Shader * _emoticonShder);
	static Json::Value genRandomComponents();

private:
	bool enabled;
};