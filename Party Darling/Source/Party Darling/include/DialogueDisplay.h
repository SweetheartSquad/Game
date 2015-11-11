#pragma once

#include <Entity.h>
#include <LinearLayout.h>
#include <TextArea.h>
#include <Timeout.h>
#include <Easing.h>
//#include <scenario/ConversationIterator.h>
#include <DialogueText.h>
#include <scenario/Conversation.h>

class DialogueDisplay : public ConversationIterator, public NodeUI{
public:
	DialogueDisplay(BulletWorld * _world, Font * _font, Shader * _textShader, float _width, float _height);
	~DialogueDisplay();
	
	LinearLayout * vlayout;
	//LinearLayout * hlayout;
	LinearLayout * optionslayout;
	TextArea * conversationId;
	TextArea * speaker;
	TextArea * dialogue;
	NodeUI * progressButton;
	bool waitingForInput;
	std::vector<NodeUI *> options;
	Font * font;
	Shader * textShader;


	virtual void update(Step * _step) override;

	// moves the conversation forward by one step and updates all of the display properties to match the new state
	// if there is nothing left to say, returns false. Returns true otherwise
	virtual bool sayNext() override;
};