#pragma once

#include <PD_UI_YellingContest.h>
#include <PD_InsultButton.h>

#include <Scene.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>

#include <NumberUtils.h>

PD_UI_YellingContest::PD_UI_YellingContest(BulletWorld* _bulletWorld, Font * _font, Shader * _textShader) :
	VerticalLinearLayout(_bulletWorld),
	keyboard(&Keyboard::getInstance()),
	modeOffensive(true)
{
	setRationalWidth(1.0);
	setRationalHeight(1.0);
	verticalAlignment = kMIDDLE;
	horizontalAlignment = kCENTER;
	setBackgroundColour(0.5, 0.5, 0.5, 1);

	enemyBubble = new HorizontalLinearLayout(_bulletWorld);
	enemyBubble->verticalAlignment = kMIDDLE;
	enemyBubble->horizontalAlignment = kCENTER;
	enemyBubble->setRationalWidth(0.25f);
	enemyBubble->setRationalHeight(0.25f);
	enemyBubble->setBackgroundColour(0.682f, 0.439f, 0.670f, 1);
	enemyBubbleText = new TextArea(world, _font, _textShader, 0.9f);
	enemyBubbleText->setRationalWidth(1.0f);
	enemyBubbleText->setRationalHeight(1.0f);
	enemyBubbleText->horizontalAlignment = kCENTER;
	enemyBubbleText->verticalAlignment = kMIDDLE;
	enemyBubble->addChild(enemyBubbleText);

	playerBubble = new HorizontalLinearLayout(_bulletWorld);
	playerBubble->verticalAlignment = kMIDDLE;
	playerBubble->horizontalAlignment = kCENTER;
	playerBubble->setRationalWidth(0.25f);
	playerBubble->setRationalHeight(0.25f);
	playerBubble->setBackgroundColour(0.451f, 0.439f, 0.675f, 1);
	playerBubbleText = new TextArea(world, _font, _textShader, 0.9f);
	playerBubbleText->setRationalWidth(0.6f);
	playerBubbleText->setRationalHeight(1.0f);
	playerBubbleText->horizontalAlignment = kCENTER;
	playerBubbleText->verticalAlignment = kMIDDLE;
	playerBubble->addChild(playerBubbleText);

	VerticalLinearLayout * buttonLayout = new VerticalLinearLayout(_bulletWorld);
	buttonLayout->setRationalWidth(0.4f);
	buttonLayout->setRationalHeight(1.0f);

	pBubbleBtn1 = new PD_InsultButton(_bulletWorld, _font, _textShader);
	pBubbleBtn1->setRationalWidth(1.0f);
	pBubbleBtn1->setRationalHeight(0.5f);
	pBubbleBtn1->setBackgroundColour(0.569f, 0.569f, 0.733f, 1);
	pBubbleBtn1->onClickFunction = [this](){insult(pBubbleBtn1->isEffective); };

	pBubbleBtn2 = new PD_InsultButton(_bulletWorld, _font, _textShader);
	pBubbleBtn2->setRationalWidth(1.0f);
	pBubbleBtn2->setRationalHeight(0.5f);
	pBubbleBtn2->setBackgroundColour(0.569f, 0.569f, 0.733f, 1);
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

	if (!_isOffensive){
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
	std::wstring optE = L"flat";
	std::wstring optI = L"lovely";

	bool btn1E = sweet::NumberUtils::randomInt(0, 1);

	playerBubbleText->setText(insult);

	pBubbleBtn1->isEffective = btn1E;
	pBubbleBtn2->isEffective = !btn1E;

	pBubbleBtn1->setText(btn1E ? optE : optI);
	pBubbleBtn2->setText(!btn1E ? optE : optI);
	
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