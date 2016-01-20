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
#include <FileUtils.h>
#include <shader/ComponentShaderText.h>
#include <DateUtils.h>

#define BORDER_SIZE 60.f

InterjectAccuracy::InterjectAccuracy(wchar_t _character, float _padding, float _targetTime, float _hitTime, unsigned long int _iteration):
	character(_character),
	padding(_padding),
	targetTime(_targetTime),
	hitTime(_hitTime),
	iteration(_iteration)
{

}

PD_UI_YellingContest::PD_UI_YellingContest(BulletWorld* _bulletWorld, Font * _font, Shader * _textShader, Shader * _shader) :
	VerticalLinearLayout(_bulletWorld),
	keyboard(&Keyboard::getInstance()),
	modeOffensive(true),
	baseCursorDelayLength(0.2f),
	cursorDelayLength(0.f),
	cursorDelayDuration(0.f),
	baseGlyphWidth(_font->getGlyphWidthHeight('m').x),
	glyphIdx(0),
	enemyCursor(new Sprite(_shader)),
	confidence(50.f),
	playerQuestionTimerLength(2.f),
	playerQuestionTimer(0),
	playerAnswerTimerLength(1.f),
	playerAnswerTimer(0),
	damage(20.f),
	shader(_shader),
	highlightedPunctuation(nullptr),
	prevHighlightedPunctuation(nullptr),
	punctuationHighlight(new Sprite(_shader)),
	highlightedWordStart(nullptr),
	highlightedWordEnd(nullptr),
	wordHighlight(new Sprite(_shader)),
	isGameOver(false),
	gameOverLength(1.f),
	gameOverDuration(0.f),
	win(false),
	isComplete(false),
	isEnabled(true),
	offensiveCorrect(0),
	offensiveWrong(0),
	defensiveCorrect(0),
	defensiveWrong(0),
	interjectTimer(0),
	punctuationCnt(-1),
	iteration(0)
{
	verticalAlignment = kTOP;
	horizontalAlignment = kCENTER;
	background->setVisible(false);
	//setMarginTop(0.1f);

	float borderSize = sweet::getWindowHeight() * 0.125 / 2;

	healthContainer = new VerticalLinearLayout(_bulletWorld);
	healthContainer->setRationalWidth(1.f, this);
	healthContainer->setRationalHeight(0.15f, this);
	healthContainer->horizontalAlignment = kCENTER;
	//healthContainer->setBackgroundColour(0, 1.f, 0.541f);

	gameContainer = new VerticalLinearLayout(_bulletWorld);
	gameContainer->setRationalWidth(1.f, this);
	gameContainer->setRationalHeight(0.85f, this);
	gameContainer->horizontalAlignment = kCENTER;
	gameContainer->verticalAlignment = kTOP;
	//gameContainer->setBackgroundColour(0, 0.714f, 0.929f);

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
	livesContainer->setRationalHeight(0.6f);
	livesContainer->setMarginTop(0.1f);
	livesContainer->setMarginBottom(0.1f);

	confidenceSlider = new SliderControlled(_bulletWorld, &confidence, 0, 100.f);
	confidenceSlider->boxSizing = kCONTENT_BOX;
	confidenceSlider->setBackgroundColour(1.f, 0, 0);
	confidenceSlider->fill->setBackgroundColour(0, 1.f, 0);
	confidenceSlider->setRationalWidth(0.7f);
	confidenceSlider->setRationalHeight(0.4f);
	confidenceSlider->setMarginBottom(0.1f);

	selectedGlyphText = new TextArea(_bulletWorld, _font, _textShader, 0.9);
	selectedGlyphText->setBackgroundColour(0, 0, 1.f);
	selectedGlyphText->setWidth(50.f);
	selectedGlyphText->setRationalHeight(0.05f);
	selectedGlyphText->horizontalAlignment = kCENTER;

	enemyBubble = new NodeUI(_bulletWorld);
	enemyBubble->setRationalWidth(0.5f, gameContainer);
	enemyBubble->setRationalHeight(0.4f, gameContainer);
	enemyBubble->background->setVisible(false);
	enemyBubble->setBackgroundColour(0, 0, 1, 0.5f);
	enemyBubble->setMargin(0.05f);

	NodeUI_NineSliced * enemyBubbleBubble = new NodeUI_NineSliced(_bulletWorld, PD_ResourceManager::scenario->getNineSlicedTexture("YELLING-CONTEST-DEFENSE-BUBBLE"));
	enemyBubbleBubble->setBorder(borderSize);
	enemyBubble->addChild(enemyBubbleBubble);
	enemyBubbleBubble->setRationalWidth(1.f);
	enemyBubbleBubble->setRationalHeight(1.f);
	enemyBubbleBubble->setMarginLeft(0.15f);
	enemyBubbleBubble->setScaleMode(GL_NEAREST);

	enemyBubbleText = new TextArea(world, _font, _textShader, 0.8);
	enemyBubbleText->setWrapMode(kWORD);
	enemyBubbleText->setRationalWidth(0.95f, enemyBubble);
	enemyBubbleText->setRationalHeight(0.95f, enemyBubble);
	enemyBubbleText->horizontalAlignment = kCENTER;
	enemyBubbleText->verticalAlignment = kMIDDLE;
	enemyBubbleText->background->setVisible(true);
	enemyBubbleBubble->uiElements->addChild(wordHighlight);
	enemyBubbleBubble->uiElements->addChild(punctuationHighlight);
	enemyBubbleBubble->addChild(enemyBubbleText);
	
	
	NodeUI * enemyBubbleTail = new NodeUI(_bulletWorld);
	enemyBubbleTail->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-DEFENSE-BUBBLE-TAIL")->texture);
	enemyBubbleTail->setRationalWidth(1.f, enemyBubble);
	enemyBubbleTail->setRationalHeight(1.f, enemyBubble);
	enemyBubbleTail->setMarginRight(0.84f);
	enemyBubbleTail->setMarginTop(0.3f);
	enemyBubbleTail->setMarginBottom(0.3f);
	enemyBubbleTail->background->mesh->setScaleMode(GL_NEAREST);

	enemyBubble->addChild(enemyBubbleTail);

	playerBubble = new NodeUI(_bulletWorld);
	playerBubble->setRationalWidth(0.6f, gameContainer);
	playerBubble->setRationalHeight(0.5f, gameContainer);
	playerBubble->setMarginBottom(0.05f);
	playerBubble->background->setVisible(false);
	playerBubble->setBackgroundColour(1, 0, 0, 0.5f);
	playerBubble->setMargin(0.05f);

	// The fancy nine sliced bubble
	NodeUI_NineSliced * playerBubbleBubble = new NodeUI_NineSliced(_bulletWorld, PD_ResourceManager::scenario->getNineSlicedTexture("YELLING-CONTEST-OFFENSE-BUBBLE"));
	playerBubbleBubble->setBorder(borderSize);
	playerBubble->addChild(playerBubbleBubble);
	playerBubbleBubble->setRationalWidth(1.f);
	playerBubbleBubble->setRationalHeight(1.f);
	playerBubbleBubble->setMarginLeft(0.15f);
	playerBubbleBubble->setMarginBottom(0.2f);
	playerBubbleBubble->setScaleMode(GL_NEAREST);

	// The side by side text and button layout
	HorizontalLinearLayout * playerBubbleLayout = new HorizontalLinearLayout(_bulletWorld);
	playerBubbleLayout->verticalAlignment = kMIDDLE;
	playerBubbleLayout->horizontalAlignment = kCENTER;
	playerBubbleLayout->setRationalWidth(0.95f);
	playerBubbleLayout->setRationalHeight(0.95f);
	playerBubbleBubble->addChild(playerBubbleLayout);

	playerBubbleText = new TextArea(world, _font, _textShader, 0.8f);
	playerBubbleText->setRationalWidth(0.6f);
	playerBubbleText->setRationalHeight(1.0f);
	playerBubbleText->horizontalAlignment = kCENTER;
	playerBubbleText->verticalAlignment = kMIDDLE;
	playerBubbleLayout->addChild(playerBubbleText);

	NodeUI * playerArrows = new NodeUI(_bulletWorld);
	playerArrows->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-OFFENSE-ARROWS")->texture);
	playerArrows->setRationalWidth(0.1f);
	playerArrows->setRationalHeight(0.4f);
	playerBubbleLayout->addChild(playerArrows);
	playerArrows->background->mesh->setScaleMode(GL_NEAREST);

	VerticalLinearLayout * buttonLayout = new VerticalLinearLayout(_bulletWorld);
	buttonLayout->setRationalWidth(0.3f);
	buttonLayout->setRationalHeight(0.4f);
	buttonLayout->verticalAlignment = kMIDDLE;

	pBubbleBtn1 = new PD_InsultButton(_bulletWorld, _font, _textShader);
	pBubbleBtn1->setRationalWidth(1.0f);
	pBubbleBtn1->setRationalHeight(0.5f);
	pBubbleBtn1->setMouseEnabled(false);

	pBubbleBtn2 = new PD_InsultButton(_bulletWorld, _font, _textShader);
	pBubbleBtn2->setRationalWidth(1.0f);
	pBubbleBtn2->setRationalHeight(0.5f);
	pBubbleBtn2->setMouseEnabled(false);

	buttonLayout->addChild(pBubbleBtn1);
	buttonLayout->addChild(pBubbleBtn2);

	playerBubbleLayout->addChild(buttonLayout);

	NodeUI * playerBubbleTail = new NodeUI(_bulletWorld);
	playerBubbleTail->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-OFFENSE-BUBBLE-TAIL")->texture);
	playerBubbleTail->setRationalWidth(1.f, playerBubble);
	playerBubbleTail->setRationalHeight(1.f, playerBubble);
	playerBubbleTail->setMarginRight(0.84f);
	playerBubbleTail->setMarginTop(0.33f);
	playerBubbleTail->setMarginBottom(0.03f);
	playerBubbleTail->background->mesh->setScaleMode(GL_NEAREST);
	playerBubble->addChild(playerBubbleTail);

	playerTimerSlider = new SliderControlled(_bulletWorld, &playerAnswerTimer, 0, playerAnswerTimerLength, true, true);
	playerTimerSlider->setRationalWidth(0.25f);
	playerTimerSlider->setRationalHeight(0.05f);
	playerTimerSlider->setBackgroundColour(0, 1.f, 0);
	playerTimerSlider->fill->setBackgroundColour(0, 0, 1.f);
	playerTimerSlider->thumb->setVisible(false);

	gameOverContainer = new VerticalLinearLayout(_bulletWorld);
	gameOverContainer->horizontalAlignment = kCENTER;
	gameOverContainer->verticalAlignment = kMIDDLE;
	gameOverContainer->setRationalWidth(0.5f);
	gameOverContainer->setRationalHeight(0.4f);
	gameOverContainer->setBackgroundColour(0.5, 0.5, 1.f);

	gameOverImage = new NodeUI(_bulletWorld);
	gameOverImage->setRationalWidth(0.5f);
	gameOverImage->setRationalHeight(0.5f);
	gameOverImage->background->mesh->setScaleMode(GL_NEAREST);
	gameOverContainer->addChild(gameOverImage);
	// don't add the container until yelling contest is over


	healthContainer->addChild(livesContainer);
	healthContainer->addChild(confidenceSlider);

	//addChild(selectedGlyphText);
	gameContainer->addChild(enemyBubble);
	gameContainer->addChild(playerBubble);
	gameContainer->addChild(playerTimerSlider);
	
	addChild(healthContainer);
	addChild(gameContainer);

	// disable and hide by default
	disable();

	eventManager.addEventListener("wordspoken", [this](sweet::Event * _event){
		// Stuff!!!
		
	});
}

