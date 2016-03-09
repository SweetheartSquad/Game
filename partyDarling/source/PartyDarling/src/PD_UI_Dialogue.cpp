#pragma once

#include <PD_UI_Dialogue.h>
#include <PD_ResourceManager.h>
#include <StringUtils.h>

PD_UI_Dialogue::PD_UI_Dialogue(BulletWorld * _world, PD_UI_Bubble * _uiBubble) :
	NodeUI(_world),
	uiBubble(_uiBubble),
	textBubble(new NodeUI_NineSliced(world, dynamic_cast<Texture_NineSliced *>(PD_ResourceManager::scenario->getTexture("NPC-BUBBLE")->texture))),
	text(new TextArea(world, PD_ResourceManager::scenario->getFont("FONT")->font, uiBubble->textShader)),
	currentSpeaker(nullptr),
	speechTimeout(nullptr),
	hadNextDialogue(false)
{
	text->setWrapMode(kWORD);
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

	speechTimeout = new Timeout(0.2, [this](sweet::Event * _event){
		if(speechBuffer.size() > 0) {
			std::wstring word = speechBuffer.front();
			char fc = tolower(word[0]);
			speechBuffer.pop();
			if(currentSpeaker != nullptr){
				auto sound = currentSpeaker->voice;
				sound->setPitch(fc/178.f+0.75f);
				sound->play();
			}
			speechTimeout->restart();
		}else {
			if(currentSpeaker != nullptr) {
				currentSpeaker->pr->talking = false;
			}
		}
	});
}

PD_UI_Dialogue::~PD_UI_Dialogue(){
	delete speechTimeout;
}

void PD_UI_Dialogue::update(Step * _step){
	speechTimeout->update(_step);
	NodeUI::update(_step);
}

bool PD_UI_Dialogue::sayNext(){
	
invalidateLayout();
	
	
	if(currentSpeaker != nullptr) {
		currentSpeaker->pr->talking = false;
	}

	if (ConversationIterator::sayNext()){
		uiBubble->clear();
		if(speechTimeout->active) {
			speechTimeout->stop();
		}
		currentSpeaker = PD_Listing::listings[currentConversation->scenario]->characters[currentConversation->getCurrentDialogue()->speaker];
		if(Dialogue * dialogue = currentConversation->getCurrentDialogue()){
			text->setText(dialogue->getCurrentText());
		}else{
			text->setText("");
		}
		clearSpeechBuffer();
		for(std::wstring s : sweet::StringUtils::split(text->getText(), L' ')) {
			speechBuffer.push(s);
		}
		if (waitingForInput){
			currentSpeaker->pr->talking = false;
			clearSpeechBuffer();
			for(unsigned long int i = 0; i < currentConversation->options.size(); ++i){
				uiBubble->addOption(currentConversation->options.at(i)->text, [this, i](sweet::Event * _event){
					select(i);
				});
			}
		}else{
			currentSpeaker->pr->talking = true;
			uiBubble->addOption("...", [this](sweet::Event * _event){
				sayNext();
			});
		}
		if(!speechTimeout->active) {
			speechTimeout->start();
		}
		hadNextDialogue = true;
		return hadNextDialogue;
	}

	uiBubble->clear();

	currentSpeaker = nullptr;
	setVisible(false);
	speechTimeout->stop();
	hadNextDialogue = false;
	return hadNextDialogue;
}

void PD_UI_Dialogue::startEvent(Conversation * _conversation, bool _temporaryConvo){
	currentConversation = _conversation;
	temporaryConvo = _temporaryConvo;
	setVisible(true);

	currentConversation->reset();
	sayNext();
}

void PD_UI_Dialogue::end(){
	if(temporaryConvo){
		delete currentConversation;
	}
	ConversationIterator::end();
	eventManager->triggerEvent("end");
}

void PD_UI_Dialogue::clearSpeechBuffer(){
   std::queue<std::wstring> empty;
   std::swap(speechBuffer, empty);
}
