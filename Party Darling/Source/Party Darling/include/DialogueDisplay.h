#pragma once

#include <Entity.h>
#include <LinearLayout.h>
#include <LabelV2.h>
#include <UIPanel.h>
#include <Timeout.h>
#include <DialogueSay.h>
#include <PD_Button.h>

class DialogueDisplay : public Entity{
public:
	std::vector<DialogueSay *> stuffToSay;
	unsigned long int currentDialogue;

	DialogueDisplay(BulletWorld * _world, Scene * _scene, Font * _font, float _width, float _height);
	~DialogueDisplay();
	
	LinearLayout * hlayout;
	LinearLayout * vlayout;
	LinearLayout * optionslayout;
	UIPanel * portraitPanel;
	LabelV2 * speaker;
	LabelV2 * dialogue;
	Timeout * timeout;
	
	bool waitingForInput;
	std::vector<PD_Button *> options;


	void update(Step * _step) override;

	// increments currentDialogue and sets text to stuffToSay.at(currentDialogue)
	// if there is nothing left to say, returns false. Returns true otherwise
	bool sayNext();
};