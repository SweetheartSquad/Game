#pragma once

#include "PD_Scene_MainMenu.h"
#include <StandardFrameBuffer.h>
#include <RenderSurface.h>
#include <RenderOptions.h>
#include <Game.h>
#include <PD_ResourceManager.h>
#include <VerticalLinearLayout.h>
#include <shader/ShaderComponentText.h>
#include <shader/ComponentShaderText.h>
#include <TextArea.h>
#include <Game.h>
#include <PD_UI_Text.h>
#include <PD_Scene_Main.h>
#include <PD_Scene_IntroSlideShow.h>
#include <PD_UI_ConfirmNewGame.h>

PD_Scene_MainMenu::PD_Scene_MainMenu(Game * _game) :
	Scene(_game),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, false)),
	screenSurface(new RenderSurface(screenSurfaceShader, false)),
	screenFBO(new StandardFrameBuffer(false)), 
	textShader(new ComponentShaderText(false)),
	uiLayer(new UILayer(0,0,0,0)),
	menuFont(PD_ResourceManager::scenario->getFont("main-menu-font")->font)
{
	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);

	uiLayer->addMouseIndicator();

	// load game scene before?

	savedGame = false;

	VerticalLinearLayout * mainContainer = new VerticalLinearLayout(uiLayer->world);
	uiLayer->addChild(mainContainer);
	mainContainer->horizontalAlignment = kCENTER;
	mainContainer->verticalAlignment = kTOP;
	mainContainer->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("main-menu-background-1")->texture);
	mainContainer->background->mesh->setScaleMode(GL_NEAREST);
	mainContainer->setRationalWidth(1.f, uiLayer);
	mainContainer->setRationalHeight(1.f, uiLayer);
	mainContainer->setVisible(true);
	mainContainer->setPaddingTop(0.45f);
	mainContainer->setBackgroundColour(1, 1, 1, 1);

	VerticalLinearLayout * textContainer = new VerticalLinearLayout(uiLayer->world);
	textContainer->boxSizing = kCONTENT_BOX;
	mainContainer->addChild(textContainer);
	textContainer->horizontalAlignment = kCENTER;
	textContainer->verticalAlignment = kMIDDLE;
	textContainer->setRationalWidth(1.f, mainContainer);
	textContainer->setRationalHeight(1.f, mainContainer);
	textContainer->setVisible(true);
	textContainer->setMarginTop(0.13f);

	joinPartyText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(joinPartyText);
	joinPartyText->setRationalWidth(1.f, textContainer);
	joinPartyText->setRationalHeight(0.2f, textContainer);
	joinPartyText->setMarginTop(0.05f);
	joinPartyText->setMarginBottom(0.05f);
	joinPartyText->enable();
	joinPartyText->horizontalAlignment = kCENTER;
	joinPartyText->setText(!savedGame ? "Join the party!" : "Reset");

	joinPartyText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	joinPartyText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);

	joinPartyText->onClick = [this](sweet::Event * _event){
		if(savedGame){
			// warning
			showConfirmBox();
		}else{
			savedGame = true;
			game->scenes["intro"] = new PD_Scene_IntroSlideShow(game);
			game->switchScene("intro", false);
			continueText->enable();
			continueText->setTextColour(0.f, 0.f, 0.f);
			joinPartyText->setText("Reset");
			uiLayer->invalidateLayout();
		}
	};

	continueText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(continueText);
	continueText->setRationalWidth(1.f, textContainer);
	continueText->setRationalHeight(0.2f, textContainer);
	continueText->setMarginTop(0.05f);
	continueText->setMarginBottom(0.05f);
	continueText->enable();
	continueText->horizontalAlignment = kCENTER;
	continueText->setText("Continue");
	if(!savedGame){
		continueText->disable();
	}

	continueText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	continueText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);

	continueText->onClick = [this](sweet::Event * _event){
		auto it = game->scenes.find("game");
		if(it == game->scenes.end()){
			// load game from save file
		}else{
			game->switchScene("game", false);
		}
	};

	optionsText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(optionsText);
	optionsText->setRationalWidth(1.f, textContainer);
	optionsText->setRationalHeight(0.2f, textContainer);
	optionsText->setMarginTop(0.05f);
	optionsText->setMarginBottom(0.05f);

	optionsText->horizontalAlignment = kCENTER;
	optionsText->setText("Options");
	optionsText->enable();
	optionsText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	optionsText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	//optionsText->setMarginTop(0.05f);
	optionsText->onClick = [this](sweet::Event * _event){
		game->switchScene("options", false);
	};

	callNightText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(callNightText);
	callNightText->setRationalWidth(1.f, textContainer);
	callNightText->setRationalHeight(0.2f, textContainer);
	callNightText->setMarginTop(0.05f);
	callNightText->setMarginBottom(0.05f);

	callNightText->onClick = [this](sweet::Event * _event){
		game->exit();		
	}; 

	callNightText->horizontalAlignment = kCENTER;

	callNightText->setText("Call it a night");
	callNightText->enable();

	callNightText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	callNightText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);

	screen = new NodeUI(uiLayer->world);
	uiLayer->addChild(screen);
	screen->setRationalWidth(1.f, uiLayer);
	screen->setRationalHeight(1.f, uiLayer);
	screen->setBackgroundColour(0, 0, 0, 0.5f);

	confirmNewGame = new PD_UI_ConfirmNewGame(uiLayer->world, textShader);
	uiLayer->addChild(confirmNewGame);
	confirmNewGame->setRationalHeight(1.f, uiLayer);
	confirmNewGame->setRationalWidth(1.f, uiLayer);
	confirmNewGame->setMargin(0.25f);
	confirmNewGame->setPadding(0.1f);
	confirmNewGame->setBackgroundColour(1.f, 1.f, 1.f);

	confirmNewGame->btnConfirm->onClick = [this](sweet::Event * _event){
		game->scenes["intro"] = new PD_Scene_IntroSlideShow(game);
		game->switchScene("intro", false);
		continueText->enable();
		continueText->setTextColour(0.f, 0.f, 0.f);
		joinPartyText->setText("Reset");
		hideConfirmBox();
	};

	
	confirmNewGame->btnCancel->onClick = [this](sweet::Event * _event){
		hideConfirmBox();
	};

	hideConfirmBox();
	
	mainContainer->invalidateLayout();

	float a = glm::degrees(atan((0.236 * uiLayer->getHeight()) / (0.871 * uiLayer->getWidth())));
	textContainer->firstParent()->rotate(a, 0, 0, 1, kOBJECT);
}

