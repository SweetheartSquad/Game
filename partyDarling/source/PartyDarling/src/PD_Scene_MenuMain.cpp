#pragma once

#include "PD_Scene_MenuMain.h"
#include "PD_Scene_IntermissionSlideshow.h"
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

PD_Scene_MenuMain::PD_Scene_MenuMain(Game * _game) :
	Scene(_game),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, false)),
	screenSurface(new RenderSurface(screenSurfaceShader, false)),
	screenFBO(new StandardFrameBuffer(false)),
	textShader(new ComponentShaderText(true)),
	uiLayer(new UILayer(0,0,0,0)),
	menuFont(PD_ResourceManager::scenario->getFont("main-menu-font")->font)
{
	textShader->incrementReferenceCount();
	textShader->name = "PD_Scene_MenuMain text shader";

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);

	uiLayer->addMouseIndicator();

	// load game scene before?

	savedGame = sweet::FileUtils::fileExists("data/save.json");

	VerticalLinearLayout * mainContainer = new VerticalLinearLayout(uiLayer->world);
	uiLayer->addChild(mainContainer);
	mainContainer->horizontalAlignment = kCENTER;
	mainContainer->verticalAlignment = kTOP;
	mainContainer->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("main-menu-background-1")->texture);
	mainContainer->background->mesh->setScaleMode(GL_NEAREST);
	mainContainer->setRationalWidth(1.f, uiLayer);
	mainContainer->setRationalHeight(1.f, uiLayer);
	mainContainer->setPaddingTop(0.45f);
	mainContainer->setBackgroundColour(1, 1, 1, 1);

	VerticalLinearLayout * textContainer = new VerticalLinearLayout(uiLayer->world);
	textContainer->boxSizing = kCONTENT_BOX;
	mainContainer->addChild(textContainer);
	textContainer->horizontalAlignment = kCENTER;
	textContainer->verticalAlignment = kMIDDLE;
	textContainer->setRationalWidth(1.f, mainContainer);
	textContainer->setRationalHeight(1.f, mainContainer);
	textContainer->setMarginTop(0.13f);

	joinPartyText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(joinPartyText);
	joinPartyText->setRationalWidth(0.25f, textContainer);
	joinPartyText->setRationalHeight(0.2f, textContainer);
	joinPartyText->setMarginTop(0.05f);
	joinPartyText->setMarginBottom(0.05f);
	joinPartyText->enable();
	joinPartyText->horizontalAlignment = kCENTER;
	joinPartyText->setText(!savedGame ? "Join the party!" : "Reset");

	joinPartyText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	joinPartyText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);

	joinPartyText->eventManager->addEventListener("click", [this](sweet::Event * _event){
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
	});

	continueText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(continueText);
	continueText->setRationalWidth(0.3f, textContainer);
	continueText->setRationalHeight(0.2f, textContainer);
	continueText->setMarginTop(0.05f);
	continueText->setMarginBottom(0.05f);
	continueText->enable();
	continueText->horizontalAlignment = kCENTER;
	continueText->setText("Back to the party");
	if(!savedGame){
		continueText->disable();
	}

	continueText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	continueText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);

	continueText->eventManager->addEventListener("click", [this](sweet::Event * _event){
		if(game->scenes.count("game") == 0){
			game->scenes["intermission"] = new PD_Scene_IntermissionSlideshow(game, PD_Game::progressManager->plotPosition);
			game->switchScene("intermission", false);
		}else{
			game->switchScene("game", false);
			sweet::setCursorMode(GLFW_CURSOR_DISABLED);
			dynamic_cast<PD_Scene_Main *>(game->scenes["game"])->player->playerCameraController->alignMouse();
		}
	});

	optionsText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(optionsText);
	optionsText->setRationalWidth(0.15f, textContainer);
	optionsText->setRationalHeight(0.2f, textContainer);
	optionsText->setMarginTop(0.05f);
	optionsText->setMarginBottom(0.05f);

	optionsText->horizontalAlignment = kCENTER;
	optionsText->setText("Options");
	optionsText->enable();
	optionsText->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	optionsText->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	//optionsText->setMarginTop(0.05f);
	optionsText->eventManager->addEventListener("click", [this](sweet::Event * _event){
		game->switchScene("options", false);
	});

	callNightText = new PD_UI_Text(uiLayer->world, menuFont, textShader);
	textContainer->addChild(callNightText);
	callNightText->setRationalWidth(0.25f, textContainer);
	callNightText->setRationalHeight(0.2f, textContainer);
	callNightText->setMarginTop(0.05f);
	callNightText->setMarginBottom(0.05f);

	callNightText->eventManager->addEventListener("click", [this](sweet::Event * _event){
		game->exit();
	});

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

	confirmNewGame->btnConfirm->eventManager->addEventListener("click", [this](sweet::Event * _event){
		// erase the existing save file
		PD_Game::progressManager->eraseSave();
		PD_Game::progressManager->loadSave(nullptr, nullptr);

		// delete the existing game
		auto it = game->scenes.find("game");
		if(it != game->scenes.end()){
			delete it->second;
		}

		// switch to the intro
		game->scenes["intro"] = new PD_Scene_IntroSlideShow(game);
		game->switchScene("intro", false);
		continueText->enable();
		continueText->setTextColour(0.f, 0.f, 0.f);
		joinPartyText->setText("Reset");
		hideConfirmBox();
	});

	confirmNewGame->btnCancel->eventManager->addEventListener("click", [this](sweet::Event * _event){
		hideConfirmBox();
	});

	hideConfirmBox();

	mainContainer->invalidateLayout();

	float a = glm::degrees(atan((0.236 * uiLayer->getHeight()) / (0.871 * uiLayer->getWidth())));
	textContainer->firstParent()->rotate(a, 0, 0, 1, kOBJECT);

	// team and donate buttons
	// itch.io button
	{
		VerticalLinearLayout * vl = new VerticalLinearLayout(uiLayer->world);
		uiLayer->addChild(vl);
		vl->setRationalHeight(1.f, uiLayer);
		vl->setRationalWidth(1.f, uiLayer);
		vl->verticalAlignment = kBOTTOM;
		vl->horizontalAlignment = kRIGHT;
		vl->setMarginRight(0.01f);
		vl->marginBottom.setRationalSize(1.f, &vl->marginRight);

		NodeUI * sweetButt = new NodeUI(uiLayer->world);
		sweetButt->setPixelHeight(sweet::getDpi());
		sweetButt->setSquareWidth(1.f);
		vl->addChild(sweetButt);
		sweetButt->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SWEET-BUTT")->texture);
		sweetButt->setMouseEnabled(true);
		sweetButt->eventManager->addEventListener("click", [](sweet::Event * _event){
			ShellExecute(HWND(nullptr), L"open", L"http://www.sweetheartsquad.com", L"", L"", SW_SHOWNORMAL);
		});

		sweetButt->eventManager->addEventListener("mousein", [sweetButt](sweet::Event * _event){
			sweetButt->setBackgroundColour(1.25, 1.25, 1.25);
		});
		sweetButt->eventManager->addEventListener("mouseout", [sweetButt](sweet::Event * _event){
			sweetButt->setBackgroundColour(1, 1, 1);
		});

		NodeUI * donateButt = new NodeUI(uiLayer->world);
		donateButt->width.setRationalSize(1.f, &sweetButt->width);
		donateButt->setSquareHeight(0.5f);
		vl->addChild(donateButt);
		donateButt->background->mesh->setScaleMode(GL_NEAREST);
		donateButt->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DONATE-BUTT")->texture);
		donateButt->setMouseEnabled(true);
		donateButt->eventManager->addEventListener("click", [](sweet::Event * _event){
			ShellExecute(HWND(nullptr), L"open", L"https://sweetheartsquad.itch.io/party-darling-test-build/purchase", L"", L"", SW_SHOWNORMAL);
		});
		donateButt->eventManager->addEventListener("mousein", [donateButt](sweet::Event * _event){
			donateButt->setBackgroundColour(1.25, 1.25, 1.25);
		});
		donateButt->eventManager->addEventListener("mouseout", [donateButt](sweet::Event * _event){
			donateButt->setBackgroundColour(1, 1, 1);
		});
	}

	fadeNode = new NodeUI(uiLayer->world);
	fadeNode->setBackgroundColour(0,0,0,1);
	fadeNode->setRationalHeight(1.f, uiLayer);
	fadeNode->setRationalWidth(1.f, uiLayer);
	uiLayer->addChild(fadeNode);

	fadeTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		fadeNode->setBackgroundColour(0,0,0, 0.f);
	});
	fadeTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		fadeNode->setBackgroundColour(0,0,0, 1.f - p);
	});

	fadeTimeout->start();
	childTransform->addChild(fadeTimeout, false);
}