void PD_UI_YellingContest::update(Step * _step){
	if(isEnabled){
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
				// INTERJECT
				interjectTimer += _step->getDeltaTime();
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
							prevHighlightedPunctuation = highlightedPunctuation;
							highlightedPunctuation = findFirstPunctuation(glyphIdx+1);
						}

						if(highlightedWordEnd != nullptr && glyphs.at(glyphIdx) == highlightedWordEnd){
							highlightNextWord(glyphIdx+1);
						}

						++glyphIdx;
						cursorDelayDuration = 0;

						if(glyphIdx < glyphs.size()){
							cursorDelayLength = glyphs.at(glyphIdx)->getWidth() / baseGlyphWidth * baseCursorDelayLength;
							/*
							std::wstringstream s;
							s << glyphs.at(glyphIdx)->character;
							selectedGlyphText->setText(s.str());
							*/
							// play sound
							OpenAL_Sound * s = PD_ResourceManager::scenario->getAudio("DEFAULT")->sound;
							s->setPitch(sweet::NumberUtils::randomInt(5,15)/10.f);
							s->play();
						}else{
							incrementConfidence(-damage);
							setEnemyText();
						}
					}
			
				}

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
			}else{
				// INSULT
				if(playerQuestionTimer >= playerQuestionTimerLength){
					// Increment player answer timer
					if(playerAnswerTimer >= playerAnswerTimerLength){
						// Out of time, enemy's turn!
						countInsultAccuracy(-1);
						incrementConfidence(-damage);
						setUIMode(false);
					}else{
						playerAnswerTimer += _step->getDeltaTime();
					}
				}else{
					// Increment player question timer
					playerQuestionTimer += _step->getDeltaTime();
					if(playerQuestionTimer >= playerQuestionTimerLength){
						pBubbleBtn1->setVisible(true);
						pBubbleBtn2->setVisible(true);
						playerTimerSlider->setVisible(true);
					}
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
}

void PD_UI_YellingContest::startNewFight(){
	// clear existing friendship
	for(unsigned int i = 0; i < lives.size(); ++i){
		livesContainer->removeChild(lives.at(i));
	}
	for(unsigned int i = 0; i < lostLives.size(); ++i){
		livesContainer->removeChild(lostLives.at(i));
	}
	lives.clear();
	lostLives.clear();

	// loop through friends and add tokens
	for(unsigned int i = 0; i < 0; ++i){
		NodeUI * l = new NodeUI(world);
		Texture * tex = PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-FRIENDSHIP")->texture;
		l->background->mesh->pushTexture2D(tex);
		l->setWidth(livesContainer->getHeight());
		l->setHeight(livesContainer->getHeight());
		lives.push_back(l);
		livesContainer->addChild(l);
	}

	confidence = 50.f;

	if(isGameOver){
		// Reset layout
		gameOverImage->setRationalWidth(0.5f);
		gameOverImage->setRationalHeight(0.5f);
		removeChild(gameOverContainer);
		gameOverDuration = 0;

		addChild(gameContainer);
		
		win = false;
		isGameOver = false;
		
	}
	isComplete = false;

	setUIMode(false);
	enable();
}

void PD_UI_YellingContest::gameOver(bool _win){
	isGameOver = true;
	win = _win;

	removeChild(gameContainer);

	if(_win){
		gameOverImage->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-WIN")->texture);
	}else{
		gameOverImage->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-LOSE")->texture);
	}

	addChild(gameOverContainer);

	// USER TESTING INFORMATION
	sweet::FileUtils::createDirectoryIfNotExists("data/YellingContestResults");
	{
		std::stringstream filename;
		std::ofstream file;

		// success rate
		filename << "data/YellingContestResults/" << sweet::DateUtils::getDatetime() << "_YellingContestResult_SuccessRate.csv";
		file.open(filename.str());
		file << "correctInterject;incorrectInterject;correctInsult;incorrectInsult;" << std::endl;
		file << defensiveCorrect << ";" << defensiveWrong << ";" << offensiveCorrect << ";" << offensiveWrong << std::endl;
		file.close();
	}


	// interject statistics
	{
		std::stringstream filename;
		std::ofstream file;
		float prevTargetTime = 0;
		float targetTime = 0;

		filename << "data/YellingContestResults/" << sweet::DateUtils::getDatetime() << "_YellingContestResult_Interject.csv";
		file.open(filename.str());
		file << "iteration;character;hitPadding;hitTime;targetTime;diffToNext;diffToPrev;expectedTarget;percentageError;" << std::endl;
		for(auto i : interjectTimes){
			if(targetTime != i.targetTime){
				prevTargetTime = targetTime;
				targetTime = i.targetTime;
			}

			std::string expected;
			float expectedDiff;
			if(std::abs(i.hitTime) < std::abs(targetTime - i.hitTime)){
				expectedDiff = std::abs(i.hitTime);
				expected = "PREV";
			}else{
				expectedDiff = std::abs(targetTime - i.hitTime);
				expected = "NEXT";
			
			}

			file << i.iteration
				 << ";" << i.character
				 << ";" << i.padding
				 << ";" << i.hitTime
				 << ";" << targetTime
				 << ";" << (targetTime - i.hitTime)
				 << ";" << (i.hitTime)
				 << ";" << expected
				 << ";" << (expectedDiff / i.padding)*100 << "%" << std::endl;
		}
		file.close();
	}
	
	// insult statistics
	{
		std::stringstream filename;
		std::ofstream file;
		filename << "data/YellingContestResults/" << sweet::DateUtils::getDatetime() << "_YellingContestResult_Insult.csv";
		file.open(filename.str());

		file << "playerAnswerTimerLength FAILED (Out of time): -1;hitTime;" << std::endl;
		for(auto i : insultTimes){
			file << playerAnswerTimerLength << ";" << i << std::endl;
		}
		file.close();
	}
}

void PD_UI_YellingContest::complete(){
	if(!isComplete){
		sweet::Event * e = new sweet::Event("complete");
		e->setIntData("win", win);
		eventManager.triggerEvent(e);
		isComplete = true;
	}
}

void PD_UI_YellingContest::disable(){
	setVisible(false);
	isEnabled = false;
	invalidateLayout();
}

void PD_UI_YellingContest::enable(){
	setVisible(true);
	isEnabled = true;
	invalidateLayout();
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
	countButtonPresses(isPunctuation, false);
	countInterjectAccuracy(interjectTimer);

	incrementConfidence(isPunctuation ? damage : -damage);

	if(isPunctuation){
		setUIMode(isPunctuation);
	}
}

void PD_UI_YellingContest::setUIMode(bool _isOffensive){
	modeOffensive = _isOffensive;
	enemyBubble->setVisible(!_isOffensive);
	enemyCursor->setVisible(!_isOffensive);

	playerBubble->setVisible(_isOffensive);
	playerTimerSlider->setVisible(_isOffensive);
	
	if (!_isOffensive){
		setEnemyText();
	}
	else{
		setPlayerText();
	}
	invalidateLayout();
}

void PD_UI_YellingContest::setEnemyText(){
	insultGenerator.makeInsults();

	enemyBubbleText->setText(insultGenerator.enemyInsult);

	//enemyBubble->invalidateLayout();

	glyphIdx = 0;
	cursorDelayDuration = 0;
	glyphs.clear();

	for (auto label : enemyBubbleText->usedLines) {
		for (auto glyph : label->usedGlyphs){
			glyphs.push_back(glyph);
		}
	}

	if(glyphs.size() > 0){
		cursorDelayLength = glyphs.at(0)->getWidth() / baseGlyphWidth * baseCursorDelayLength;
	}
	
	interjectTimer = 0;
	prevHighlightedPunctuation = nullptr;
	highlightedPunctuation = findFirstPunctuation();
	highlightNextWord();
}

void PD_UI_YellingContest::setPlayerText(){
	insultGenerator.makeInsults();
	// set the insult prompt
	playerBubbleText->setText(insultGenerator.playerInsult);
	
	// randomize which of the two options
	// presented to the user is effective
	bool btn1E = sweet::NumberUtils::randomBool();

	pBubbleBtn1->isEffective = btn1E;
	pBubbleBtn2->isEffective = !btn1E;

	pBubbleBtn1->label->setText(btn1E ? insultGenerator.playerBadChoice : insultGenerator.playerGoodChoice);
	pBubbleBtn2->label->setText(btn1E ? insultGenerator.playerGoodChoice : insultGenerator.playerBadChoice);
	
	// Reset timer
	playerQuestionTimer = 0;
	playerAnswerTimer = 0;

	pBubbleBtn1->setVisible(false);
	pBubbleBtn2->setVisible(false);
	playerTimerSlider->setVisible(false);
}

void PD_UI_YellingContest::insult(bool _isEffective){
	countButtonPresses(_isEffective, true);
	countInsultAccuracy(playerAnswerTimer);
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
			lostLives.push_back(lives.back());
			lives.back()->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("YELLING-CONTEST-LOSTFRIENDSHIP")->texture);
			lives.pop_back();
			confidence = 50.f;
		}
	}

	if(confidence >= 100){
		gameOver(true);
	}
}

