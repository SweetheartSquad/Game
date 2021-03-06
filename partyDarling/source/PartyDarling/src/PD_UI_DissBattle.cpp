#pragma once

#include <PD_UI_DissBattle.h>
#include <PD_InsultButton.h>

#include <Scene.h>
#include <GLFW\glfw3.h>
#include <Easing.h>
#include <Sprite.h>
#include <PD_ResourceManager.h>
#include <NumberUtils.h>
#include <FileUtils.h>
#include <shader/ComponentShaderText.h>
#include <DateUtils.h>
#include <regex>
#include <ProgrammaticTexture.h>
#include <TextureUtils.h>
#include <PD_DissStats.h>

#define PLAYER_TEXT_WIDTH 0.5f
#define FAIL_INSULT	"glassBreak"
#define SUCCEED_INSULT	"ohhh_"
#define NUM_SUCCEED_INSULT	7
#define PASSED_INSULT_TIME_LIMIT "glassBreak"
#define INTERJECT "recordScratch"
#define NUM_COMPLIMENTS 6
#define TIMER "timer"
#define MIN_OFFENSE_SPEED_MULTIPLIER 0.3
#define NUM_OFFENSE_SPEED_CHANGES 3

InterjectAccuracy::InterjectAccuracy(wchar_t _character, float _padding, float _targetTime, float _hitTime, unsigned long int _iteration):
	character(_character),
	padding(_padding),
	targetTime(_targetTime),
	hitTime(_hitTime),
	iteration(_iteration)
{
}

DissBattleValues::DissBattleValues(){
	Json::Value root;
	Json::Reader reader;
	std::string jsonLoaded = sweet::FileUtils::readFile("assets/stats.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
	}else{
		
		for(unsigned long int i = 0; i < 4; ++i){
			std::string posOrNeg = i < 2 ? "pos" : "neg";
			int j = i < 2 ? i : (i-2);
			playerAttackMultiplier[i] = root["statMultipliers"][posOrNeg]["playerAttack"].get(j, 1).asFloat();
			enemyAttackMultiplier[i] = root["statMultipliers"][posOrNeg]["enemyAttack"].get(j, 1).asFloat();
			insightMultiplier[i] = root["statMultipliers"][posOrNeg]["insight"].get(j, 1).asFloat();
			insightAlpha[i] = root["statMultipliers"][posOrNeg]["insightAlpha"].get(j, 1).asFloat();
			sassInsultMultiplier[i] = root["statMultipliers"][posOrNeg]["sassInsult"].get(j, 1).asFloat();
			sassInterjectMultiplier[i] = root["statMultipliers"][posOrNeg]["sassInterject"].get(j, 1).asFloat();
		}

		// combo increments
		playerComboIncrement = root["comboMultipliers"].get("player", 1.f).asFloat();
		enemyComboIncrement = root["comboMultipliers"].get("enemy", 1.f).asFloat();
	}
}


