#pragma once

#include <PD_Scene_YellingContestTest.h>
#include <PD_ResourceManager.h>
#include <System.h>
#include <PD_Game.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <RenderOptions.h>

PD_Scene_YellingContestTest::PD_Scene_YellingContestTest(Game * _game) :
	Scene(_game),
	textShader(new ComponentShaderText(true)),
	uiLayer(this, 0,0,0,0)
{
	textShader->textComponent->setColor(glm::vec3(0.0f, 0.0f, 0.0f));

	glm::uvec2 sd = sweet::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);

	// mouse cursor
	mouseIndicator = new Sprite(uiLayer.shader);
	uiLayer.childTransform->addChild(mouseIndicator);
	mouseIndicator->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CURSOR")->texture);
	mouseIndicator->parents.at(0)->scale(32, 32, 1);
	mouseIndicator->mesh->scaleModeMag = GL_NEAREST;
	mouseIndicator->mesh->scaleModeMin = GL_NEAREST;

	for(unsigned long int i = 0; i < mouseIndicator->mesh->vertices.size(); ++i){
		mouseIndicator->mesh->vertices[i].x += 0.5f;
		mouseIndicator->mesh->vertices[i].y -= 0.5f;
	}
	mouseIndicator->mesh->dirty = true;
}

PD_Scene_YellingContestTest::~PD_Scene_YellingContestTest(){
	deleteChildTransform();
	textShader->safeDelete();
}


void PD_Scene_YellingContestTest::update(Step * _step){
	if(keyboard->keyJustDown(GLFW_KEY_F12)){
		game->toggleFullScreen();
	}

	// debug controls
	if(keyboard->keyJustDown(GLFW_KEY_1)){
		cycleCamera();
	}
	
	Scene::update(_step);

	// update ui stuff
	glm::uvec2 sd = sweet::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	uiLayer.update(_step);

	mouseIndicator->parents.at(0)->translate(mouse->mouseX(), mouse->mouseY(), 0, false);
}

void PD_Scene_YellingContestTest::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_Scene_YellingContestTest::load(){
	Scene::load();	
	uiLayer.load();
}

void PD_Scene_YellingContestTest::unload(){
	uiLayer.unload();
	Scene::unload();	
}