UIGlyph * PD_UI_YellingContest::findFirstPunctuation(int _startIdx){
	// No autos here, it starts at _startIdx
	for (unsigned int i = _startIdx; i < glyphs.size(); ++i){
		UIGlyph * glyph = glyphs.at(i);
		if(isPunctuation(glyph)){
			float w = glyph->getWidth();
			float h = enemyBubbleText->font->getLineHeight();
			punctuationHighlight->childTransform->scale(w, h, 1.f, false);
			punctuationHighlight->setVisible(true);
			++punctuationCnt;
			++iteration;
			return glyph;
		}
	}
	punctuationHighlight->setVisible(false);
	return nullptr;
}

void PD_UI_YellingContest::highlightNextWord(int _startIdx){
	unsigned int length = 0;
	float w = 0;
	highlightedWordStart = nullptr;
	highlightedWordEnd = nullptr;

	for(unsigned long int i = _startIdx; i < glyphs.size(); ++i){
		// Search for start of next word
		if(highlightedWordStart == nullptr){
			if(i == 0 || glyphs.at(i-1)->character == ' '){
				highlightedWordStart = glyphs.at(i);
			}
		}

		// If start of word found, continue until end of word
		if(highlightedWordStart != nullptr){
			w += glyphs.at(i)->getWidth();
			if(!isPunctuation(glyphs.at(i))){
				++length;
			}

			if(i == glyphs.size()-1 || glyphs.at(i+1)->character == ' '){
				float h = enemyBubbleText->font->getLineHeight();
				highlightedWordEnd = glyphs.at(i);
				wordHighlight->childTransform->scale(w, h, 1.f, false);
				wordHighlight->setVisible(true);
				
				sweet::Event * e = new sweet::Event("wordspoken");
				e->setIntData("length", length);
				eventManager.triggerEvent(e);

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

void PD_UI_YellingContest::countButtonPresses(bool _isCorrect, bool _isOffensive){
	std::stringstream s;
	if(_isOffensive){
		if(_isCorrect){
			++offensiveCorrect;
		}else{
			++offensiveWrong;
		}
	}else{
		if(_isCorrect){
			++defensiveCorrect;
		}else{
			++defensiveWrong;
		}
	}
}

void PD_UI_YellingContest::countInterjectAccuracy(float _pressTime){
	if(highlightedPunctuation != nullptr){
		float punctuationTimeStart = 0, punctuationMeasureFromTime = 0;
		float padding = 0;
		
		if(prevHighlightedPunctuation != nullptr){
			for(auto glyph : glyphs){
				float glyphTime = glyph->getWidth() / baseGlyphWidth * baseCursorDelayLength;
				if(glyph != prevHighlightedPunctuation){
					punctuationMeasureFromTime += glyphTime;
				}else{
					punctuationMeasureFromTime += glyphTime/2.f;
					break;
				}
			}
		}
		
		for(auto glyph : glyphs){
			float glyphTime = glyph->getWidth() / baseGlyphWidth * baseCursorDelayLength;
			if(glyph != highlightedPunctuation){
				punctuationTimeStart += glyphTime;
			}else{
				padding += glyphTime;
				break;
			}
		}

		float targetTime = (punctuationTimeStart - punctuationMeasureFromTime) + padding/2.f;
		//float hitTime = _pressTime == NULL ? NULL : _pressTime - targetTime;

		interjectTimes.push_back(InterjectAccuracy(highlightedPunctuation->character, padding, targetTime, (_pressTime-punctuationMeasureFromTime), iteration));
	}

	
}

void PD_UI_YellingContest::countInsultAccuracy(float _hitTime){
	insultTimes.push_back(_hitTime);
}