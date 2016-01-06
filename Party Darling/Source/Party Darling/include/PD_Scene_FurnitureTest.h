#pragma once

#include <Scene.h>
#include <Sprite.h>
#include <UILayer.h>
#include <shader/ComponentShaderBase.h>

class PD_Scene_FurnitureTest : public Scene{
public:
	ComponentShaderBase * shader;
	Sprite * mouseIndicator;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	Transform * createChair();

	void createFurniture(Json::Value v);
	void createComponent(Json::Value c);

	UILayer uiLayer;

	PD_Scene_FurnitureTest(Game * _game);
	~PD_Scene_FurnitureTest();

};