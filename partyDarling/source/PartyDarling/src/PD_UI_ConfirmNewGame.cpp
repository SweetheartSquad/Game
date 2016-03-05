#pragma once

#include <PD_UI_ConfirmNewGame.h>
#include <PD_UI_Text.h>
#include <PD_ResourceManager.h>
#include <OpenAlSound.h>

PD_UI_ConfirmNewGame::PD_UI_ConfirmNewGame(BulletWorld * _world, ComponentShaderText * _textShader) :
	VerticalLinearLayout(_world)
{
	verticalAlignment = kMIDDLE;
	
	TextArea * text = new TextArea(_world, PD_ResourceManager::scenario->getFont("options-menu-sub-font")->font, _textShader);
	addChild(text);
	text->setWrapMode(kWORD);
	text->verticalAlignment = kMIDDLE;
	text->horizontalAlignment = kCENTER;
	text->setRationalWidth(1.f, this);
	text->setRationalHeight(0.5f, this);
	text->setText("Are you sure you want to reset the game? You will lose all of your progress.");

	HorizontalLinearLayout * buttonsContainer = new HorizontalLinearLayout(_world);
	addChild(buttonsContainer);
	buttonsContainer->setRationalWidth(1.f, this);
	buttonsContainer->setRationalHeight(0.5f, this);

	btnConfirm = new PD_UI_Text(_world, PD_ResourceManager::scenario->getFont("main-menu-font")->font, _textShader);
	buttonsContainer->addChild(btnConfirm);
	btnConfirm->setRationalWidth(0.5f, buttonsContainer);
	btnConfirm->setRationalHeight(1.f, buttonsContainer);
	btnConfirm->verticalAlignment = kMIDDLE;
	btnConfirm->horizontalAlignment = kCENTER;

	btnConfirm->setText("Yes");
	btnConfirm->setMouseEnabled(true);
	btnConfirm->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	btnConfirm->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	//optionsText->setMarginTop(0.05f);

	btnCancel = new PD_UI_Text(_world, PD_ResourceManager::scenario->getFont("main-menu-font")->font, _textShader);
	buttonsContainer->addChild(btnCancel);
	btnCancel->setRationalWidth(0.5f, buttonsContainer);
	btnCancel->setRationalHeight(1.f, buttonsContainer);
	btnCancel->verticalAlignment = kMIDDLE;
	btnCancel->horizontalAlignment = kCENTER;

	btnCancel->setText("No");
	btnCancel->setMouseEnabled(true);
	btnCancel->setDownColour(147.f/255.f, 25.f/255.f, 45.f/255.f);
	btnCancel->setOverColour(188.f/255.f, 60.f/255.f, 61.f/255.f);
	//optionsText->setMarginTop(0.05f);
	
	invalidateLayout();
}

void PD_UI_ConfirmNewGame::enable(){
	setVisible(true);
	btnConfirm->setMouseEnabled(true);
	btnCancel->setMouseEnabled(true);
}

void PD_UI_ConfirmNewGame::disable(){
	setVisible(false);
	btnConfirm->setMouseEnabled(false);
	btnCancel->setMouseEnabled(false);
}