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
	TextArea(_world, _font, _textShader, 0.9f)
{
	//highlightTextShader->setColor(0, 1.f, 0);
}
PD_UI_YellingContest_TextArea::~PD_UI_YellingContest_TextArea(){
}

void PD_UI_YellingContest_TextArea::setText(std::wstring _text){
	TextArea::setText(_text);
	
}


void PD_UI_YellingContest_TextArea::update(Step * _step){

	TextArea::update(_step);
}

PD_UI_YellingContest::PD_UI_YellingContest(BulletWorld* _bulletWorld, Font * _font, Shader * _textShader, Shader * _shader, Camera * _cam) :
	VerticalLinearLayout(_bulletWorld),
	keyboard(&Keyboard::getInstance()),
	modeOffensive(true),
	cursorDelayLength(0.2f),
	cursorDelayDuration(0.f),
	moveCursor(false),
	glyphIdx(0),
	enemyCursor(new Sprite(_shader)),
	cam(_cam),
	confidence(50.f),
	damage(20.f),
	highlightShader(_shader)
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
	background->setVisible(false);

	confidenceSlider = new SliderControlled(_bulletWorld, &confidence, 0, 100.f);
	confidenceSlider->setBackgroundColour(1.f, 0, 0);
	confidenceSlider->fill->setBackgroundColour(0, 1.f, 0);
	confidenceSlider->setRationalWidth(0.7f);
	confidenceSlider->setRationalHeight(0.05f);

	selectedGlyphText = new TextArea(_bulletWorld, _font, _textShader, 0.9);
	selectedGlyphText->setBackgroundColour(0, 0, 1.f);
	selectedGlyphText->setWidth(50.f);
	selectedGlyphText->setRationalHeight(0.05f);
	selectedGlyphText->horizontalAlignment = kCENTER;

	enemyBubble = new NodeUI(_bulletWorld);
	//enemyBubble->verticalAlignment = kMIDDLE;
	//enemyBubble->horizontalAlignment = kCENTER;
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
	enemyBubbleText->background->setVisible(false);
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
	
	addChild(confidenceSlider);
	addChild(selectedGlyphText);
	addChild(enemyBubble);
	addChild(playerBubble);

	setUIMode(false);
}

void PD_UI_YellingContest::update(Step * _step){
	
	
	if(modeOffensive){
		if(keyboard->keyJustDown(GLFW_KEY_UP)){
			insult(pBubbleBtn1->isEffective);
		}
		if(keyboard->keyJustDown(GLFW_KEY_DOWN)){
			insult(pBubbleBtn2->isEffective);
		}
	}else{
		if (keyboard->keyJustDown(GLFW_KEY_SPACE)){
			interject();
		}
	}

	VerticalLinearLayout::update(_step);

	if(!modeOffensive){
		// Cursor
		if(glyphIdx < glyphs.size()){
			glm::vec3 screenPos = glyphs.at(glyphIdx)->childTransform->getWorldPos();
			float w = glyphs.at(glyphIdx)->getWidth();

			glm::vec3 screenPos1 = screenPos;
			glm::vec3 screenPos2 = glm::vec3(screenPos.x + w, screenPos.y, screenPos.z);
			
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

				if(glyphIdx < glyphs.size()){
					std::wstringstream s;
					s << glyphs.at(glyphIdx)->character;
					selectedGlyphText->setText(s.str());
				}
			}
			
		}else{
			moveCursor = false;
		}

		//Position Highlights
		typedef std::map<UIGlyph *, Sprite *>::iterator it_type;
		for(it_type it = highlights.begin(); it != highlights.end(); it++) {
			UIGlyph * g = it->first;
			Sprite * s = it->second;
			glm::vec3 pos = g->firstParent()->getTranslationVector();
			// text label
			glm::mat4 mm = g->firstParent()->firstParent()->firstParent()->firstParent()->firstParent()->firstParent()->getModelMatrix();
			pos = glm::vec3(mm* glm::vec4(pos, 1));
			// text area
			//pos = glm::vec3(firstParent()->getModelMatrix() * glm::vec4(pos, 1));
			s->childTransform->translate(pos, false);
			// 
		}
	}
}

void PD_UI_YellingContest::interject(){
	bool isPunctuation = false;
	if(glyphs.size() > 0 && glyphIdx < glyphs.size()){
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

	incrementConfidence(isPunctuation ? damage : -damage);

	setUIMode(isPunctuation);
}

void PD_UI_YellingContest::setUIMode(bool _isOffensive){
	modeOffensive = _isOffensive;
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
	cursorDelayDuration = 0;
	glyphs.clear();

	typedef std::map<UIGlyph *, Sprite *>::iterator it_type;
	for(it_type it = highlights.begin(); it != highlights.end(); it++) {
		enemyBubbleText->firstParent()->removeChild(it->second);
	}
	highlights.clear();

	for (auto label : enemyBubbleText->usedLines) {
		for (int i = 0; i < label->usedGlyphs.size(); ++i){
			glyphs.push_back(label->usedGlyphs.at(i));
			highlightPunctuation(label->usedGlyphs.at(i));
		}
	}
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
		incrementConfidence(damage);
		setPlayerText();
	}
	else{
		//fail
		incrementConfidence(-damage);
		setUIMode(false);
	}
}

void PD_UI_YellingContest::incrementConfidence(float _value){
	confidence = confidence + _value > 100.f ? 100.f : confidence + _value < 0.f ? 0.f : confidence + _value; 
}

void PD_UI_YellingContest::highlightPunctuation(UIGlyph * _glyph){

	switch (_glyph->character){
		case L'.':
		case L'?':
		case L'!':
		case L',':
			highlightGlyph(_glyph, true);
	}
}

void PD_UI_YellingContest::highlightGlyph(UIGlyph * _glyph, bool _highlight){
	if(_glyph != nullptr){
		std::map<UIGlyph *, Sprite *>::iterator it = highlights.find(_glyph);

		if (it == highlights.end()) {
			// doesn't exist
			if(_highlight){
				highlights.insert(std::map<UIGlyph *, Sprite *>::value_type(_glyph, new Sprite(highlightShader)));
				highlights.at(_glyph)->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-HIGHLIGHT")->texture);
				// move the cusrosr's mesh up so that the origin is aligned with the bottom
				for (unsigned long int i = 0; i < highlights.at(_glyph)->mesh->vertices.size(); ++i){
					highlights.at(_glyph)->mesh->vertices.at(i).x += 0.5f;
					highlights.at(_glyph)->mesh->vertices.at(i).y += 0.5f;
				}
				float w = _glyph->getWidth();
				float h = enemyBubbleText->font->getLineHeight();
				highlights.at(_glyph)->childTransform->scale(w, h, 1.f);

				enemyBubbleText->firstParent()->addChildAtIndex(highlights.at(_glyph), 0);
			}
		} else {
			// exists
			if(!_highlight){
				enemyBubbleText->firstParent()->removeChild(it->second);
				highlights.erase(it);
			}
		}
	}
}