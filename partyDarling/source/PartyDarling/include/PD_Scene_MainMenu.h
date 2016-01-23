#pragma once

#include <Scene.h>
#include <shader/Shader.h>
#include <UILayer.h>
#include <Font.h>

class ComponentShaderText;

class PD_Scene_MainMenu: public Scene {
public:		

	Shader * screenSurfaceShader;
	RenderSurface* screenSurface;
	StandardFrameBuffer* screenFBO;

	ComponentShaderText * textShaderJoinParty;
	ComponentShaderText * textShaderOptions;
	ComponentShaderText * textShaderLeave;

	UILayer uiLayer;
	Font* menuFont;
	explicit PD_Scene_MainMenu(Game * _game);
	~PD_Scene_MainMenu();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;
};
