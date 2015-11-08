#pragma once

#include <PD_UI_YellingContest.h>
#include <PD_InsultButton.h>

#include <Scene.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>

PD_UI_YellingContest::PD_UI_YellingContest(BulletWorld* _bulletWorld, Scene* _scene, Font * _font, Shader * _textShader) :
	VerticalLinearLayout(_bulletWorld, _scene),
	keyboard(_scene->keyboard),
	modeOffensive(true)
{
	setWidth(1);
	setHeight(1);
	verticalAlignment = kMIDDLE;
	horizontalAlignment = kCENTER;
	setBackgroundColour(0.5, 0.5, 0.5, 1);

	enemyBubble = new HorizontalLinearLayout(_bulletWorld, _scene);
	enemyBubble->verticalAlignment = kMIDDLE;
	enemyBubble->horizontalAlignment = kCENTER;
	enemyBubble->setWidth(0.25);
	enemyBubble->setHeight(0.25);
	enemyBubble->setBackgroundColour(0.682, 0.439, 0.670, 1);
	enemyBubbleText = new TextArea(world, scene, _font, _textShader, 0.9);
	enemyBubble->addChild(enemyBubbleText);

	playerBubble = new HorizontalLinearLayout(_bulletWorld, _scene);
	playerBubble->verticalAlignment = kMIDDLE;
	playerBubble->horizontalAlignment = kCENTER;
	playerBubble->setWidth(0.25);
	playerBubble->setHeight(0.25);
	playerBubble->setBackgroundColour(0.451, 0.439, 0.675, 1);
	playerBubbleText = new TextArea(world, scene, _font, _textShader, 0.9);
	playerBubble->addChild(playerBubbleText);

	VerticalLinearLayout * buttonLayout = new VerticalLinearLayout(_bulletWorld, _scene);
	buttonLayout->setWidth(0.4);
	buttonLayout->setHeight(1.0);

	pBubbleBtn1 = new PD_InsultButton(_bulletWorld, _scene, _font, _textShader);
	pBubbleBtn1->setWidth(1.0);
	pBubbleBtn1->setHeight(0.5);
	pBubbleBtn1->setBackgroundColour(0.569, 0.569, 0.733, 1);
	pBubbleBtn1->onClickFunction = [this](){insult(pBubbleBtn1->isEffective); };

	pBubbleBtn2 = new PD_InsultButton(_bulletWorld, _scene, _font, _textShader);
	pBubbleBtn2->setWidth(1.0);
	pBubbleBtn2->setHeight(0.5);
	pBubbleBtn2->setBackgroundColour(0.569, 0.569, 0.733, 1);
	pBubbleBtn2->onClickFunction = [this](){insult(pBubbleBtn2->isEffective); };
	buttonLayout->addChild(pBubbleBtn1);
	buttonLayout->addChild(pBubbleBtn2);

	playerBubble->addChild(buttonLayout);
	
	addChild(enemyBubble);
	addChild(playerBubble);

	setUIMode(false);
}

void PD_UI_YellingContest::update(Step * _step){
	if (keyboard->keyJustDown(GLFW_KEY_SPACE)){
		interject();
		
	}
	std::cout << pBubbleBtn1->bgShader << std::endl;
	VerticalLinearLayout::update(_step);
}

void PD_UI_YellingContest::interject(){
	setUIMode(true);
}

void PD_UI_YellingContest::setUIMode(bool _isOffensive){
	enemyBubble->setVisible(!_isOffensive);
	playerBubble->setVisible(_isOffensive);

	if (_isOffensive){
		setEnemyText();
	}
	else{
		setPlayerText();
	}
}

void PD_UI_YellingContest::setEnemyText(){
	// Generate full insult
	std::wstring insult = L"Well, you got a set of lizard legs!";

	enemyBubbleText->setText(insult);
}

void PD_UI_YellingContest::setPlayerText(){
	// Generate partial insult
	std::wstring insult = L"Your face is";
	// Generate insult word choices
	std::wstring opt1 = L"flat";
	std::wstring opt2 = L"lovely";

	playerBubbleText->setText(insult);
	pBubbleBtn1->setText(opt1);
	pBubbleBtn2->setText(opt2);
}

void PD_UI_YellingContest::insult(bool _isEffective){
	if (_isEffective){
		// next insult
	}
	else{
		//fail
		setUIMode(false);
	}
}