PD_UI_DissBattle::PD_UI_DissBattle(BulletWorld* _bulletWorld, Player * _player, Font * _font, Shader * _textShader, Shader * _shader) :
	VerticalLinearLayout(_bulletWorld),
	iteration(0),
	enabled(true),
	canInterject(true),
	basePlayerInsultSpeedMultiplier(1.f),
	playerInsultSpeedMultiplier(basePlayerInsultSpeedMultiplier),
	basePlayerQuestionTimerLength(1.f),
	playerQuestionTimerLength(basePlayerQuestionTimerLength),
	playerQuestionTimer(0),
	basePlayerAnswerTimerLength(1.25f),
	playerAnswerTimerLength(basePlayerAnswerTimerLength),
	playerAnswerTimer(0),
	playerResult(false),
	playerResultEffective(false),
	playerResultTimerLength(1.5f),
	playerResultTimer(0.f),
	enemyCursor(new Sprite(_shader)),
	prevHighlightedPunctuation(nullptr),
	highlightedPunctuation(nullptr),
	punctuationHighlight(new Sprite(_shader)),
	highlightedWordStart(nullptr),
	highlightedWordEnd(nullptr),
	wordHighlight(new Sprite(_shader)),
	complimentBubble(new Sprite(_shader)),
	complimentBubbleTimerBaseLength(1.f),
	complimentBubbleTimerLength(1.f),
	complimentBubbleTimer(0.f),
	complimentBubbleScale(1.f),
	interjectBubble(new Sprite(_shader)),
	interjected(false),
	interjectBubbleTimerBaseLength(1.f),
	interjectBubbleTimerLength(1.f),
	interjectBubbleTimer(0.f),
	shader(_shader),
	baseCursorDelayLength(0.05f),
	baseCursorPunctDelayLength(0.15f),
	cursorDelayLength(0.f),
	cursorDelayDuration(0.f),
	baseGlyphWidth(_font->getGlyphAdvance('m').x),
	glyphIdx(0),
	confidence(50.f),
	isGameOver(false),
	gameOverLength(1.f),
	gameOverDuration(0.f),
	win(false),
	isComplete(false),
	offensiveCorrect(0),
	offensiveWrong(0),
	defensiveCorrect(0),
	defensiveWrong(0),
	punctuationCnt(-1),
	interjectTimer(0),
	damage(10.f),
	playerComboMultiplier(1.f),
	enemyComboMultiplier(1.f),
	keyboard(&Keyboard::getInstance()),
	enemy(nullptr),
	modeOffensive(true),
	player(_player),
	playerAttackMultiplier(1.f),
	enemyAttackMultiplier(1.f),
	insightMultiplier(1.f),
	insightAlpha(1.f),
	sassInsultMultiplier(1.f),
	sassInterjectMultiplier(1.f),
	playerComboIncrement(0),
	enemyComboIncrement(0),
	optionOneShader(new ComponentShaderText(true)),
	optionTwoShader(new ComponentShaderText(true))
{
	verticalAlignment = kTOP;
	horizontalAlignment = kCENTER;
	background->setVisible(false);

	float borderSize = sweet::getWindowHeight() * 0.1f / 2.f;

	healthContainer = new VerticalLinearLayout(_bulletWorld);
	addChild(healthContainer);
	healthContainer->setRationalWidth(1.f, this);
	healthContainer->setRationalHeight(0.15f, this);
	healthContainer->horizontalAlignment = kCENTER;
	healthContainer->verticalAlignment = kMIDDLE;
	//healthContainer->setBackgroundColour(0, 1.f, 0.541f);

	displayContainer = new NodeUI(_bulletWorld);
	addChild(displayContainer);
	displayContainer ->setRationalWidth(1.f, this);
	displayContainer ->setRationalHeight(0.85f, this);
	displayContainer ->background->setVisible(false);

	gameContainer = new NodeUI(_bulletWorld);
	displayContainer->addChild(gameContainer);
	gameContainer->setRationalWidth(1.f, displayContainer);
	gameContainer->setRationalHeight(1.f, displayContainer);
	gameContainer->background->setVisible(false);
	//gameContainer->setBackgroundColour(0, 0.714f, 0.929f);

	// Enemy Cursor
	enemyCursor->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-CURSOR")->texture);
	enemyCursor->mesh->setScaleMode(GL_NEAREST);
	enemyCursor->childTransform->scale(20.f);
	childTransform->addChild(enemyCursor);

	// move the cusrosr's mesh up so that the origin is aligned with the top
	for (unsigned long int i = 0; i < enemyCursor->mesh->vertices.size(); ++i){
		enemyCursor->mesh->vertices.at(i).y -= 0.5f;
	}

	// Punctuation Highlight
	punctuationHighlight->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-HIGHLIGHT")->texture);
	punctuationHighlight->mesh->setScaleMode(GL_NEAREST);
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

		wordHighlight->mesh->vertices.at(i).red = 0.820f;
		wordHighlight->mesh->vertices.at(i).green = 0.722f;
		wordHighlight->mesh->vertices.at(i).blue = 0.851f;
	}
	wordHighlight->setVisible(false);

	livesContainer = new HorizontalLinearLayout(_bulletWorld);
	healthContainer->addChild(livesContainer);
	//livesContainer->setBackgroundColour(0.5f, 1.f, 0.5f);
	livesContainer->setRationalWidth(1.f, healthContainer);
	livesContainer->setRationalHeight(0.5f, healthContainer);
	livesContainer->horizontalAlignment = kLEFT;
	livesContainer->verticalAlignment = kTOP;

	// healthbar
	confidenceSlider = new SliderControlled(_bulletWorld, &confidence, 0, 100.f);
	healthContainer->addChild(confidenceSlider);
	//confidenceSlider->setBackgroundColour(1.f, 0, 0);
	//confidenceSlider->fill->setBackgroundColour(0, 1.f, 0);
	confidenceSlider->setRationalWidth(0.7f, healthContainer);
	confidenceSlider->setRationalHeight(0.5f, healthContainer);

	//confidenceSlider->thumb->background->meshTransform->scale(2);
	confidenceSlider->thumb->setBackgroundColour(1,1,1,1);
	confidenceSlider->thumb->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-SLIDER-THUMB-HAPPY")->texture);
	confidenceSlider->thumb->background->mesh->setScaleMode(GL_NEAREST);

	confidenceSlider->setBackgroundColour(1,1,1,1);
	confidenceSlider->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-SLIDER-TRACK")->texture);
	confidenceSlider->background->mesh->setScaleMode(GL_NEAREST);

	confidenceSlider->fill->setBackgroundColour(1,1,1,1);
	confidenceSlider->fill->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-SLIDER-FILL")->texture);
	confidenceSlider->fill->background->mesh->setScaleMode(GL_NEAREST);

	enemyBubble = new NodeUI(_bulletWorld);
	gameContainer->addChild(enemyBubble);
	enemyBubble->setRationalWidth(1.f, gameContainer);
	enemyBubble->setRationalHeight(1.f, gameContainer);
	enemyBubble->setMarginTop(0.4f);
	enemyBubble->setMarginBottom(0.3f);
	enemyBubble->background->setVisible(false);
	enemyBubble->setBackgroundColour(0.5, 0.5, 0.5, 1.f);
	enemyBubble->setMarginRight(0.05f);
	enemyBubble->setMarginLeft(0.55f);

	NodeUI_NineSliced * enemyBubbleBubble = new NodeUI_NineSliced(_bulletWorld, PD_ResourceManager::scenario->getNineSlicedTexture("NPC-BUBBLE"));
	enemyBubble->addChild(enemyBubbleBubble);
	enemyBubbleBubble->setBorder(borderSize);
	enemyBubbleBubble->setRationalWidth(1.f, enemyBubble);
	enemyBubbleBubble->setRationalHeight(1.f, enemyBubble);
	enemyBubbleBubble->setMarginLeft(0.15f);
	enemyBubbleBubble->setPadding(0.05f);
	enemyBubbleBubble->setScaleMode(GL_NEAREST);
	//enemyBubbleBubble->setBackgroundColour(0.f, 1.f, 0.f, 0.5f);

	NodeUI * enemyBubbleTextContainer = new NodeUI(_bulletWorld);
	enemyBubbleBubble->addChild(enemyBubbleTextContainer);
	enemyBubbleTextContainer->setRationalWidth(1.f, enemyBubbleTextContainer->nodeUIParent);
	enemyBubbleTextContainer->setRationalHeight(1.f, enemyBubbleTextContainer->nodeUIParent);
	enemyBubbleTextContainer->setBackgroundColour(1.f,0, 0, 0.5);
	enemyBubbleTextContainer->background->setVisible(false);
	enemyBubbleTextContainer->setPadding(0.05f);

	enemyBubbleTextContainer->uiElements->addChild(wordHighlight);
	enemyBubbleTextContainer->uiElements->addChild(punctuationHighlight);

	enemyBubbleText = new TextArea(world, _font, _textShader);
	enemyBubbleText->setRenderMode(kTEXTURE);
	enemyBubbleTextContainer->addChild(enemyBubbleText);
	enemyBubbleText->setWrapMode(kWORD);
	enemyBubbleText->setRationalWidth(1.f, enemyBubbleTextContainer);
	enemyBubbleText->setRationalHeight(1.f, enemyBubbleTextContainer);
	enemyBubbleText->horizontalAlignment = kCENTER;
	enemyBubbleText->verticalAlignment = kMIDDLE;
	enemyBubbleText->background->setVisible(false);
	enemyBubbleText->setBackgroundColour(1, 1, 1, 0.5);

	NodeUI * enemyBubbleTail = new NodeUI(_bulletWorld);
	enemyBubble->addChild(enemyBubbleTail);
	enemyBubbleTail->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-DEFENSE-BUBBLE-TAIL")->texture);
	enemyBubbleTail->setRationalWidth(1.f, enemyBubble);
	enemyBubbleTail->setRationalHeight(1.f, enemyBubble);
	enemyBubbleTail->setMarginRight(0.835f);
	enemyBubbleTail->setMarginTop(0.3f);
	enemyBubbleTail->setMarginBottom(0.3f);
	//enemyBubbleTail->setBackgroundColour(1.f, 0, 0, 0.5f);
	enemyBubbleTail->background->mesh->setScaleMode(GL_NEAREST);

	playerBubble = new NodeUI(_bulletWorld);
	gameContainer->addChild(playerBubble);
	playerBubble->setRationalWidth(1.f, gameContainer);
	playerBubble->setRationalHeight(1.f, gameContainer);
	playerBubble->setMarginTop(0.5f);
	playerBubble->setMarginBottom(0.05f);
	playerBubble->background->setVisible(false);
	playerBubble->setBackgroundColour(1, 0, 0, 0.5f);
	playerBubble->setMarginRight(0.05f);
	playerBubble->setMarginLeft(0.4f);

	playerTimerSlider = new SliderControlled(_bulletWorld, &playerAnswerTimer, 0, playerAnswerTimerLength, true, true);
	playerBubble->addChild(playerTimerSlider);
	playerTimerSlider->boxSizing = kCONTENT_BOX;
	playerTimerSlider->setRationalWidth(0.7f, playerBubble);
	playerTimerSlider->setRationalHeight(0.1f, playerBubble);
	playerTimerSlider->setBackgroundColour(1,1,1,1);
	playerTimerSlider->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("SLIDER-TRACK")->texture);
	playerTimerSlider->background->mesh->setScaleMode(GL_NEAREST);
	playerTimerSlider->fill->setBackgroundColour(1,1,1,1);
	playerTimerSlider->fill->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-SLIDER-FILL")->texture);
	playerTimerSlider->fill->background->mesh->setScaleMode(GL_NEAREST);
	playerTimerSlider->thumb->setVisible(false);
	playerTimerSlider->setMarginLeft(0.3f);
	playerTimerSlider->setMarginBottom(1.f);

	// The fancy nine sliced bubble
	NodeUI_NineSliced * playerBubbleBubble = new NodeUI_NineSliced(_bulletWorld, PD_ResourceManager::scenario->getNineSlicedTexture("PLAYER-BUBBLE"));
	playerBubble->addChild(playerBubbleBubble);
	playerBubbleBubble->setBorder(borderSize);
	playerBubbleBubble->setRationalWidth(1.f, playerBubble);
	playerBubbleBubble->setRationalHeight(1.f, playerBubble);
	playerBubbleBubble->setMarginLeft(0.3f);
	playerBubbleBubble->setMarginBottom(0.3f);
	//playerBubbleBubble->setBackgroundColour(0, 0.1f, 0, 0.5f);
	playerBubbleBubble->setScaleMode(GL_NEAREST);

	// The side by side text and button layout
	playerBubbleLayout = new HorizontalLinearLayout(_bulletWorld);
	playerBubbleBubble->addChild(playerBubbleLayout);
	playerBubbleLayout->verticalAlignment = kMIDDLE;
	playerBubbleLayout->horizontalAlignment = kCENTER;
	playerBubbleLayout->setRationalWidth(1.f, playerBubbleBubble);
	playerBubbleLayout->setRationalHeight(1.f, playerBubbleBubble);
	playerBubbleLayout->setPadding(0.05f);
	playerBubbleLayout->background->setVisible(false);
	playerBubbleLayout->setBackgroundColour(1, 1, 1, 0.5f);

	playerBubbleText = new TextArea(world, _font, _textShader);
	playerBubbleText->setRenderMode(kTEXTURE);
	playerBubbleText->setWrapMode(kWORD);
	playerBubbleLayout->addChild(playerBubbleText);
	playerBubbleText->setRationalWidth(PLAYER_TEXT_WIDTH, playerBubbleLayout);
	playerBubbleText->setRationalHeight(1.0f, playerBubbleLayout);
	playerBubbleText->horizontalAlignment = kCENTER;
	playerBubbleText->verticalAlignment = kMIDDLE;
	playerBubbleText->background->setVisible(false);
	playerBubbleText->setBackgroundColour(1.f, 0, 0, 0.5f);

	playerBubbleOptions = new HorizontalLinearLayout(_bulletWorld);
	playerBubbleLayout->addChild(playerBubbleOptions);

	playerBubbleOptions->setRationalWidth(0.5f, playerBubbleLayout);
	playerBubbleOptions->setRationalHeight(1.f, playerBubbleLayout);
	playerBubbleOptions->setBackgroundColour(0, 1.f, 0, 0.5f);
	playerBubbleOptions->background->setVisible(false);
	playerBubbleOptions->horizontalAlignment = kCENTER;
	playerBubbleOptions->verticalAlignment = kMIDDLE;

	Texture * arrowsTex = PD_ResourceManager::scenario->getTexture("DISS-TUTORIAL-ARROWS")->texture;
	NodeUI * playerArrows = new NodeUI(_bulletWorld);
	playerBubbleOptions->addChild(playerArrows);
	playerArrows->background->mesh->pushTexture2D(arrowsTex);
	playerArrows->setRationalWidth(0.2f, playerBubbleOptions);
	playerArrows->setSquareHeight((float)arrowsTex->height/arrowsTex->width);
	playerArrows->background->mesh->setScaleMode(GL_NEAREST);

	VerticalLinearLayout * buttonLayout = new VerticalLinearLayout(_bulletWorld);
	playerBubbleOptions->addChild(buttonLayout);
	buttonLayout->setRationalWidth(0.8f, playerBubbleOptions);
	buttonLayout->setRationalHeight(0.75f, playerBubbleOptions);
	buttonLayout->verticalAlignment = kMIDDLE;
	buttonLayout->setPaddingLeft(0.05f);

	pBubbleBtn1 = new PD_InsultButton(_bulletWorld, _font, optionOneShader);
	buttonLayout->addChild(pBubbleBtn1);
	pBubbleBtn1->setRationalWidth(1.f, buttonLayout);
	pBubbleBtn1->setRationalHeight(0.5f, buttonLayout);
	pBubbleBtn1->setPadding(0.f, 0.1f);
	pBubbleBtn1->setMarginBottom(0.1f);
	pBubbleBtn1->setMouseEnabled(false);

	pBubbleBtn2 = new PD_InsultButton(_bulletWorld, _font, optionTwoShader);
	buttonLayout->addChild(pBubbleBtn2);
	pBubbleBtn2->setRationalWidth(1.f, buttonLayout);
	pBubbleBtn2->setRationalHeight(0.5f, buttonLayout);
	pBubbleBtn2->setPadding(0.f, 0.1f);
	pBubbleBtn2->setMarginTop(0.1f);
	pBubbleBtn2->setMouseEnabled(false);

	NodeUI * playerBubbleTail = new NodeUI(_bulletWorld);
	playerBubble->addChild(playerBubbleTail);
	playerBubbleTail->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-OFFENSE-BUBBLE-TAIL")->texture);
	playerBubbleTail->setRationalWidth(1.f, playerBubble);
	playerBubbleTail->setRationalHeight(1.f, playerBubble);
	playerBubbleTail->setMarginRight(0.69f);
	playerBubbleTail->setMarginTop(0.3f);
	playerBubbleTail->setMarginBottom(0.03f);
	playerBubbleTail->background->mesh->setScaleMode(GL_NEAREST);

	gameOverContainer = new VerticalLinearLayout(_bulletWorld);
	displayContainer->addChild(gameOverContainer);
	gameOverContainer->horizontalAlignment = kCENTER;
	gameOverContainer->verticalAlignment = kMIDDLE;
	gameOverContainer->setRationalHeight(1.f, displayContainer);
	gameOverContainer->setRationalWidth(1.f, displayContainer);
	gameOverContainer->setPadding(0.f, 0.1f);
	gameOverContainer->setVisible(false);

	gameOverImage = new NodeUI(_bulletWorld);
	gameOverContainer->addChild(gameOverImage);
	gameOverImage->setRationalHeight(0.5f, gameOverContainer);
	gameOverImage->setSquareWidth(1.f);
	gameOverImage->background->mesh->setScaleMode(GL_NEAREST);

	complimentBubble->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-COMPLIMENT1")->texture);
	complimentBubble->mesh->setScaleMode(GL_NEAREST);
	complimentBubble->childTransform->scale(sweet::getWindowHeight() * 0.5, sweet::getWindowHeight() * 0.5, 0);
	complimentBubble->meshTransform->scale(0, 0, 0);
	complimentBubble->childTransform->translate(sweet::getWindowWidth() * 0.2, 0, 0);
	complimentBubble->setVisible(false);

	// move the interject bubble's mesh up so that the origin is aligned with the bottom
	for (unsigned long int i = 0; i < complimentBubble->mesh->vertices.size(); ++i){
		complimentBubble->mesh->vertices.at(i).x += 0.5f;
		complimentBubble->mesh->vertices.at(i).y += 0.5f;
	}
	childTransform->addChild(complimentBubble);

	interjectBubble->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-INTERJECT")->texture);
	interjectBubble->mesh->setScaleMode(GL_NEAREST);
	interjectBubble->childTransform->scale(sweet::getWindowHeight() * 0.6, sweet::getWindowHeight() * 0.6, 0);
	interjectBubble->meshTransform->scale(0, 0, 0);
	interjectBubble->childTransform->translate(sweet::getWindowWidth() * 0.6, 0, 0);
	interjectBubble->setVisible(false);

	// move the interject bubble's mesh up so that the origin is aligned with the bottom
	for (unsigned long int i = 0; i < interjectBubble->mesh->vertices.size(); ++i){
		interjectBubble->mesh->vertices.at(i).x += 0.5f;
		interjectBubble->mesh->vertices.at(i).y += 0.5f;
	}
	childTransform->addChild(interjectBubble);

	tutorialSpacebar = new HorizontalLinearLayout(_bulletWorld);
	gameContainer->addChild(tutorialSpacebar);
	tutorialSpacebar->setRationalWidth(1.f, gameContainer);
	tutorialSpacebar->setRationalHeight(0.4f, gameContainer);
	tutorialSpacebar->verticalAlignment = kMIDDLE;
	tutorialSpacebar->horizontalAlignment = kCENTER;
	tutorialSpacebar->setMarginLeft(0.6f);
	tutorialSpacebar->setVisible(false);

	tutorialSpacebarImage = new NodeUI(_bulletWorld);
	tutorialSpacebar->addChild(tutorialSpacebarImage);
	tutorialSpacebarImage->setRationalHeight(1.f, tutorialSpacebar);
	tutorialSpacebarImage->setSquareWidth(1.f);
	tutorialSpacebarImage->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISS-TUTORIAL-SPACEBAR")->texture);
	tutorialSpacebarImage->background->mesh->setScaleMode(GL_NEAREST);

	// disable and hide by default
	disable();

	eventManager->addEventListener("wordspoken", [this](sweet::Event * _event){
		// Stuff!!!
	});

	// Init sound vectors
	for(unsigned long int i = 1; i < 11; ++i) {
		missInterjectSounds.push_back(PD_ResourceManager::scenario->getAudio("slap" + std::to_string(i))->sound);
	}

	for(unsigned long int i = 1; i < NUM_SUCCEED_INSULT; ++i) {
		succeedInsultSounds.push_back(PD_ResourceManager::scenario->getAudio(SUCCEED_INSULT + std::to_string(i))->sound);
	}
	
	PD_ResourceManager::scenario->getAudio(TIMER)->sound->setGain(3);

	optionOneShader->incrementReferenceCount();
	optionTwoShader->incrementReferenceCount();
	optionOneShader->name = "PD_UI_DissBattle text shader (option one)";
	optionTwoShader->name = "PD_UI_DissBattle text shader (option two)";


	eventManager->addEventListener("fightStarted", [this](sweet::Event * _event){
		if(modeOffensive){
			playerBubbleText->invalidateLayout();
		}else{
			enemyBubbleText->invalidateLayout();
		}
	});
}

