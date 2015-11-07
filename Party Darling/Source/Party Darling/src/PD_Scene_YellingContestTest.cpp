#pragma once

#include <PD_Scene_YellingContestTest.h>
#include <PD_ResourceManager.h>
#include <System.h>
#include <PD_Game.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <PD_UI_YellingContest.h>

PD_Scene_YellingContestTest::PD_Scene_YellingContestTest(Game * _game) :
	Scene(_game),
	textShader(new ComponentShaderText(true)),
	uiLayer(this, 0,0,0,0)
{
	bulletWorld = new BulletWorld();

	textShader->textComponent->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

	glm::uvec2 sd = sweet::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);

	// mouse cursor
	mouseIndicator = new Sprite();
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
	mouseIndicator->setShader(uiLayer.shader, true);

	Font * f = new Font("assets/engine basics/OpenSans-Regular.ttf", 12, true);
	uiYellingContest = new PD_UI_YellingContest(bulletWorld, this, f, textShader);
	
	uiLayer.addChild(uiYellingContest);
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
	
	if (keyboard->keyJustUp(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
		if (uiLayer.bulletDebugDrawer != nullptr){
			uiLayer.bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
		}else{
			uiLayer.bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
		}
	}



	Scene::update(_step);

	// update ui stuff
	glm::uvec2 sd = sweet::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	uiLayer.update(_step);

	glm::vec3 sp = activeCamera->worldToScreen(glm::vec3(0,0,0), sd);
	if(sp.z < 0){
		sp.z = activeCamera->farClip * 2;
	}
	mouseIndicator->parents.at(0)->translate(mouse->mouseX(), mouse->mouseY(), 0, false);
}

void PD_Scene_YellingContestTest::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	clear();
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