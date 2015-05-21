#pragma once

#include <PD_Button.h>
#include <Font.h>
#include <shader\BaseComponentShader.h>
#include <shader\ShaderComponentText.h>
#include <shader/ShaderComponentTexture.h>

PD_Button::PD_Button(BulletWorld * _world, Scene * _scene) :
	NodeUI(_world, _scene),
	NodeBulletBody(_world)
{
	BaseComponentShader * textShader = new BaseComponentShader(true);
	textShader->addComponent(new ShaderComponentText(textShader));
	textShader->compileShader();

	BaseComponentShader * shader = new BaseComponentShader(true);
	shader->addComponent(new ShaderComponentTexture(shader));
	shader->compileShader();

	float size = 300.f;
	Font * font = new Font("../assets/arial.ttf", size, false);
	size = 1.f/size;
	normalLabel = new Label(font, textShader, shader, WrapMode::WORD_WRAP, 200);
	normalLabel->setText(L"normal");
	childTransform->addChild(normalLabel)->scale(size);
	
	downLabel = new Label(font, textShader, shader, WrapMode::WORD_WRAP, 200);
	downLabel->setText(L"down");
	childTransform->addChild(downLabel)->scale(size);
	
	overLabel = new Label(font, textShader, shader, WrapMode::WORD_WRAP, 200);
	overLabel->setText(L"over");
	childTransform->addChild(overLabel)->scale(size);

	setColliderAsBox(3,0.5,0.1);
	createRigidBody(0);
	childTransform->translate(-1.5, -0.25, 0);
	body->translate(btVector3(1.5, 0.25, 0));
}

void PD_Button::update(Step * _step){
	NodeUI::update(_step);

	if(isHovered){
		if(isDown){
			normalLabel->setVisible(false);
			overLabel->setVisible(false);
			downLabel->setVisible(true);
		}else{
			normalLabel->setVisible(false);
			overLabel->setVisible(true);
			downLabel->setVisible(false);
		}
	}else{
		normalLabel->setVisible(true);
		overLabel->setVisible(false);
		downLabel->setVisible(false);
	}
}