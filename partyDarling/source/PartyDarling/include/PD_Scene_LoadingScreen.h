#pragma once

#include <Scene.h>
#include <shader/Shader.h>
#include <UILayer.h>
#include <Font.h>
#include <sweet/UI.h>

class ComponentShaderText;

class PD_Scene_LoadingScreen: public Scene {
public:		

	Shader * screenSurfaceShader;
	RenderSurface* screenSurface;
	StandardFrameBuffer* screenFBO;

	ComponentShaderText * textShader;

	TextLabel * loadingMessage;

	UILayer uiLayer;
	Font * menuFont;
	explicit PD_Scene_LoadingScreen(Game * _game);
	~PD_Scene_LoadingScreen();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;
};
