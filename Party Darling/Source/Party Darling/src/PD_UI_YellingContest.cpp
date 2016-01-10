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

PD_UI_YellingContest::PD_UI_YellingContest(BulletWorld* _bulletWorld, Font * _font, Shader * _textShader, Shader * _shader, Camera * _cam) :
	VerticalLinearLayout(_bulletWorld),
	keyboard(&Keyboard::getInstance()),
	modeOffensive(true),
	baseCursorDelayLength(0.2f),
	cursorDelayLength(0.f),
	cursorDelayDuration(0.f),
	baseGlyphWidth(_font->getGlyphWidthHeight('m').x),
	glyphIdx(0),
	enemyCursor(new Sprite(_shader)),
	cam(_cam),
	confidence(50.f),
	damage(20.f),
	shader(_shader),
	highlightedPunctuation(nullptr),
	punctuationHighlight(new Sprite(_shader)),
	highlightedWordStart(nullptr),
	highlightedWordEnd(nullptr),
	wordHighlight(new Sprite(_shader)),
	isGameOver(false),
	gameOverLength(1.f),
	gameOverDuration(0.f)
{
	verticalAlignment = kTOP;
	horizontalAlignment = kCENTER;
	background->setVisible(false);

	// Enemy Cursor
	enemyCursor->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-CURSOR")->texture);
	enemyCursor->childTransform->scale(20.f);
	childTransform->addChild(enemyCursor);

	// move the cusrosr's mesh up so that the origin is aligned with the top
	for (unsigned long int i = 0; i < enemyCursor->mesh->vertices.size(); ++i){
		enemyCursor->mesh->vertices.at(i).y -= 0.5f;
	}

	// Punctuation Highlight
	punctuationHighlight->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-HIGHLIGHT")->texture);
	// move the highlight's mesh up so that the origin is aligned with the bottom
	for (unsigned long int i = 0; i < punctuationHighlight->mesh->vertices.size(); ++i){
		punctuationHighlight->mesh->vertices.at(i).x += 0.5f;
		punctuationHighlight->mesh->vertices.at(i).y += 0.5f;
	}
	punctuationHighlight->setVisible(false);

	// Word Highlight
	// move the highlights's mesh up so that the origin is aligned with the bottom and set the colour
	for (unsigned long int i = 0; i < wordHighlight->mesh->vertices.size(); ++i){
		wordHighlight->mesh->vertices.at(i).x += 0.5f;
		wordHighlight->mesh->vertices.at(i).y += 0.5f;

		wordHighlight->mesh->vertices.at(i).red = 0.820;
		wordHighlight->mesh->vertices.at(i).green = 0.722;
		wordHighlight->mesh->vertices.at(i).blue = 0.851;

	}
	wordHighlight->setVisible(false);

	livesContainer = new HorizontalLinearLayout(_bulletWorld);
	livesContainer->setBackgroundColour(0.5f, 1.f, 0.5f);
	livesContainer->setRationalWidth(0.5f);
	livesContainer->setHeight(50.f);
	livesContainer->setPadding(0.01f);
	livesContainer->setMarginTop(20.f);
	livesContainer->setMarginBottom(20.f);

	// loop through friends and add tokens
	for(unsigned int i = 0; i < 3; ++i){
		NodeUI * l = new NodeUI(world);
		Texture * tex = PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-FRIENDSHIP")->texture;
		l->background->mesh->pushTexture2D(tex);
		l->setWidth(tex->width);
		l->setHeight(tex->height);
		lives.push_back(l);
		livesContainer->addChild(l);
	}

	confidenceSlider = new SliderControlled(_bulletWorld, &confidence, 0, 100.f);
	confidenceSlider->boxSizing = kCONTENT_BOX;
	confidenceSlider->setBackgroundColour(1.f, 0, 0);
	confidenceSlider->fill->setBackgroundColour(0, 1.f, 0);
	confidenceSlider->setRationalWidth(0.7f);
	confidenceSlider->setRationalHeight(0.05f);
	confidenceSlider->setMarginBottom(0.2f);

	selectedGlyphText = new TextArea(_bulletWorld, _font, _textShader, 0.9);
	selectedGlyphText->setBackgroundColour(0, 0, 1.f);
	selectedGlyphText->setWidth(50.f);
	selectedGlyphText->setRationalHeight(0.05f);
	selectedGlyphText->horizontalAlignment = kCENTER;

	enemyBubble = new NodeUI(_bulletWorld);
	enemyBubble->setRationalWidth(0.25f, this);
	enemyBubble->setRationalHeight(0.25f, this);
	enemyBubble->setBackgroundColour(0.682f, 0.439f, 0.670f, 1);
	enemyBubbleText = new TextArea(world, _font, _textShader, 0.9);
	enemyBubbleText->setWrapMode(kWORD);
	enemyBubbleText->setRationalWidth(1.0f, enemyBubble);
	enemyBubbleText->setRationalHeight(1.0f, enemyBubble);
	enemyBubbleText->horizontalAlignment = kCENTER;
	enemyBubbleText->verticalAlignment = kMIDDLE;
	enemyBubbleText->background->setVisible(false);
	enemyBubble->uiElements->addChild(wordHighlight);
	enemyBubble->uiElements->addChild(punctuationHighlight);
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

	gameOverContainer = new VerticalLinearLayout(_bulletWorld);
	gameOverContainer->horizontalAlignment = kCENTER;
	gameOverContainer->verticalAlignment = kMIDDLE;
	gameOverContainer->setRationalWidth(0.5f);
	gameOverContainer->setHeight(0.4f);
	gameOverContainer->setBackgroundColour(0.5, 0.5, 1.f);

	gameOverImage = new NodeUI(_bulletWorld);
	gameOverImage->setRationalWidth(0.5f);
	gameOverImage->setRationalHeight(0.5f);
	gameOverContainer->addChild(gameOverImage);
	// don't add the container until yelling contest is over
	
	addChild(livesContainer);
	addChild(confidenceSlider);
	addChild(selectedGlyphText);
	addChild(enemyBubble);
	addChild(playerBubble);

	setUIMode(false);
}

