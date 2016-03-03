#pragma once

#include <Scene.h>
#include <BulletDebugDrawer.h>
#include <UILayer.h>
#include "PD_Assets.h"
#include <HorizontalLinearLayout.h>

class PD_CharacterAnimationStep;

class Keyframe : public NodeUI {
public:

	PD_CharacterAnimationStep * step;
	Person * character;
	
	explicit Keyframe(BulletWorld * _world, Person * _character);

};

class Effector : public NodeUI {
public:

	bool active;
	PersonLimbSolver * solver;

	explicit Effector(BulletWorld * _world, PersonLimbSolver * _solver);
	void update(Step * _step) override;
	void setPos(glm::vec3 _mpos, glm::vec3 _pos);

};

class PD_Scene_Animation : public Scene {
public:

	Person * character;

	ComponentShaderBase * characterShader;
	ComponentShaderBase * baseShader;
	UILayer * uiLayer;

	HorizontalLinearLayout * keyFrameLayout;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;

	Effector * leftArmEffector;
	Effector * rightArmEffector;
	Effector * leftLegEffector;
	Effector * rightLegEffector;
	Effector * bodyEffector;

	std::vector<PD_CharacterAnimationStep> testSteps;

	std::vector<Keyframe *> keyframes;
	Keyframe * currentKeyFrame;

	explicit PD_Scene_Animation(Game * _game);
	~PD_Scene_Animation();
	
	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	void updateEffectors() const;
	void writeToFile() const;
	void loadFromTestFile();
};