PD_Scene_MenuMain::~PD_Scene_MenuMain() {
	deleteChildTransform();
	delete uiLayer;

	delete screenSurface;
	delete screenSurfaceShader;
	delete screenFBO;

	textShader->decrementAndDelete();
}

void PD_Scene_MenuMain::showConfirmBox(){
	screen->setVisible(true);
	confirmNewGame->enable();

	joinPartyText->setMouseEnabled(false);
	continueText->setMouseEnabled(false);
	optionsText->setMouseEnabled(false);
	callNightText->setMouseEnabled(false);
}

void PD_Scene_MenuMain::hideConfirmBox(){
	screen->setVisible(false);
	confirmNewGame->disable();

	joinPartyText->setMouseEnabled(true);
	continueText->setMouseEnabled(continueText->isEnabled());
	optionsText->setMouseEnabled(true);
	callNightText->setMouseEnabled(true);
}

void PD_Scene_MenuMain::update(Step* _step) {
#ifdef DEBUG_CONTROLS
	// toggle debug draw
	if(keyboard->keyJustUp(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
		if(Transform::drawTransforms){
			uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
		}else{
			uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
		}
	}
#endif

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);
	uiLayer->update(_step);
}

void PD_Scene_MenuMain::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	screenFBO->resize(_renderOptions->viewPortDimensions.width, _renderOptions->viewPortDimensions.height);

	FrameBufferInterface::pushFbo(screenFBO);

	_renderOptions->setClearColour(0,0,0,0);
	_renderOptions->clear();

	Scene::render(_matrixStack, _renderOptions);
	uiLayer->render(_matrixStack, _renderOptions);

	FrameBufferInterface::popFbo();

	screenSurface->render(screenFBO->getTextureId());
}

void PD_Scene_MenuMain::load() {
	Scene::load();
	uiLayer->load();
	screenSurface->load();
	screenSurfaceShader->load();
	screenFBO->load();
}

void PD_Scene_MenuMain::unload() {
	uiLayer->unload();
	screenSurface->unload();
	screenSurfaceShader->unload();
	screenFBO->unload();
	Scene::unload();
}