void PD_UI_YellingContest::update(Step * _step){
	if(!isGameOver){
		VerticalLinearLayout::update(_step);

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
					// Get next glyph

					// Find next punctuation
					if(highlightedPunctuation != nullptr && glyphs.at(glyphIdx) == highlightedPunctuation){
						highlightedPunctuation = findFirstPunctuation(glyphIdx+1);
					}

					if(highlightedWordEnd != nullptr && glyphs.at(glyphIdx) == highlightedWordEnd){
						highlightNextWord(glyphIdx+1);
					}

					++glyphIdx;
					cursorDelayDuration = 0;

					if(glyphIdx < glyphs.size()){
						cursorDelayLength = glyphs.at(glyphIdx)->getWidth() / baseGlyphWidth * baseCursorDelayLength;
						std::wstringstream s;
						s << glyphs.at(glyphIdx)->character;
						selectedGlyphText->setText(s.str());
					}
				}
			
			}

			//Position Highlights
			// Punctuation Highlight
			if(highlightedPunctuation != nullptr){
				glm::vec3 pos = highlightedPunctuation->firstParent()->getTranslationVector();
				// text label
				glm::mat4 mm = highlightedPunctuation->nodeUIParent->firstParent()->getModelMatrix();
				pos = glm::vec3(mm* glm::vec4(pos, 1));
			
				punctuationHighlight->childTransform->translate(pos, false);
			}

			// Word Highlight
			if(highlightedWordStart != nullptr){
				glm::vec3 pos = highlightedWordStart->firstParent()->getTranslationVector();
				// text label
				glm::mat4 mm = highlightedWordStart->nodeUIParent->firstParent()->getModelMatrix();
				pos = glm::vec3(mm* glm::vec4(pos, 1));
			
				wordHighlight->childTransform->translate(pos, false);
			}
		}
	}else{
		gameOverDuration += _step->getDeltaTime();
		
		if(gameOverDuration >= gameOverLength){
			complete();
		}else{
			float size = 0.5f + (gameOverDuration / (2 * gameOverLength));
			gameOverImage->setRationalWidth(size, gameOverContainer);
			gameOverImage->setRationalHeight(size, gameOverContainer);
			gameOverContainer->invalidateLayout();
		}

		VerticalLinearLayout::update(_step);
	}
}

void PD_UI_YellingContest::complete(){
	// I don't know
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
		}
	}
	if(glyphs.size() > 0){
		cursorDelayLength = glyphs.at(0)->getWidth() / baseGlyphWidth * baseCursorDelayLength;
	}

	highlightedPunctuation = findFirstPunctuation();
	highlightNextWord();
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

	if(confidence <= 0){
		// Check if out of lives
		if(lives.size() <= 0){
			gameOver(false);
		}else{
			livesContainer->removeChild(lives.back());
			lives.pop_back();
			confidence = 50.f;
		}
		
	}

	if(confidence >= 100){
		gameOver(true);
	}
}

void PD_UI_YellingContest::gameOver(bool _win){
	isGameOver = true;

	removeChild(enemyBubble);
	removeChild(playerBubble);

	if(_win){
		gameOverImage->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-WIN")->texture);
	}else{
		gameOverImage->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-LOSE")->texture);
	}

	addChild(gameOverContainer);
}

UIGlyph * PD_UI_YellingContest::findFirstPunctuation(int _startIdx){
	for(int i = _startIdx; i < glyphs.size(); ++i){
		if(isPunctuation(glyphs.at(i))){
			float w = glyphs.at(i)->getWidth();
			float h = enemyBubbleText->font->getLineHeight();
			punctuationHighlight->childTransform->scale(w, h, 1.f, false);
			punctuationHighlight->setVisible(true);

			return glyphs.at(i);
		}
	}
	punctuationHighlight->setVisible(false);
	return nullptr;
}

void PD_UI_YellingContest::highlightNextWord(int _startIdx){
	float w = 0;
	highlightedWordStart = nullptr;
	highlightedWordEnd = nullptr;

	for(int i = _startIdx; i < glyphs.size(); ++i){
		// Search for start of next word
		if(highlightedWordStart == nullptr){
			if(i == 0 || glyphs.at(i-1)->character == ' '){
				highlightedWordStart = glyphs.at(i);
			}
		}

		// If start of word found, continue until end of word
		if(highlightedWordStart != nullptr){
			w += glyphs.at(i)->getWidth();
			if(i == glyphs.size()-1 || glyphs.at(i+1)->character == ' '){
				float h = enemyBubbleText->font->getLineHeight();
				highlightedWordEnd = glyphs.at(i);
				wordHighlight->childTransform->scale(w, h, 1.f, false);
				wordHighlight->setVisible(true);

				return;
			}
		}
	}
	wordHighlight->setVisible(false);
	
}

bool PD_UI_YellingContest::isPunctuation(UIGlyph * _glyph){

	switch (_glyph->character){
		case L'.':
		case L'?':
		case L'!':
		case L',':
			return true;
	}

	return false;
}