#pragma once

#include <PD_Button.h>
#include <Font.h>
#include <shader\BaseComponentShader.h>
#include <shader\ShaderComponentText.h>

PD_Button::PD_Button(BulletWorld * _world, Scene * _scene) :
	NodeUI(_world, _scene),
	NodeBulletBody(_world)
{
	BaseComponentShader * textShader = new BaseComponentShader(true);
	textShader->addComponent(new ShaderComponentText(textShader));
	textShader->compileShader();

	Font * font = new Font("../assets/arial.ttf", 100, false);
	normalLabel = new Label(font, textShader, WrapMode::WORD_WRAP, 200);
	normalLabel->setText(L"normal");
	childTransform->addChild(normalLabel)->scale(0.01, 0.01, 0.01);
	
	downLabel = new Label(font, textShader, WrapMode::WORD_WRAP, 200);
	downLabel->setText(L"down");
	childTransform->addChild(downLabel)->scale(0.01, 0.01, 0.01);
	
	overLabel = new Label(font, textShader, WrapMode::WORD_WRAP, 200);
	overLabel->setText(L"over");
	childTransform->addChild(overLabel)->scale(0.01, 0.01, 0.01);

	setColliderAsBox(10,10,10);
	createRigidBody(0);
}

void PD_Button::renderDefault(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	normalLabel->render(_matrixStack, _renderOptions);
}

void PD_Button::renderDown(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	downLabel->render(_matrixStack, _renderOptions);
}

void PD_Button::renderOver(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	overLabel->render(_matrixStack, _renderOptions);
}