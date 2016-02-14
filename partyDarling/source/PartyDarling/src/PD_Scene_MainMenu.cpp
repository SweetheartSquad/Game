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

class PD_UI_Text;

PD_Scene_MainMenu::PD_Scene_MainMenu(Game * _game) :
	Scene(_game),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)), 
	textShader(new ComponentShaderText(true)),
	uiLayer(0,0,0,0),
	menuFont(PD_ResourceManager::scenario->getFont("main-menu-font")->font)
{
	screenSurfaceShader->referenceCount++;
	screenFBO->referenceCount++;
	screenSurface->referenceCount++;

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);

	uiLayer.addMouseIndicator();

	VerticalLinearLayout * mainContainer = new VerticalLinearLayout(uiLayer.world);
	mainContainer->horizontalAlignment = kCENTER;
	mainContainer->verticalAlignment = kMIDDLE;
	mainContainer->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("main-menu-background-1")->texture);
	mainContainer->background->mesh->setScaleMode(GL_NEAREST);
	mainContainer->setRationalWidth(1.f, &uiLayer);
	mainContainer->setRationalHeight(1.f, &uiLayer);
	mainContainer->setVisible(true);
	mainContainer->setBackgroundColour(1, 1, 1, 1);

	VerticalLinearLayout * textContainer = new VerticalLinearLayout(uiLayer.world);
	mainContainer->addChild(textContainer);
	textContainer->horizontalAlignment = kCENTER;
	textContainer->verticalAlignment = kMIDDLE;
	textContainer->setVisible(true);
	textContainer->setMarginTop(0.25f);

	PD_UI_Text * joinPartyText = new PD_UI_Text(uiLayer.world, menuFont, textShader);
	textContainer->addChild(joinPartyText);
	joinPartyText->setWidth(400);
	joinPartyText->setHeight(60);

	joinPartyText->horizontalAlignment = kCENTER;
	joinPartyText->setText("Join the party");
	joinPartyText->setMouseEnabled(true);

	joinPartyText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	joinPartyText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);

	joinPartyText->onClick = [this](sweet::Event * _event){
		dynamic_cast<PD_Game *>(game)->showLoading("Loading...", 0);
		auto it = game->scenes.find("game");
		if(it == game->scenes.end() || it->second == nullptr){
			game->scenes["game"] = new PD_Scene_Main(dynamic_cast<PD_Game*>(game));
		}
		game->switchScene("game", false);
		
	};

	PD_UI_Text * optionsText = new PD_UI_Text(uiLayer.world, menuFont, textShader);
	textContainer->addChild(optionsText);
	optionsText->setWidth(400);
	optionsText->setHeight(60);

	optionsText->horizontalAlignment = kCENTER;
	optionsText->setText("Options");
	optionsText->setMouseEnabled(true);
	optionsText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	optionsText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	optionsText->setMarginTop(0.05f);
	optionsText->onClick = [this](sweet::Event * _event){
		game->switchScene("options", false);
	};

	PD_UI_Text * callNightText = new PD_UI_Text(uiLayer.world, menuFont, textShader);
	textContainer->addChild(callNightText);
	callNightText->setWidth(400);
	callNightText->setHeight(60);
	
	callNightText->onClick = [this](sweet::Event * _event){
		game->exit();		
	}; 

	callNightText->horizontalAlignment = kCENTER;

	callNightText->setText("Call it a night");
	callNightText->setMouseEnabled(true);

	callNightText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	callNightText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);

	callNightText->setMarginTop(0.05f);

	
	
	uiLayer.addChild(mainContainer);
	mainContainer->invalidateLayout();

	textContainer->firstParent()->rotate(10, 0, 0, 1, kOBJECT);
}

PD_Scene_MainMenu::~PD_Scene_MainMenu() {
	deleteChildTransform();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
	screenSurface->decrementAndDelete();
}

void PD_Scene_MainMenu::update(Step* _step) {

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);
	uiLayer.update(_step);
}

void PD_Scene_MainMenu::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
	

	FrameBufferInterface::pushFbo(screenFBO);
	
	_renderOptions->setClearColour(1,0,1,1);
	_renderOptions->clear();

	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
	
	FrameBufferInterface::popFbo();


	screenSurface->render(screenFBO->getTextureId());
}

void PD_Scene_MainMenu::load() {
	Scene::load();	
	uiLayer.load();
	screenSurface->load();
	screenSurfaceShader->load();
	screenFBO->load();
}

void PD_Scene_MainMenu::unload() {
	uiLayer.unload();
	screenSurface->unload();
	screenSurfaceShader->unload();
	screenFBO->unload();
	Scene::unload();	
}
