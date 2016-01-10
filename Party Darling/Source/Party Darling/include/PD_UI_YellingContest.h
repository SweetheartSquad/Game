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
	float playerDelay;
	float playerTimer;

	std::map<UIGlyph *, Sprite *> highlights;

	UIGlyph * highlightedPunctuation;
	Sprite * punctuationHighlight;
	
	UIGlyph * highlightedWordStart;
	UIGlyph * highlightedWordEnd;
	Sprite * wordHighlight;
	
	Shader * shader;

	float cursorDelayLength;
	float cursorDelayDuration;

	std::vector<UIGlyph *> glyphs;
	unsigned int glyphIdx;
	glm::vec3 glyph1Pos;
	glm::vec3 glyph2Pos;

	float confidence;

public:
	Sprite * enemyCursor;
	Camera * cam;

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

	PD_UI_YellingContest(BulletWorld * _bulletWorld, Font * _font, Shader * _textShader, Shader * _shader, Camera * _cam);
	void setEnemyText();
	void setPlayerText();

	virtual void update(Step * _step) override;

	void setUIMode(bool _isOffensive);
	void interject();
	void insult(bool _isEffective);

	void incrementConfidence(float _value);

	UIGlyph * findFirstPunctuation(int startIdx = 0);
	void highlightNextWord(int startIdx = 0);
	bool isPunctuation(UIGlyph * _glyph);
};