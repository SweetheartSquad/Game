#pragma once

#include <Scene.h>
#include <shader/Shader.h>
#include <UILayer.h>
#include <Font.h>
#include <sweet/UI.h>
#include <PhraseGenerator.h>

class ComponentShaderText;

#define LOADING_PHASES 4
#define TIME_PER_MESSAGE 1.0

class PD_PhraseGenerator_Loading : public PhraseGenerator{
public:
	PD_PhraseGenerator_Loading();
	std::string getMessage(unsigned long int _phase);
	void reset();
};

class PD_Scene_LoadingScreen: public Scene {
public:
	double lastMessageTime;
	unsigned long int lastMessagePhase;
	PD_PhraseGenerator_Loading loadingMessages;

	Shader * screenSurfaceShader;
	RenderSurface* screenSurface;
	StandardFrameBuffer* screenFBO;

	ComponentShaderText * textShader;

	TextLabel * loadingMessage;
	float loadingPercent;
	Slider * loadingSlider;

	UILayer * uiLayer;
	Font * menuFont;
	explicit PD_Scene_LoadingScreen(Game * _game);
	~PD_Scene_LoadingScreen();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	virtual void load() override;
	virtual void unload() override;

	void updateProgress(float _progress);
};
