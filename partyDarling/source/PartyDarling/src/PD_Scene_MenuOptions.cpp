#pragma once

#include <PD_Scene_MenuOptions.h>
#include <PD_ResourceManager.h>
#include <VerticalLinearLayout.h>
#include <PD_UI_VolumeControl.h>
#include <RenderOptions.h>
#include <PD_UI_Text.h>
#include <Game.h>

PD_Scene_MenuOptions::PD_Scene_MenuOptions(Game* _game) :
	Scene(_game),
	textShader(new ComponentShaderText(true)),
	uiLayer(new UILayer(0,0,0,0)),
	menuFont(PD_ResourceManager::scenario->getFont("options-menu-font")->font)
{
	++textShader->referenceCount;

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);

	uiLayer->addMouseIndicator();

	PD_UI_VolumeControl * volumeControl = new PD_UI_VolumeControl(uiLayer->world, textShader);

	VerticalLinearLayout * mainLayout = new VerticalLinearLayout(uiLayer->world);
	uiLayer->addChild(mainLayout);
	mainLayout->setRationalWidth(1.f, uiLayer);
	mainLayout->setRationalHeight(1.f, uiLayer);
	mainLayout->horizontalAlignment = kCENTER;
	mainLayout->verticalAlignment = kMIDDLE;
	mainLayout->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("main-menu-background-1")->texture);
	mainLayout->setVisible(true);
	mainLayout->setPaddingTop(0.48f);
	mainLayout->setBackgroundColour(1, 1, 1, 1);
	mainLayout->background->mesh->setScaleMode(GL_NEAREST);

	VerticalLinearLayout * subLayout = new VerticalLinearLayout(uiLayer->world);
	mainLayout->addChild(subLayout);
	subLayout->boxSizing = kCONTENT_BOX;
	subLayout->setRationalWidth(1.0f, mainLayout);
	subLayout->setRationalHeight(0.5f, mainLayout);
	subLayout->horizontalAlignment = kCENTER;
	subLayout->verticalAlignment = kMIDDLE;
	subLayout->setMarginTop(0.05f);
	subLayout->setBackgroundColour(1.f, 0, 0, 0.5f);
	
	PD_UI_Text * controls = new PD_UI_Text(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-font")->font, textShader);
	controls->setText("Controls");
	controls->setMouseEnabled(true);
	controls->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	controls->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	controls->setRationalWidth(1.0f, subLayout);
	controls->setRationalHeight(0.15f, subLayout);
	controls->horizontalAlignment = kCENTER;
	controls->verticalAlignment = kMIDDLE;
	controls->onClick = [this](sweet::Event * _event){
		game->switchScene("controls", false);
	};

	TextArea * audioLabel = new TextArea(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-font")->font, textShader);
	audioLabel->setText("Audio");
	audioLabel->setRationalWidth(1.0f, subLayout);
	audioLabel->setRationalHeight(0.15f, subLayout);
	audioLabel->horizontalAlignment = kCENTER;
	audioLabel->verticalAlignment = kMIDDLE;

	PD_UI_Text * rsvp = new PD_UI_Text(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-font")->font, textShader);
	rsvp->setText("RSVP");
	rsvp->setMouseEnabled(true);
	rsvp->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	rsvp->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	rsvp->setRationalWidth(1.0f, subLayout);
	rsvp->setRationalHeight(0.15f, subLayout);
	rsvp->horizontalAlignment = kCENTER;
	rsvp->verticalAlignment = kMIDDLE;
	rsvp->onClick = [this](sweet::Event * _event){
		game->switchScene("menu", false);
	};

	subLayout->addChild(audioLabel);
	subLayout->addChild(volumeControl);
	subLayout->addChild(rsvp);
	
	volumeControl->setRationalWidth(0.35f, mainLayout);
	volumeControl->setRationalHeight(0.65f, mainLayout);

	subLayout->firstParent()->rotate(9, 0, 0, 1, kOBJECT);
}

PD_Scene_MenuOptions::~PD_Scene_MenuOptions() {
	deleteChildTransform();
	delete uiLayer;

	textShader->decrementAndDelete();
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
