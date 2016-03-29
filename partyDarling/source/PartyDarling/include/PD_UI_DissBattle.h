#pragma once

#include <sweet/UI.h>
#include <PD_PhraseGenerator_Insults.h>
#include <OpenALSound.h>
#include <NumberUtils.h>
#include <shader/ComponentShaderText.h>
#include "Player.h"
#include "PD_Character.h"

class Shader;
class PD_InsultButton;
class Keyboard;
class Sprite;
class ComponentShaderText;

struct DissBattleValues{

	// Stats multipliers
	float playerAttackMultiplier[4]; // enemy defense, player strength
	float enemyAttackMultiplier[4]; // player defense, enemy strength
	
	float insightMultiplier[4]; // player insight - enemy insight
	float insightAlpha[4];
	
	float sassInsultMultiplier[4]; // player sass - enemy sass
	float sassInterjectMultiplier[4]; // player sass - enemy sass

	// combo increments
	float playerComboIncrement;
	float enemyComboIncrement;

	DissBattleValues();
};

class InterjectAccuracy {
public:
	wchar_t character;
	float padding;
	float targetTime;
	float hitTime;
	unsigned long int iteration;

	InterjectAccuracy(wchar_t character, float _padding, float _targetTime, float _hitTime, unsigned long int _iteration);
};

/**
* Events:
*	interject:	data "success"		int
*	miss									// missed punctuation opportunity
*   insult:		data "success"		int
*	changeturn	data "isPlayerTurn"	int
*   confidence	data "value"		float	// the value the confidence is being incremented by
*	gameover	data "win"			int		// occurs immediately when the player wins or loses
*	complete:	data "win"			int		// occurs after the game over animation
*/
class PD_UI_DissBattle : public VerticalLinearLayout{
private:
	// number of punctuation marks passed
	unsigned long int iteration;

	PD_PhraseGenerator_Insults insultGenerator;

	bool enabled;
	bool canInterject;

	NodeUI * displayContainer;

	HorizontalLinearLayout * tutorialSpacebar;
	NodeUI * tutorialSpacebarImage;

	VerticalLinearLayout * healthContainer;
	NodeUI * gameContainer;

	HorizontalLinearLayout * livesContainer;
	// lives in the current fight
	std::vector<NodeUI *> lives;
	// lost lives in the current fight
	std::vector<NodeUI *> lostLives;
	// duplicates of lifeTokens, modified to have a red cross through them to show lost lives
	std::vector<Texture *> lifeTokensCrossed;

	SliderControlled * confidenceSlider;

	float basePlayerInsultSpeedMultiplier;
	float playerInsultSpeedMultiplier;

	float basePlayerQuestionTimerLength;
	float playerQuestionTimerLength;
	float playerQuestionTimer;

	float basePlayerAnswerTimerLength;
	float playerAnswerTimerLength;
	float playerAnswerTimer;

	bool playerResult;
	bool playerResultEffective;
	float playerResultTimerLength;
	float playerResultTimer;

	Sprite * enemyCursor;

	UIGlyph * prevHighlightedPunctuation;
	UIGlyph * highlightedPunctuation;
	Sprite * punctuationHighlight;

	UIGlyph * highlightedWordStart;
	UIGlyph * highlightedWordEnd;
	Sprite * wordHighlight;

	NodeUI * enemyBubble;
	TextArea * enemyBubbleText;

	NodeUI * playerBubble;
	HorizontalLinearLayout * playerBubbleLayout;
	TextArea * playerBubbleText;

	HorizontalLinearLayout * playerBubbleOptions;
	PD_InsultButton * pBubbleBtn1;
	PD_InsultButton * pBubbleBtn2;

	SliderControlled * playerTimerSlider;

	Sprite * complimentBubble;
	float complimentBubbleTimerBaseLength;
	float complimentBubbleTimerLength;
	float complimentBubbleTimer;
	float complimentBubbleScale;

	Sprite * interjectBubble;
	bool  interjected; // interjected successfully
	float interjectBubbleTimerBaseLength;
	float interjectBubbleTimerLength;
	float interjectBubbleTimer;
	float interjectBubbleScale;

	Shader * shader;
	ComponentShaderText * optionOneShader;
	ComponentShaderText * optionTwoShader;

	float baseCursorDelayLength;
	float baseCusrorPunctDelayLength;
	float cursorDelayLength;
	float cursorDelayDuration;
	float baseGlyphWidth;

	std::vector<UIGlyph *> glyphs;
	unsigned int glyphIdx;

	float confidence;

	bool isGameOver;
	LinearLayout * gameOverContainer;
	NodeUI * gameOverImage;

	float gameOverLength;
	float gameOverDuration;

	bool win;

	bool isComplete;

	// USER TESTING VARIABLES
	// Button Presses Success Rate
	int offensiveCorrect;
	int offensiveWrong;
	int defensiveCorrect;
	int defensiveWrong;

	// Button Presses Speed
	std::vector<float> insultTimes;
	int punctuationCnt;
	std::vector<InterjectAccuracy> interjectTimes;
	float interjectTimer;

	//Sounds
	std::vector<OpenAL_Sound *> missInterjectSounds;
	Player * player;
	std::vector<OpenAL_Sound *> succeedInsultSounds;

	// Stats multipliers
	float playerAttackMultiplier; // enemy defense, player strength
	float enemyAttackMultiplier; // player defense, enemy strength
	
	float insightMultiplier; // player insight - enemy insight
	float insightAlpha;
	
	float sassInsultMultiplier; // player sass - enemy sass
	float sassInterjectMultiplier; // player sass - enemy sass

	float playerComboIncrement;
	float enemyComboIncrement;

	void setupDissValues();
public:
	float prevXP;
	float wonXP;

	// the life tokens accumulated so far
	// this size of the list also indicates how many lives the player gets at the start of a fight
	std::vector<Texture *> lifeTokens;

	TextArea * selectedGlyphText;

	float damage;
	
	float playerComboMultipier;
	float enemyComboMultipier;

	Keyboard * keyboard;

	PD_Character * enemy;

	bool modeOffensive;

	PD_UI_DissBattle(BulletWorld * _bulletWorld, Player * _player, Font * _font, Shader * _textShader, Shader * _shader);
	~PD_UI_DissBattle();
	void setEnemyText();
	void setPlayerText();

	virtual void update(Step * _step) override;

	void setUIMode(bool _isOffensive);
	void interject();
	void insult(bool _isEffective, std::wstring _word);

	void incrementConfidence(float _value);

	void startNewFight(PD_Character * _enemy, bool _playerFirst = true);
	void gameOver(bool _win);
	void complete();

	void addLife(Texture * _tokenTexture);

	void disable();
	void enable();
	bool isEnabled();

	UIGlyph * findFirstPunctuation(int startIdx = 0);
	void highlightNextWord(int startIdx = 0);
	bool isPunctuation(UIGlyph * _glyph);

	// USER TESTING FUNCTIONS
	void countButtonPresses(bool _isCorrect, bool _isOffensive);
	void countInterjectAccuracy(float _pressTime);
	void countInsultAccuracy(float _insultHitTime);
};