PD_UI_DissBattle::~PD_UI_DissBattle(){
	while(lifeTokens.size() > 0){
		lifeTokens.back()->decrementAndDelete();
		lifeTokens.pop_back();
	}while(lifeTokensCrossed.size() > 0){
		lifeTokensCrossed.back()->decrementAndDelete();
		lifeTokensCrossed.pop_back();
	}

	// put elements which may not be in the hierarcy back in so they're deleted
	playerBubbleLayout->removeChild(playerBubbleOptions, false);
	playerBubbleLayout->addChild(playerBubbleOptions, false);

	optionOneShader->decrementAndDelete();
	optionTwoShader->decrementAndDelete();

	if(enemyCursor->firstParent()->parents.size() == 0){
		childTransform->addChild(enemyCursor->firstParent(), false);
	}
}

void PD_UI_DissBattle::update(Step * _step){
	if(isEnabled()){
		if(!isGameOver){
			if(modeOffensive && playerQuestionTimer >= playerQuestionTimerLength && !playerResult){
				if(player->wantsToInsultUp()){
					insult(pBubbleBtn1->isEffective, pBubbleBtn1->label->getText(false));
				}else if(player->wantsToInsultDown()){
					insult(pBubbleBtn2->isEffective, pBubbleBtn2->label->getText(false));
				}
			}else{
				if (canInterject && player->wantsToInterject()){
					interject();
				}

				if(player->interjecting()){
					tutorialSpacebarImage->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("DISS-TUTORIAL-SPACEBAR-PRESSED")->texture);
				}else{
					tutorialSpacebarImage->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("DISS-TUTORIAL-SPACEBAR")->texture);
				}
			}

			if(!modeOffensive){
				enemy->pr->talking = true;
				// update interject bubble
				if(interjectBubble->isVisible()){
					if(interjectBubbleTimer >= interjectBubbleTimerLength){
						// hide bubble
						interjectBubble->setVisible(false);
						if(interjected){
							// Switch to player turn
							setUIMode(true);
						}
					}else{
						interjectBubbleTimer += _step->getDeltaTime();

						float s = 0.f;
						// animate bubble
						if(!interjected){
							s = interjectBubbleTimer / interjectBubbleTimerLength <= 0.5 ? Easing::easeOutCubic(interjectBubbleTimer, 0, interjectBubbleScale, interjectBubbleTimerLength * 0.5f) : Easing::easeInCubic(interjectBubbleTimer - interjectBubbleTimerLength * 0.5f, interjectBubbleScale, -interjectBubbleScale, interjectBubbleTimerLength * 0.5f);
						}else{
							s = interjectBubbleTimer / interjectBubbleTimerLength <= 0.7 ? Easing::easeOutElastic(interjectBubbleTimer, 0, interjectBubbleScale, interjectBubbleTimerLength * 0.7f) : Easing::easeInBack(interjectBubbleTimer - interjectBubbleTimerLength * 0.7f, interjectBubbleScale, -interjectBubbleScale, interjectBubbleTimerLength * 0.3f);
						}
						interjectBubble->meshTransform->scale(s, s, 1, false);
					}
				}

				// update enemy turn
				if(!interjected){
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
								sweet::Event * e = new sweet::Event("miss");
								eventManager->triggerEvent(e);

								prevHighlightedPunctuation = highlightedPunctuation;
								highlightedPunctuation = findFirstPunctuation(glyphIdx+1);
							}

							if(highlightedWordEnd != nullptr && glyphs.at(glyphIdx) == highlightedWordEnd){
								highlightNextWord(glyphIdx+1);
							}

							++glyphIdx;
							cursorDelayDuration = 0;

							if(glyphIdx < glyphs.size()){
								// set cursor delay for this glyph
								cursorDelayLength = glyphs.at(glyphIdx)->getWidth() / baseGlyphWidth * (glyphs.at(glyphIdx) != highlightedPunctuation ? baseCursorDelayLength * (1.f + (sassInterjectMultiplier - 1.f) * 0.5f) : baseCursorPunctDelayLength * sassInterjectMultiplier);
								
								// play sound
								if(glyphIdx == 1 || glyphs.at(glyphIdx - 1)->character == ' '){
									auto sound = enemy->voice;
									sound->setPitch((glyphs.at(glyphIdx)->character-100.f)/100.f+1.0f);
									sound->play();
								}
							}else{
								// Enemy's insult effective!
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
				}
			}else{
				enemy->pr->talking = false;
				// update compliment bubble
				if(complimentBubble->isVisible()){
					if(complimentBubbleTimer >= complimentBubbleTimerLength){
						complimentBubble->setVisible(false);
					}else{
						complimentBubbleTimer += _step->getDeltaTime();
						float s = 0.f;
						s = complimentBubbleTimer / complimentBubbleTimerLength <= 0.7 ? Easing::easeOutElastic(complimentBubbleTimer, 0, complimentBubbleScale, complimentBubbleTimerLength * 0.7f) : Easing::easeInBack(complimentBubbleTimer - complimentBubbleTimerLength * 0.7f, complimentBubbleScale, -complimentBubbleScale, complimentBubbleTimerLength * 0.3f);
						complimentBubble->meshTransform->scale(s, s, 1, false);
					}
				}

				// INSULT
				if(playerQuestionTimer >= playerQuestionTimerLength){
					if(!playerResult){
						// Increment player answer timer
						if(playerAnswerTimer >= playerAnswerTimerLength){
							// Out of time, enemy's turn!
							countInsultAccuracy(-1);
							// Reset multipier for failed insult damage
							playerComboMultiplier = 1.f;
							incrementConfidence(-damage);
							PD_ResourceManager::scenario->getAudio(TIMER)->sound->stop();
							PD_ResourceManager::scenario->getAudio(PASSED_INSULT_TIME_LIMIT)->sound->play();
							setUIMode(false);
						}else{
							playerAnswerTimer += _step->getDeltaTime();
						}
					}else{
						// Player Insult Result
						// Increment player result timer
						if(playerResultTimer >= playerResultTimerLength){
							if (playerResultEffective){
								// Insult Success!
								// Reset enemy combo multipier if first insult was effective
								if(enemyComboMultiplier > 1.f){
									enemyComboMultiplier = 1.f;
								}
								// Get next insult
								incrementConfidence(damage);
								setPlayerText();
								playerInsultSpeedMultiplier -= (1.f - MIN_OFFENSE_SPEED_MULTIPLIER) / NUM_OFFENSE_SPEED_CHANGES;

								playerAnswerTimerLength = basePlayerAnswerTimerLength *  sassInsultMultiplier * playerInsultSpeedMultiplier;
								playerQuestionTimerLength = basePlayerQuestionTimerLength * playerInsultSpeedMultiplier;
								playerTimerSlider->setValueMax(playerAnswerTimerLength);
							}
							else{
								//Insult Failed
								// Reset multipier for failed insult
								playerComboMultiplier = 1.f;
								incrementConfidence(-damage);
								setUIMode(false);
							}
						}else{
							playerResultTimer += _step->getDeltaTime();
						}
					}
				}else{
					// Increment player question timer
					playerQuestionTimer += _step->getDeltaTime();
					if(playerQuestionTimer >= playerQuestionTimerLength){
						playerBubbleOptions->setVisible(true);
						playerTimerSlider->setVisible(true);
						PD_ResourceManager::scenario->getAudio(TIMER)->sound->play(true);
					}
				}
			}
		}else{
			gameOverDuration += _step->getDeltaTime();

			if(gameOverDuration >= gameOverLength){
				complete();
			}else{
				float p = gameOverDuration / gameOverLength;
				if(p < 0.75f){
					float size = Easing::easeOutElastic(p, 0.5f, 0.5f, gameOverLength * 0.75f);
					gameOverImage->setRationalHeight(size, gameOverContainer);
					gameOverContainer->invalidateLayout();
				}
			}
		}

		canInterject = !interjectBubble->isVisible() && !modeOffensive;
		VerticalLinearLayout::update(_step);
	}
}

