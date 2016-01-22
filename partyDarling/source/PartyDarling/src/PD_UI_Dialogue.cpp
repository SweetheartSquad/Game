#pragma once

#include <PD_UI_Dialogue.h>
#include <PD_ResourceManager.h>

PD_UI_Dialogue::PD_UI_Dialogue(BulletWorld * _world, PD_UI_Bubble * _uiBubble) :
	NodeUI(_world),
	uiBubble(_uiBubble),
	textBubble(new NodeUI_NineSliced(world, uiBubble->bubbleTex)),
	text(new TextArea(world, PD_ResourceManager::scenario->defaultFont->font, uiBubble->textShader, 1.f))
{
	setRenderMode(kTEXTURE);
	VerticalLinearLayout * vl = new VerticalLinearLayout(world);
	vl->setRationalWidth(1.f, this);
	vl->setRationalHeight(1.f, this);
	
	vl->horizontalAlignment = kCENTER;
	vl->verticalAlignment = kTOP;

	
	VerticalLinearLayout * vl2 = new VerticalLinearLayout(world);
	
	vl2->horizontalAlignment = kCENTER;
	vl2->verticalAlignment = kMIDDLE;

	addChild(vl);
	vl->addChild(textBubble);
	textBubble->setMargin(15,15,0,15);
	textBubble->setBorder(15);
	textBubble->setRationalWidth(0.9f, vl);
	textBubble->setRationalHeight(0.25f, vl);
	textBubble->addChild(vl2);
	vl2->setRationalWidth(1.f, textBubble);
	vl2->setRationalHeight(1.f, textBubble);
	vl2->addChild(text);
	vl2->setPadding(15);
	text->setRationalWidth(1.f, vl2);
	text->setText("test");
	background->setVisible(false);
	
	// disable and hide by default
	setVisible(false);
}

bool PD_UI_Dialogue::sayNext(){
	invalidateLayout();
	uiBubble->clear();
	if (ConversationIterator::sayNext()){
		if(Dialogue * dialogue = currentConversation->getCurrentDialogue()){
			text->setText(dialogue->getCurrentText());
		}else{
			text->setText("");
		}

		if (waitingForInput){
			for(unsigned long int i = 0; i < currentConversation->options.size(); ++i){
				uiBubble->addOption(currentConversation->options.at(i)->text, [this, i](sweet::Event * _event){
					select(i);
				});
			}
		}else{
			uiBubble->addOption("...", [this](sweet::Event * _event){
				sayNext();
			});
		}
		return true;
	}else{
		setVisible(false);
		return false;
	}
}

void PD_UI_Dialogue::startEvent(Conversation * _conversation){
	currentConversation = _conversation;
	setVisible(true);

	currentConversation->reset();
	sayNext();
}

void PD_UI_Dialogue::end(){
	ConversationIterator::end();
	eventManager.triggerEvent("end");
}