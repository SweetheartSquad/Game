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

	playerBubble = new HorizontalLinearLayout(_bulletWorld, _scene);
	playerBubble->verticalAlignment = kMIDDLE;
	playerBubble->horizontalAlignment = kCENTER;
	playerBubble->setWidth(0.25);
	playerBubble->setHeight(0.25);
	playerBubble->setBackgroundColour(0.451, 0.439, 0.675, 1);
	playerBubbleText = new TextArea(world, scene, _font, _textShader, 0.9);
	VerticalLinearLayout * buttonLayout = new VerticalLinearLayout(_bulletWorld, _scene);
	playerBubbleBtn1 = new PD_InsultButton(_bulletWorld, _scene);
	playerBubbleBtn1->onClickFunction = [this](){insult(playerBubbleBtn1->isEffective); };
	playerBubbleBtn2 = new PD_InsultButton(_bulletWorld, _scene);
	playerBubbleBtn2->onClickFunction = [this](){insult(playerBubbleBtn1->isEffective); };

	

	
	
	addChild(enemyBubble);
	addChild(playerBubble);

	setUIMode(false);
}

void PD_UI_YellingContest::update(Step * _step){
	if (keyboard->keyJustDown(GLFW_KEY_SPACE)){
		interject();
		
	}
	std::cout << playerBubbleBtn2->isHovered << std::endl;
	VerticalLinearLayout::update(_step);
}

void PD_UI_YellingContest::interject(){
	setUIMode(true);
}

void PD_UI_YellingContest::setUIMode(bool _isOffensive){
	enemyBubble->setVisible(!_isOffensive);
	playerBubble->setVisible(_isOffensive);
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