void PD_UI_DissBattle::startNewFight(PD_Character * _enemy, bool _playerFirst){
	// clear existing friendship
	for(unsigned int i = 0; i < lives.size(); ++i){
		livesContainer->removeChild(lives.at(i));
		delete lives.at(i);
	}
	for(unsigned int i = 0; i < lostLives.size(); ++i){
		livesContainer->removeChild(lostLives.at(i));
		delete lostLives.at(i);
	}
	lives.clear();
	lostLives.clear();

	enemy = _enemy;

	// loop through friends and add tokens
	for(unsigned int i = 0; i < lifeTokens.size(); ++i){
		NodeUI * l = new NodeUI(world);
		livesContainer->addChild(l);
		Texture * tex = lifeTokens.at(i);
		l->background->mesh->pushTexture2D(tex);
		l->background->mesh->setScaleMode(GL_NEAREST);
		l->setRationalHeight(1.f, livesContainer);
		l->setSquareWidth(1.f);
		l->boxSizing = kCONTENT_BOX;
		l->setMarginLeft(5);
		l->setMarginRight(5);
		l->firstParent()->scale(1.f, 1.f, 1.f);
		lives.push_back(l);
	}

	confidence = 50.f;
	playerComboMultiplier = 1.f;
	enemyComboMultiplier = 1.f;

	setupDissValues();

	if(isGameOver){
		// Reset layout
		gameOverImage->setRationalHeight(0.5f, gameOverContainer);
		gameOverContainer->setVisible(false);
		gameOverDuration = 0;

		gameContainer->setVisible(true);
		childTransform->addChild(enemyCursor->firstParent(), false);
		interjectBubble->setVisible(true);

		win = false;
		isGameOver = false;
	}
	isComplete = false;

	interjected = false;
	interjectBubble->setVisible(false);
	setUIMode(_playerFirst);
	enable();

	eventManager->triggerEvent("fightStarted");
}

