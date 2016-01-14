#pragma once

#include <Scene.h>

#include <UILayer.h>
#include <Sprite.h>
#include <shader/ComponentShaderText.h>

class PD_UI_YellingContest;

class PD_Scene_YellingContestTest : public Scene{
public:
	Sprite * mouseIndicator;
	ComponentShaderText * textShader;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;

	UILayer uiLayer;
	
	PD_UI_YellingContest * uiYellingContest;

	BulletWorld * bulletWorld;

	PD_Scene_YellingContestTest(Game * _game);
	~PD_Scene_YellingContestTest();
};