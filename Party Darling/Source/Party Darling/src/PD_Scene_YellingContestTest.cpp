#pragma once

#include <PD_Scene_YellingContestTest.h>
#include <PD_ResourceManager.h>
#include <PD_Game.h>
#include <RenderOptions.h>
#include <PD_UI_YellingContest.h>

#include <sweet/Input.h>

PD_Scene_YellingContestTest::PD_Scene_YellingContestTest(Game * _game) :
	Scene(_game),
	textShader(new ComponentShaderText(true)),
	uiLayer(0,0,0,0)
{
	bulletWorld = new BulletWorld();

	textShader->textComponent->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

	glm::uvec2 sd = sweet::getWindowDimensions();
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
	mouseIndicator->setShader(uiLayer.shader, true);

	Font * f = new Font("assets/engine basics/OpenSans-Regular.ttf", 42, true);
	uiYellingContest = new PD_UI_YellingContest(bulletWorld, f, textShader, uiLayer.shader, activeCamera);
	
	uiLayer.addChild(uiYellingContest);
	uiYellingContest->setRationalWidth(1.f, &uiLayer);
	uiYellingContest->setRationalHeight(1.f, &uiLayer);
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
	glm::uvec2 sd = sweet::getWindowDimensions();
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