void PD_UI_DissBattle::setupDissValues(){
	// read diss values
	DissBattleValues dissValues;

	// Player's attack: increased if strength outweights enemy's defence, and lowered otherwise
	playerAttackMultiplier = (player->dissStats->getStrength() - enemy->dissStats->getDefense()) / MAX_DISS_LEVEL;
	if(playerAttackMultiplier > 0){
		playerAttackMultiplier = sweet::NumberUtils::map(playerAttackMultiplier, 0.f, 1.f, dissValues.playerAttackMultiplier[0], dissValues.playerAttackMultiplier[1]);
	}else{
		playerAttackMultiplier = sweet::NumberUtils::map(playerAttackMultiplier, 0.f, -1.f, dissValues.playerAttackMultiplier[2], dissValues.playerAttackMultiplier[3]);
	}

	// Enemy's attack: increased if strength outweights player's defence, and lowered otherwise
	enemyAttackMultiplier = (enemy->dissStats->getStrength() - player->dissStats->getDefense()) / MAX_DISS_LEVEL;
	if(enemyAttackMultiplier > 0){
		enemyAttackMultiplier = sweet::NumberUtils::map(enemyAttackMultiplier, 0.f, 1.f, dissValues.enemyAttackMultiplier[0], dissValues.enemyAttackMultiplier[1]);
	}else{
		enemyAttackMultiplier = sweet::NumberUtils::map(enemyAttackMultiplier, 0.f, -1.f, dissValues.enemyAttackMultiplier[2], dissValues.enemyAttackMultiplier[3]);
	}

	insightMultiplier = (player->dissStats->getInsight() - enemy->dissStats->getInsight()) / MAX_DISS_LEVEL; // -1 to 1
	if(insightMultiplier > 0){
		insightAlpha = sweet::NumberUtils::map(insightMultiplier, 0.f, 1.f, dissValues.insightAlpha[0], dissValues.insightAlpha[1]);
	}else{
		insightAlpha = sweet::NumberUtils::map(insightMultiplier, 0.f, -1.f, dissValues.insightAlpha[2], dissValues.insightAlpha[3]);
	}

	float sassMultiplier = (player->dissStats->getSass() - enemy->dissStats->getSass()) / MAX_DISS_LEVEL; // -1 to 1
	if(sassMultiplier > 0){
		sassInsultMultiplier = sweet::NumberUtils::map(sassMultiplier, 0.f, 1.f, dissValues.sassInsultMultiplier[0], dissValues.sassInsultMultiplier[1]);
		sassInterjectMultiplier = sweet::NumberUtils::map(sassMultiplier, -1.f, 1.f, dissValues.sassInterjectMultiplier[0], dissValues.sassInterjectMultiplier[1]); // 0.75 to 1.5
	}else{
		sassInsultMultiplier = sweet::NumberUtils::map(sassMultiplier, 0.f, -1.f, dissValues.sassInsultMultiplier[2], dissValues.sassInsultMultiplier[3]);
		sassInterjectMultiplier = sweet::NumberUtils::map(sassMultiplier, -1.f, 1.f, dissValues.sassInterjectMultiplier[2], dissValues.sassInterjectMultiplier[3]); // 0.75 to 1.5
	}
	playerComboIncrement = dissValues.playerComboIncrement;
	enemyComboIncrement = dissValues.enemyComboIncrement;
}

