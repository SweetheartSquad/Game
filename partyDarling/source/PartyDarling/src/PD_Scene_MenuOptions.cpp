#pragma once

#include <PD_Scene_MenuOptions.h>
#include <PD_Scene_InstructionsSlideShow.h>
#include <PD_ResourceManager.h>
#include <VerticalLinearLayout.h>
#include <PD_UI_VolumeControl.h>
#include <RenderOptions.h>
#include <PD_UI_Text.h>
#include <Game.h>

PD_Scene_MenuOptions::PD_Scene_MenuOptions(Game* _game) :
	Scene(_game),
	textShader(new ComponentShaderText(false)),
	uiLayer(new UILayer(0,0,0,0)),
	menuFont(PD_ResourceManager::scenario->getFont("options-menu-font")->font)
{
	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);

	uiLayer->addMouseIndicator();

	VerticalLinearLayout * mainLayout = new VerticalLinearLayout(uiLayer->world);
	uiLayer->addChild(mainLayout);
	mainLayout->setRationalWidth(1.f, uiLayer);
	mainLayout->setRationalHeight(1.f, uiLayer);
	mainLayout->horizontalAlignment = kCENTER;
	mainLayout->verticalAlignment = kTOP;
	mainLayout->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("main-menu-background-1")->texture);
	mainLayout->setVisible(true);
	mainLayout->setPaddingTop(0.48f);
	mainLayout->setPaddingBottom(0.f);
	mainLayout->setPaddingLeft(0.05f);
	mainLayout->setBackgroundColour(1, 1, 1, 1);
	mainLayout->background->mesh->setScaleMode(GL_NEAREST);

	VerticalLinearLayout * subLayout = new VerticalLinearLayout(uiLayer->world);
	mainLayout->addChild(subLayout);
	subLayout->setRationalWidth(0.6f, mainLayout);
	subLayout->setRationalHeight(1.f, mainLayout);
	subLayout->horizontalAlignment = kCENTER;
	subLayout->verticalAlignment = kMIDDLE;
	subLayout->setPadding(0.05f);

	PD_UI_Text * backButton = new PD_UI_Text(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-font")->font, textShader);
	backButton->setText("Back");
	backButton->setMouseEnabled(true);
	backButton->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	backButton->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	backButton->setRationalWidth(1.0f, subLayout);
	backButton->setRationalHeight(0.15f, subLayout);
	backButton->horizontalAlignment = kCENTER;
	backButton->verticalAlignment = kMIDDLE;
	backButton->eventManager->addEventListener("click", [this](sweet::Event * _event){
		game->switchScene("menu", false);
	});

	PD_UI_Text * howToPlay = new PD_UI_Text(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-font")->font, textShader);
	howToPlay->setText("How To Play");
	howToPlay->setMouseEnabled(true);
	howToPlay->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	howToPlay->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	howToPlay->setRationalWidth(1.0f, subLayout);
	howToPlay->setRationalHeight(0.15f, subLayout);
	howToPlay->horizontalAlignment = kCENTER;
	howToPlay->verticalAlignment = kMIDDLE;
	howToPlay->eventManager->addEventListener("click", [this](sweet::Event * _event){
		game->scenes["howToPlay"] = new PD_Scene_InstructionsSlideShow(game);
		game->switchScene("howToPlay", false);
	});

	HorizontalLinearLayout * audioContainer = new HorizontalLinearLayout(uiLayer->world);
	audioContainer->horizontalAlignment = kCENTER;
	audioContainer->verticalAlignment = kTOP;
	audioContainer->setRationalWidth(1.f, subLayout);
	audioContainer->setRationalHeight(0.6f, subLayout);

	TextArea * audioLabel = new TextArea(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-font")->font, textShader);
	audioContainer->addChild(audioLabel);
	audioLabel->setText("Audio");
	audioLabel->setRationalWidth(0.4f, audioContainer);
	audioLabel->setRationalHeight(0.15f, audioContainer);
	audioLabel->setPaddingRight(0.05f);
	audioLabel->horizontalAlignment = kRIGHT;
	audioLabel->verticalAlignment = kMIDDLE;

	PD_UI_VolumeControl * volumeControl = new PD_UI_VolumeControl(uiLayer->world, textShader);
	audioContainer->addChild(volumeControl);
	volumeControl->setRationalWidth(0.6f, audioContainer);
	volumeControl->setRationalHeight(1.f, audioContainer);

	subLayout->addChild(backButton);
	subLayout->addChild(howToPlay);
	subLayout->addChild(audioContainer);;

	audioContainer->setPaddingTop(0.05f);

	float a = glm::degrees(atan((0.236 * uiLayer->getHeight()) / (0.871 * uiLayer->getWidth())));
	subLayout->firstParent()->rotate(a, 0, 0, 1, kOBJECT);
}

PD_Scene_MenuOptions::~PD_Scene_MenuOptions() {
	deleteChildTransform();
	delete uiLayer;
	delete textShader;
}

void PD_Scene_MenuOptions::update(Step* _step) {
	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);
	uiLayer->update(_step);
}

void PD_Scene_MenuOptions::load() {
	Scene::load();
	uiLayer->load();
}

void PD_Scene_MenuOptions::unload() {
	Scene::unload();
	uiLayer->unload();
}

void PD_Scene_MenuOptions::render(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOptions) {
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer->render(_matrixStack, _renderOptions);
}