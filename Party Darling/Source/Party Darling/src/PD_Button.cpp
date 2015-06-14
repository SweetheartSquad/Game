#pragma once

#include <PD_Button.h>
#include <Font.h>
#include <shader\ComponentShaderBase.h>
#include <shader\ShaderComponentText.h>
#include <shader/ShaderComponentTexture.h>
#include <MeshFactory.h>

PD_Button::PD_Button(BulletWorld * _world, Scene * _scene) :
	NodeUI(_world, _scene),
	NodeBulletBody(_world),
	MeshEntity(MeshFactory::getPlaneMesh())
{
	ComponentShaderBase * textShader = new ComponentShaderBase(true);
	textShader->addComponent(new ShaderComponentText(textShader));
	textShader->compileShader();

	ComponentShaderBase * shader = new ComponentShaderBase(true);
	shader->addComponent(new ShaderComponentTexture(shader));
	shader->compileShader();

	float size = 30.f;
	Font * font = new Font("../assets/arial.ttf", size, false);
	//size = 1.f/size;
	normalLabel = new Label(_world, _scene, font, textShader, shader, WrapMode::WORD_WRAP, 200);
	normalLabel->setText(L"normal");
	childTransform->addChild(normalLabel);//->scale(size/64.f);
	
	downLabel = new Label(_world, _scene, font, textShader, shader, WrapMode::WORD_WRAP, 200);
	downLabel->setText(L"down");
	childTransform->addChild(downLabel);//->scale(size/64.f);
	
	overLabel = new Label(_world, _scene, font, textShader, shader, WrapMode::WORD_WRAP, 200);
	overLabel->setText(L"over");
	childTransform->addChild(overLabel);//->scale(size/64.f);

	setColliderAsBox(200, size, 0.1);
	createRigidBody(0);
	childTransform->translate(-1.5, -0.25, 0);
	body->translate(btVector3(1.5, 0.25, 0));
}

float PD_Button::getMeasuredWidth(){
	return 200.f;
}

float PD_Button::getMeasuredHeight(){
	return 30.f;
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