void PD_UI_DissBattle::gameOver(bool _win){
	sweet::Event * e = new sweet::Event("gameover");
	e->setIntData("win", _win);
	eventManager->triggerEvent(e);

	isGameOver = true;
	win = _win;
	enemy->dissedAt = true;
	enemy->wonDissBattle = !win;

	gameContainer->setVisible(false);
	childTransform->removeChild(enemyCursor->firstParent());
	interjectBubble->setVisible(false);

	prevXP = player->experience;
	if(_win){
		int sumP = player->dissStats->getDefense() + player->dissStats->getInsight() + player->dissStats->getStrength() + player->dissStats->getSass();
		int sumE = enemy->dissStats->getDefense() + enemy->dissStats->getInsight() + enemy->dissStats->getStrength() + enemy->dissStats->getSass();
		float res = (sumE - sumP) / 40.f;
		if(res < 0){
			res = 0.f;
		}
		float xpMultipier = sweet::NumberUtils::map(res, 0.f, 1.f, 1.f, 3.f);

		wonXP = 100.f / ((player->level + 1) * 3.f) * xpMultipier; 
		gameOverImage->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-WIN")->texture);
	}else{
		wonXP = 0.f;
		gameOverImage->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-LOSE")->texture);
	}

	gameOverContainer->setVisible(true);
	//invalidateLayout();
	// USER TESTING INFORMATION
	sweet::FileUtils::createDirectoryIfNotExists("data/DissBattleResults");
	{
		std::stringstream filename;
		std::ofstream file;

		// success rate
		filename << "data/DissBattleResults/" << sweet::DateUtils::getDatetime() << "_DissBattleResult_SuccessRate.csv";
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

		filename << "data/DissBattleResults/" << sweet::DateUtils::getDatetime() << "_DissBattleResult_Interject.csv";
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
		filename << "data/DissBattleResults/" << sweet::DateUtils::getDatetime() << "_DissBattleResult_Insult.csv";
		file.open(filename.str());

		file << "playerAnswerTimerLength FAILED (Out of time): -1;hitTime;" << std::endl;
		for(auto i : insultTimes){
			file << playerAnswerTimerLength << ";" << i << std::endl;
		}
		file.close();
	}
}

