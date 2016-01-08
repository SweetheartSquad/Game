#pragma once

#include <PD_UI_YellingContest.h>
#include <PD_InsultButton.h>

#include <Scene.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>
#include <Easing.h>
#include <Sprite.h>
#include <PD_ResourceManager.h>
#include <NumberUtils.h>
#include <shader/ComponentShaderText.h>

PD_UI_YellingContest_TextArea::PD_UI_YellingContest_TextArea(BulletWorld * _world, Font * _font, Shader * _textShader, Shader * _shader) :
	TextArea(_world, _font, _textShader, 0.9f),
	highlightTextShader(new ComponentShaderText(true))
{
	highlightTextShader->setColor(0, 1.f, 0);
}
PD_UI_YellingContest_TextArea::~PD_UI_YellingContest_TextArea(){
}

void PD_UI_YellingContest_TextArea::setText(std::wstring _text){
	TextArea::setText(_text);

	for (auto label : usedLines) {
		for (int i = 0; i < label->usedGlyphs.size(); ++i){
			highlightPunctuation(label->usedGlyphs.at(i));
		}
	}
}
void PD_UI_YellingContest_TextArea::highlightPunctuation(UIGlyph * _glyph){
	switch (_glyph->character){
		case L'.':
		case L'?':
		case L'!':
		case L',':
			if(_glyph->shader != nullptr){
				_glyph->shader->decrementAndDelete();
			}
			_glyph->shader = highlightTextShader;
			++_glyph->shader->referenceCount;
			_glyph->shader->load();
	}
}

void PD_UI_YellingContest_TextArea::update(Step * _step){

	TextArea::update(_step);
}

PD_UI_YellingContest::PD_UI_YellingContest(BulletWorld* _bulletWorld, Font * _font, Shader * _textShader, Shader * _shader, Camera * _cam) :
	VerticalLinearLayout(_bulletWorld),
	keyboard(&Keyboard::getInstance()),
	modeOffensive(true),
	cursorDelayLength(0.25f),
	cursorDelayDuration(0.f),
	moveCursor(false),
	glyphIdx(0),
	enemyCursor(new Sprite(_shader)),
	cam(_cam)
{
	enemyCursor->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-CURSOR")->texture);
	enemyCursor->childTransform->scale(20.f);
	childTransform->addChild(enemyCursor);

	// move the cusrosr's mesh up so that the origin is aligned with the top
	for (unsigned long int i = 0; i < enemyCursor->mesh->vertices.size(); ++i){
		enemyCursor->mesh->vertices.at(i).y -= 0.5f;
	}

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
	//enemyBubbleText = new PD_UI_YellingContest_TextArea(world, _font, _textShader, 0.9f);//
	enemyBubbleText = new PD_UI_YellingContest_TextArea(world, _font, _textShader, _shader);
	//enemyBubbleText->setWrapMode(kWORD);
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
	pBubbleBtn1->eventManager.addEventListener("mousedown", [this](sweet::Event * _event){insult(pBubbleBtn1->isEffective); });

	pBubbleBtn2 = new PD_InsultButton(_bulletWorld, _font, _textShader);
	pBubbleBtn2->setRationalWidth(1.0f);
	pBubbleBtn2->setRationalHeight(0.5f);
	pBubbleBtn2->setBackgroundColour(0.569f, 0.569f, 0.733f, 1);
	pBubbleBtn2->eventManager.addEventListener("mousedown", [this](sweet::Event * _event){insult(pBubbleBtn2->isEffective); });
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
	VerticalLinearLayout::update(_step);

	if(moveCursor){
		// At least two glyphs in stack
		if(glyphIdx+1 < glyphs.size()){
			glm::vec3 screenPos1 = glyphs.at(glyphIdx)->childTransform->getWorldPos();//cam->worldToScreen(glyphs.at(glyphIdx)->childTransform->getWorldPos(), sweet::getWindowDimensions());
			glm::vec3 screenPos2 = glyphs.at(glyphIdx+1)->childTransform->getWorldPos();
			
			cursorDelayDuration += _step->getDeltaTime();

			if(cursorDelayDuration < cursorDelayLength){
				float dx = screenPos2.x - screenPos1.x;
				float dy = screenPos2.y - screenPos1.y;
				float tx = Easing::linear(cursorDelayDuration, screenPos1.x, dx, cursorDelayLength);
				float ty = Easing::linear(cursorDelayDuration, screenPos1.y, dy, cursorDelayLength);
				enemyCursor->childTransform->translate(tx, ty, 0, false);
			}else{
				++glyphIdx;
				cursorDelayDuration = 0;
			}
			
		}else{
			moveCursor = false;
		}
	}
}

void PD_UI_YellingContest::interject(){
	bool isPunctuation = false;
	if(glyphs.size() > 0){
		UIGlyph * g = glyphs.at(glyphIdx);
	
		switch (g->character){
			case L'.':
			case L'?':
			case L'!':
			case L',':
				isPunctuation = true;
		}
	}else{
		isPunctuation = true;
	}
	setUIMode(isPunctuation);
}

void PD_UI_YellingContest::setUIMode(bool _isOffensive){
	enemyBubble->setVisible(!_isOffensive);
	enemyCursor->setVisible(!_isOffensive);

	playerBubble->setVisible(_isOffensive);

	enemyBubble->setMouseEnabled(!_isOffensive);
	playerBubble->setMouseEnabled(_isOffensive);
	
	if (!_isOffensive){
		setEnemyText();
	}
	else{
		setPlayerText();
	}
	invalidateLayout();
}

void PD_UI_YellingContest::setEnemyText(){
	// Generate full insult
	std::wstring insult = L"Well, you got a set of lizard legs!";

	enemyBubbleText->setText(insult);

	glyphIdx = 0;
	moveCursor = true;
	glyphs.clear();
	for (auto label : enemyBubbleText->usedLines) {
		for (int i = 0; i < label->usedGlyphs.size(); ++i){
			glyphs.push_back(label->usedGlyphs.at(i));
		}
	}
	//enemyBubbleText->tickerIn(0.05f);
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
		setPlayerText();
	}
	else{
		//fail
		setUIMode(false);
	}
}