PD_Scene_MainMenu::~PD_Scene_MainMenu() {
	deleteChildTransform();
	delete uiLayer;
	
	delete screenSurface;
	delete screenSurfaceShader;
	delete screenFBO;
	delete textShader;
}

void PD_Scene_MainMenu::showConfirmBox(){
	screen->setVisible(true);
	confirmNewGame->enable();

	joinPartyText->setMouseEnabled(false);
	continueText->setMouseEnabled(false);
	optionsText->setMouseEnabled(false);
	callNightText->setMouseEnabled(false);
}

void PD_Scene_MainMenu::hideConfirmBox(){
	screen->setVisible(false);
	confirmNewGame->disable();

	joinPartyText->setMouseEnabled(true);
	continueText->setMouseEnabled(continueText->isEnabled());
	optionsText->setMouseEnabled(true);
	callNightText->setMouseEnabled(true);
}

void PD_Scene_MainMenu::update(Step* _step) {

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);
	uiLayer->update(_step);
}

void PD_Scene_MainMenu::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
	

	FrameBufferInterface::pushFbo(screenFBO);
	
	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();

	Scene::render(_matrixStack, _renderOptions);
	uiLayer->render(_matrixStack, _renderOptions);
	
	FrameBufferInterface::popFbo();


	screenSurface->render(screenFBO->getTextureId());
}

void PD_Scene_MainMenu::load() {
	Scene::load();	
	uiLayer->load();
	screenSurface->load();
	screenSurfaceShader->load();
	screenFBO->load();
}

void PD_Scene_MainMenu::unload() {
	uiLayer->unload();
	screenSurface->unload();
	screenSurfaceShader->unload();
	screenFBO->unload();
	Scene::unload();	
}
