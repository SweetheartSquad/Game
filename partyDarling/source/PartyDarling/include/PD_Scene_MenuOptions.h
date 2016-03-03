#pragma once

#include <Scene.h>
#include <shader/Shader.h>
#include <UILayer.h>
#include <Font.h>
#include <shader/ComponentShaderText.h>

class PD_Scene_MenuOptions : public Scene {
	public:		

	ComponentShaderText * textShader;

	UILayer * uiLayer;
	Font * menuFont;

	explicit PD_Scene_MenuOptions(Game * _game);
	~PD_Scene_MenuOptions();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;
};
