#pragma once

#include <TextArea.h>
#include <TextLabel.h>
#include <Timeout.h>

class DialogueTextLabel : public TextLabel{
public:
	bool ticking;

	DialogueTextLabel(BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader);
	~DialogueTextLabel();

	std::vector<Timeout *> timers;
	void tickerIn(float _delay);
	void finishTicking();

	virtual void update(Step * _step) override;
};

class DialogueTextArea : public TextArea{
public:
	bool ticking;

	std::vector<Timeout *> timers;
	void tickerIn(float _delay);
	void finishTicking();
	virtual TextLabel * getNewLine();
	virtual void update(Step * _step) override;

	DialogueTextArea(BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader);
	~DialogueTextArea();
};