void PD_UI_DissBattle::complete(){
	if(!isComplete){
		sweet::Event * e = new sweet::Event("complete");
		e->setIntData("win", win);
		eventManager->triggerEvent(e);
		isComplete = true;
		enemy->pr->talking = false;
		enemy = nullptr;
	}
}

void PD_UI_DissBattle::addLife(Texture * _tokenTexture){
	// create a duplicate token with a cross through it
	ProgrammaticTexture * tex = new ProgrammaticTexture();
	tex->name = "Life Token Crossed";
	tex->allocate(_tokenTexture->width, _tokenTexture->height, _tokenTexture->channels);

	for(signed long int y = 0; y < tex->height; ++y){
		for(signed long int x = 0; x < tex->width; ++x){
			if(glm::abs(x-y) < tex->width/10 || glm::abs(tex->width - x-y) < tex->width/10){
				sweet::TextureUtils::setPixel(tex, x, y, glm::uvec4(255,0,0,255));
			}else{
				unsigned char * p1 = sweet::TextureUtils::getPixelPointer(tex, x, y);
				unsigned char * p2 = sweet::TextureUtils::getPixelPointer(_tokenTexture, x, y);
				*(p1+0) = *(p2+0);
				*(p1+1) = *(p2+1);
				*(p1+2) = *(p2+2);
				*(p1+3) = *(p2+3) * 0.5f; // reduce the alpha a bit
			}
		}
	}
	tex->load();

	// save both tokens
	lifeTokens.push_back(_tokenTexture);
	lifeTokensCrossed.push_back(tex);

	tex->incrementReferenceCount();
	_tokenTexture->incrementReferenceCount();
}

void PD_UI_DissBattle::disable(){
	setVisible(false);
	enabled = false;
	invalidateLayout();
}

void PD_UI_DissBattle::enable(){
	setVisible(true);
	enabled = true;
	invalidateLayout();
}

bool PD_UI_DissBattle::isEnabled() {
	return enabled;
}

void PD_UI_DissBattle::interject(){
	// Determine Success
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

	// Get User Test data
	countButtonPresses(isPunctuation, false);
	countInterjectAccuracy(interjectTimer);

	// Get interject bubble animation ready
	interjectBubbleTimerLength = interjectBubbleTimerBaseLength * (isPunctuation ? 1.f : 0.25f);
	interjectBubbleTimer = 0.f;
	interjectBubbleScale = isPunctuation ? 1.f : 0.4f;
	interjectBubble->meshTransform->scale(0, 0, 0, false);
	interjectBubble->setVisible(true);

	// Trigger interject event
	sweet::Event * e = new sweet::Event("interject");
	e->setIntData("success", isPunctuation);

	if(!isPunctuation){
		// Play sound effect for missing
		auto sound = sweet::NumberUtils::randomItem(missInterjectSounds);
		sound->play();
	}else {
		PD_ResourceManager::scenario->getAudio(INTERJECT)->sound->play();
	}

	eventManager->triggerEvent(e);

	// Set flag if interjectionn was successful
	interjected = isPunctuation;
	canInterject = false;
}

