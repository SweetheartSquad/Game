#pragma once

#include <PD_UI_YellingContest.h>
#include <sweet/UI.h>

class Shader;
class PD_InsultButton;
class Keyboard;
class Sprite;
class ComponentShaderText;
/*
class PD_UI_YellingContest_TextLabel : public TextLabel{
	Sprite * highlight;
	UIGlyph * highlightedGlyph;

	void highlightGlyph(UIGlyph * _glyph);

	PD_UI_YellingContest_TextLabel(BulletWorld* _world, Font * _font, Shader * _textShader, Shader * _shader, float _width = -1);

	virtual void update(Step * _step) override;
};
*/
class PD_UI_YellingContest_TextArea : public TextArea{
private:
	
public:
	
	virtual void setText(std::wstring _text);
	

	PD_UI_YellingContest_TextArea(BulletWorld * _world, Font * _font, Shader * _textShader, Shader * _shader);
	~PD_UI_YellingContest_TextArea();

	virtual void update(Step * _step) override;
};

class PD_UI_YellingContest : public VerticalLinearLayout{
private:
	float playerDelay;
	float playerTimer;

	std::map<UIGlyph *, Sprite *> highlights;
	UIGlyph * highlightedGlyph;
	Shader * highlightShader;

	float cursorDelayLength;
	float cursorDelayDuration;

	bool moveCursor;
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
	PD_UI_YellingContest_TextArea * enemyBubbleText;

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

	void highlightPunctuation(UIGlyph * _glyph);
	void highlightGlyph(UIGlyph * _glyph, bool _highlight);
};