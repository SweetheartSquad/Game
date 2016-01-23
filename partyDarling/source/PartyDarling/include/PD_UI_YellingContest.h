#pragma once

#include <sweet/UI.h>
#include <PD_InsultGenerator.h>

class Shader;
class PD_InsultButton;
class Keyboard;
class Sprite;
class ComponentShaderText;

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
*   insult:		data "success"		int
*	changeturn	data "isPlayerTurn"	int
*   confidence	data "value"		float	// the value the confidence is being incremented by
*	gameover	data "win"			int		// occurs immediately when the player wins or loses
*	complete:	data "win"			int		// occurs after the game over animation
*/
class PD_UI_YellingContest : public VerticalLinearLayout{
private:
	// number of punctuation marks passed
	unsigned long int iteration;

	PD_InsultGenerator insultGenerator;

	bool isEnabled;

	VerticalLinearLayout * healthContainer;
	VerticalLinearLayout * gameContainer;

	HorizontalLinearLayout * livesContainer;
	std::vector<NodeUI *> lives;
	std::vector<NodeUI *> lostLives;

	SliderControlled * confidenceSlider;

	float playerQuestionTimerLength;
	float playerQuestionTimer;

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

	Shader * shader;

	float baseCursorDelayLength;
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

public:
	
	TextArea * selectedGlyphText;
	
	float damage;

	Keyboard * keyboard;

	bool modeOffensive;

	PD_UI_YellingContest(BulletWorld * _bulletWorld, Font * _font, Shader * _textShader, Shader * _shader);
	void setEnemyText();
	void setPlayerText();

	virtual void update(Step * _step) override;

	void setUIMode(bool _isOffensive);
	void interject();
	void insult(bool _isEffective, std::wstring _word);

	void incrementConfidence(float _value);

	void startNewFight();
	void gameOver(bool _win);
	void complete();
	void disable();
	void enable();

	UIGlyph * findFirstPunctuation(int startIdx = 0);
	void highlightNextWord(int startIdx = 0);
	bool isPunctuation(UIGlyph * _glyph);

	// USER TESTING FUNCTIONS
	void countButtonPresses(bool _isCorrect, bool _isOffensive);
	void countInterjectAccuracy(float _pressTime);
	void countInsultAccuracy(float _insultHitTime);
};