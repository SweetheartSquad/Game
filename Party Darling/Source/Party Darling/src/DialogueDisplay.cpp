#pragma once

#include <DialogueDisplay.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentTexture.h>
#include <MeshInterface.h>
#include <Font.h>
#include <VerticalLinearLayout.h>
#include <HorizontalLinearLayout.h>
#include <PD_ResourceManager.h>

DialogueDisplay::DialogueDisplay(BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader, float _width, float _height) :
	waitingForInput(false),
	font(_font),
	textShader(_textShader),
	NodeUI(_world, _scene)
{
	setWidth(_width);
	setHeight(_height);
	setPadding(10);
	setMargin(10);
	
	vlayout = new VerticalLinearLayout(_world, _scene);
	vlayout->setRationalWidth(1.f, this);
	vlayout->setRationalHeight(1.f, this);
	vlayout->verticalAlignment = kTOP;
	vlayout->setPadding(10);
	
	dialogue = new TextArea(_world, _scene, _font, _textShader, -1);
	dialogue->setRationalWidth(1.f, vlayout);
	dialogue->setMarginLeft(0.1f);

	speaker = new TextArea(_world, _scene, _font, _textShader, -1);
	speaker->setRationalWidth(1.f, vlayout);
	speaker->setMarginLeft(0.1f);

	conversationId = new TextArea(_world, _scene, _font, _textShader, -1);
	conversationId->setRationalWidth(1.f, vlayout);
	conversationId->setMarginLeft(0.1f);

	progressButton = new NodeUI(world, scene, 1.f);
	//progressButton->setText(L"Next");
	progressButton->setRationalWidth(1.f);
	progressButton->setRationalHeight(1.f);
	progressButton->setMarginTop(0.9f);
	progressButton->setMarginRight(0.9f);
	progressButton->onClickFunction = [this](NodeUI * _this) {
		this->autoProgressTimer->trigger();
	};
	progressButton->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DEFAULT")->texture);
	//progressButton->setVisible(false);

	optionslayout = new HorizontalLinearLayout(_world, _scene);
	optionslayout->setRationalWidth(1.f);
	optionslayout->setRationalHeight(1.f);
	optionslayout->verticalAlignment = kMIDDLE;
	optionslayout->horizontalAlignment = kCENTER;
	
	addChild(vlayout);
	addChild(progressButton);
	vlayout->addChild(conversationId);
	vlayout->addChild(speaker);
	vlayout->addChild(dialogue);
	vlayout->addChild(optionslayout);

	autoProgressTimer->onCompleteFunction = [this](Timeout * _this) {
		//if(dialogue->ticking){
		//	dialogue->finishTicking();
		//	autoProgressTimer->restart();
		//}else{
			this->shouldSayNext = true;
		//}
	};
	autoProgressTimer->start();
}

DialogueDisplay::~DialogueDisplay(){
}

bool DialogueDisplay::sayNext(){
	// move conversation forward
	if(!ConversationIterator::sayNext()){
		return false;
	}
	
	// set the text
	std::string conversationIdText = currentConversation->id;
	std::string speakerText = currentConversation->getCurrentDialogue()->speaker;
	std::string dialogueText = currentConversation->getCurrentDialogue()->getCurrentText();

	conversationId->setText(std::wstring(conversationIdText.begin(), conversationIdText.end()));
	speaker->setText(std::wstring(speakerText.begin(), speakerText.end()));
	dialogue->setText(std::wstring(dialogueText.begin(), dialogueText.end()));

	// check for dialogue options
	DialogueAsk * ask = dynamic_cast<DialogueAsk *>(currentConversation->getCurrentDialogue());
	if(ask != nullptr){
		if(ask->currentText == ask->text.size()-1){
			waitingForInput = true;
			removeChild(progressButton);
			for(unsigned long int i = 0; i < ask->options.size(); ++i){
				//dialogue->appendText(std::wstring(s.begin(), s.end()));
				TextArea * o = new TextArea(world, scene, font, textShader, 0.3f);
				if(i != 0){
					o->setMarginLeft(10);
				}
				std::wstringstream ss;
				ss << (i+1) << L". " << std::wstring(ask->options.at(i)->text.begin(), ask->options.at(i)->text.end());
				o->setText(ss.str());
				options.push_back(o);
				optionslayout->addChild(o);
				//o->parents.at(0)->scale(50,50,1);
				
				std::vector<Trigger *> optionResult = ask->options.at(i)->triggers;
				o->onClickFunction = [this, optionResult](NodeUI * _this) {
					// remove the function pointers on all the other buttons to avoid multiple buttons getting triggered at the same time
					for(auto o : options){
						if(o != _this){
							o->onClickFunction = nullptr;
						}
					}

					for(auto t : optionResult){
						t->trigger();
					}
					this->waitingForInput = false;
					this->shouldSayNext = true;
					this->addChild(progressButton);
				};
			}

			optionslayout->layoutChildren();
		}
	}

	return true;
}

void DialogueDisplay::update(Step * _step){
	if(shouldSayNext){
		while(options.size() > 0){
			optionslayout->removeChild(options.back());
			delete options.back();
			options.pop_back();
		}
		sayNext();
		shouldSayNext = false;
	}
	
	if(autoProgress){
		autoProgressTimer->update(_step);
	}
	NodeUI::update(_step);
}