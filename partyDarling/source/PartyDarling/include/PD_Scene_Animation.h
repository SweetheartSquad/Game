#pragma once

#include <Scene.h>
#include <BulletDebugDrawer.h>
#include <UILayer.h>
#include "PD_Assets.h"

class PD_AnimationScene : public Scene {
public:

	Person * character;

	ComponentShaderBase * characterShader;
	ComponentShaderBase * baseShader;
	UILayer uiLayer;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	explicit PD_AnimationScene(Game * _game);
	~PD_AnimationScene();
};


