#pragma once

#include <PD_UI_YellingContest.h>
#include <sweet/UI.h>

class Shader;
class PD_InsultButton;
class Keyboard;
class Sprite;
class ComponentShaderText;

class PD_UI_YellingContest : public VerticalLinearLayout{
private:
	bool isEnabled;

	HorizontalLinearLayout * livesContainer;
	std::vector<NodeUI *> lives;
	std::vector<NodeUI *> lostLives;

	float playerDelay;
	float playerTimer;

	std::map<UIGlyph *, Sprite *> highlights;

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

public:
	Sprite * enemyCursor;

	SliderControlled * confidenceSlider;

	TextArea * selectedGlyphText;
	
	float damage;

	NodeUI * enemyBubble;
	TextArea * enemyBubbleText;

	HorizontalLinearLayout * playerBubble;
	TextArea * playerBubbleText;
	
	PD_InsultButton * pBubbleBtn1;
	PD_InsultButton * pBubbleBtn2;

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
};