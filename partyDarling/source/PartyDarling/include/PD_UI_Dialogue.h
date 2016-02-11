#pragma once

#include <sweet/UI.h>
#include <scenario/Conversation.h>
#include <PD_UI_Bubble.h>
#include "PD_Character.h"

class PD_UI_Dialogue : public virtual NodeUI, public virtual ConversationIterator{
public:

	Person * currentSpeaker;

	PD_UI_Bubble * uiBubble;
	NodeUI_NineSliced * textBubble;
	TextArea * text;
	bool temporaryConvo;

	PD_UI_Dialogue(BulletWorld * _world, PD_UI_Bubble * _uiBubble);

	virtual bool sayNext() override;

	void startEvent(Conversation * _conversation, bool _temporaryConvo);

	virtual void end() override;
};
