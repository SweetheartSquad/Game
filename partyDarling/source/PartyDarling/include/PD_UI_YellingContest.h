#pragma once

#include <sweet/UI.h>
#include <PD_InsultGenerator.h>

class Shader;
class PD_InsultButton;
class Keyboard;
class Sprite;
class ComponentShaderText;

class PD_UI_YellingContest : public VerticalLinearLayout{
private:
	PD_InsultGenerator insultGenerator;

	bool isEnabled;

	HorizontalLinearLayout * livesContainer;
	std::vector<NodeUI *> lives;
	std::vector<NodeUI *> lostLives;

	float playerQuestionTimerLength;
	float playerQuestionTimer;

	float playerAnswerTimerLength;
	float playerAnswerTimer;

	UIGlyph * highlightedPunctuation;
	Sprite * punctuationHighlight;
	
	UIGlyph * highlightedWordStart;
	UIGlyph * highlightedWordEnd;
	Sprite * wordHighlight;
	
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

	// TESTING
	int offensiveCorrect;
	int offensiveWrong;
	int defensiveCorrect;
	int defensiveWrong;

	VerticalLinearLayout * buttonPresses;
	TextLabel * offensiveCorrectLabel;
	TextLabel * offensiveWrongLabel;
	TextLabel * defensiveCorrectLabel;
	TextLabel * defensiveWrongLabel;

public:
	Sprite * enemyCursor;

	SliderControlled * confidenceSlider;

	TextArea * selectedGlyphText;
	
	float damage;

	NodeUI * enemyBubble;
	TextArea * enemyBubbleText;

	NodeUI * playerBubble;
	TextArea * playerBubbleText;
	
	PD_InsultButton * pBubbleBtn1;
	PD_InsultButton * pBubbleBtn2;

	SliderControlled * playerTimerSlider;

	Keyboard * keyboard;

	bool modeOffensive;

	PD_UI_YellingContest(BulletWorld * _bulletWorld, Font * _font, Shader * _textShader, Shader * _shader);
	void setEnemyText();
	void setPlayerText();

	virtual void update(Step * _step) override;

	void setUIMode(bool _isOffensive);
	void interject();
	void insult(bool _isEffective);

	void incrementConfidence(float _value);

	void startNewFight();
	void gameOver(bool _win);
	void complete();
	void disable();
	void enable();

	UIGlyph * findFirstPunctuation(int startIdx = 0);
	void highlightNextWord(int startIdx = 0);
	bool isPunctuation(UIGlyph * _glyph);

	void countButtonPresses(bool _isCorrect, bool _isOffensive);
};