void PD_UI_DissBattle::setUIMode(bool _isOffensive){
	confidenceSlider->thumb->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture(std::string("DISS-BATTLE-SLIDER-THUMB-") + (_isOffensive ? "HAPPY" : "SAD"))->texture);

	modeOffensive = _isOffensive;
	enemyBubble->setVisible(!_isOffensive);
	enemyCursor->setVisible(!_isOffensive);

	playerBubble->setVisible(_isOffensive);

	tutorialSpacebar->setVisible(!_isOffensive);

	if (!_isOffensive){
		interjected = false;
		setEnemyText();
	}
	else{
		playerInsultSpeedMultiplier = 1.f;
		playerQuestionTimerLength = basePlayerQuestionTimerLength;
		playerAnswerTimerLength = basePlayerAnswerTimerLength * sassInsultMultiplier;
		playerTimerSlider->setValueMax(playerAnswerTimerLength);
		setPlayerText();
	}

	sweet::Event * e = new sweet::Event("changeturn");
	e->setIntData("isPlayerTurn", _isOffensive);
	eventManager->triggerEvent(e);
}

void PD_UI_DissBattle::setEnemyText(){
	insultGenerator.makeInsults();

	enemyBubbleText->setText(insultGenerator.enemyInsult);

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

	enemyBubbleText->invalidateLayout();
}

void PD_UI_DissBattle::setPlayerText(){

	if(playerResult){
		playerBubbleLayout->addChild(playerBubbleOptions);
		playerBubbleText->setRationalWidth(PLAYER_TEXT_WIDTH, playerBubbleText->nodeUIParent);
		playerBubble->invalidateLayout();
	}

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

	
	// Apply insight effect
	if(insightMultiplier > 0){
		// Highlight correct choice
		if(pBubbleBtn1->isEffective) {
			optionOneShader->setColor(insightAlpha, 1.f, insightAlpha, 1.f);
			optionTwoShader->setColor(1.f, 1.f, 1.f, 1.f);
		}else {
			optionTwoShader->setColor(insightAlpha, 1.f, insightAlpha, 1.f);
			optionOneShader->setColor(1.f, 1.f, 1.f, 1.f);
		}
	}else if(insightMultiplier < 0){
		optionOneShader->setColor(1.f, 1.f, 1.f, insightAlpha);
		optionTwoShader->setColor(1.f, 1.f, 1.f, insightAlpha);
	}else{
		optionOneShader->setColor(1.f, 1.f, 1.f, 1.f);
		optionTwoShader->setColor(1.f, 1.f, 1.f, 1.f);
	}
	

	// Reset timer
	playerQuestionTimer = 0;
	playerAnswerTimer = 0;
	playerResultTimer = 0;
	playerResult = false;

	playerBubbleOptions->setVisible(false);
	playerTimerSlider->setVisible(false);

	playerBubbleText->invalidateLayout();
}

void PD_UI_DissBattle::insult(bool _isEffective, std::wstring _word){
	PD_ResourceManager::scenario->getAudio(TIMER)->sound->stop();
	if(!_isEffective) {
		PD_ResourceManager::scenario->getAudio(FAIL_INSULT)->sound->play();
	}else {
		sweet::NumberUtils::randomItem(succeedInsultSounds)->play();
		int randComp = sweet::NumberUtils::randomInt(1, NUM_COMPLIMENTS);
		complimentBubble->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("DISS-BATTLE-COMPLIMENT" + std::to_string(randComp))->texture);
		complimentBubbleTimer = 0.f;
		complimentBubble->setVisible(true);
	}

	// Make resulting insult
	const std::string constText = insultGenerator.playerInsult;
	std::regex rg(insultGenerator.playerBlank);
	std::string text = std::regex_replace(constText, rg, std::string(_word.begin(), _word.end()));

	playerBubbleText->setText(text);
	playerBubbleLayout->removeChild(playerBubbleOptions);
	playerBubbleText->setRationalWidth(1.f, playerBubbleText->nodeUIParent);
	playerBubble->invalidateLayout();

	playerResult = true;

	countButtonPresses(_isEffective, true);
	countInsultAccuracy(playerAnswerTimer);

	playerResultEffective = _isEffective;

	sweet::Event * e = new sweet::Event("insult");
	e->setIntData("success", _isEffective);
	eventManager->triggerEvent(e);
}

void PD_UI_DissBattle::incrementConfidence(float _value){
	// Value > 0 means the player is attacking
	if(_value > 0) {
		// Factor in enemy's defense
		_value *= playerAttackMultiplier * playerComboMultiplier;
		playerComboMultiplier += playerComboIncrement;
	}else {
		_value *= enemyAttackMultiplier * enemyComboMultiplier;
		enemyComboMultiplier += enemyComboIncrement;
	}

	sweet::Event * e = new sweet::Event("confidence");
	e->setFloatData("value", _value);
	eventManager->triggerEvent(e);

	confidence = confidence + _value > 100.f ? 100.f : confidence + _value < 0.f ? 0.f : confidence + _value;

	if(confidence <= 0){
		// Check if out of lives
		if(lives.size() <= 0){
			gameOver(false);
		}else{
			lostLives.push_back(lives.back());
			lives.pop_back();
			lostLives.back()->background->mesh->replaceTextures(lifeTokensCrossed.at(lives.size()));
			confidence = 50.f;
		}
	}

	if(confidence >= 100){
		gameOver(true);
	}
}

UIGlyph * PD_UI_DissBattle::findFirstPunctuation(int _startIdx){
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

void PD_UI_DissBattle::highlightNextWord(int _startIdx){
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
				eventManager->triggerEvent(e);

				return;
			}
		}
	}
	wordHighlight->setVisible(false);
}

bool PD_UI_DissBattle::isPunctuation(UIGlyph * _glyph){
	switch (_glyph->character){
	case L'.':
	case L'?':
	case L'!':
	case L',':
		return true;
	}

	return false;
}

void PD_UI_DissBattle::countButtonPresses(bool _isCorrect, bool _isOffensive){
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

void PD_UI_DissBattle::countInterjectAccuracy(float _pressTime){
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

void PD_UI_DissBattle::countInsultAccuracy(float _hitTime){
	insultTimes.push_back(_hitTime);
}