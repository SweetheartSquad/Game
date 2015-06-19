#pragma once

#include <DialogueDisplay.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentTexture.h>
#include <MeshInterface.h>
#include <Font.h>
#include <PD_ResourceManager.h>
#include <VerticalLinearLayout.h>
#include <HorizontalLinearLayout.h>

DialogueDisplay::DialogueDisplay(BulletWorld * _world, Scene * _scene, Font * _font, Shader * _textShader, float _width, float _height) :
	currentDialogue(0),
	waitingForInput(false),
	font(_font),
	textShader(_textShader),
	NodeUI(_world, _scene),
	NodeBulletBody(_world)
{
	setWidth(_width);
	setHeight(_height);


	hlayout = new HorizontalLinearLayout(_world, _scene);
	hlayout->setRationalWidth(1.f);
	vlayout = new VerticalLinearLayout(_world, _scene);
	vlayout->setRationalWidth(0.75f);
	optionslayout = new VerticalLinearLayout(_world, _scene);
	optionslayout->setRationalWidth(1.f);
	
	portraitPanel = new NodeUI(_world, _scene);
	portraitPanel->setRationalWidth(0.25f);
	
	dialogue = new TextArea(_world, _scene, _font, _textShader, -1);
	dialogue->setRationalWidth(1.f);
	speaker = new TextArea(_world, _scene, _font, _textShader, -1);
	speaker->setRationalWidth(1.f);

	
	addChild(hlayout);
	hlayout->addChild(portraitPanel);
	hlayout->addChild(vlayout);
	vlayout->addChild(speaker);
	vlayout->addChild(dialogue);
	vlayout->addChild(optionslayout);

	timeout = new Timeout(2.f);
	timeout->onCompleteFunction = [this](Timeout * _this) {
		this->sayNext();
	};

}

DialogueDisplay::~DialogueDisplay(){

}

bool DialogueDisplay::sayNext(){
	// check if there's anything left to say at all
	if(currentDialogue >= stuffToSay.size()){
		return false;
	}

	// clear out the options list (they'll still be there if the last thing was an ask)
	/*while(options.size() > 0){
		delete options.back()->parents.at(0);
		options.pop_back();
	}*/

	// set the speaker and portrait
	std::string sp = stuffToSay.at(currentDialogue)->speaker;
	speaker->setText(std::wstring(sp.begin(), sp.end()));
	while(portraitPanel->background->mesh->textures.size() > 0){
		portraitPanel->background->mesh->popTexture2D();
	}
	if(stuffToSay.at(currentDialogue)->portrait == "cheryl"){
		portraitPanel->background->mesh->pushTexture2D(PD_ResourceManager::cheryl);
	}else{

	}

	// move to the next text in the current dialogue object
	++stuffToSay.at(currentDialogue)->currentText;
	// check if there's any text left to say in the current dialogue object
	if(stuffToSay.at(currentDialogue)->currentText < stuffToSay.at(currentDialogue)->text.size()){
		// if there is, read the text in the current dialogue and return
		std::string thingToSay = stuffToSay.at(currentDialogue)->getCurrentText();
		dialogue->setText(std::wstring(thingToSay.begin(), thingToSay.end()));

		DialogueAsk * ask = dynamic_cast<DialogueAsk *>(stuffToSay.at(currentDialogue));
		if(ask != nullptr){
			waitingForInput = true;
			for(std::string s : ask->options){
				//dialogue->appendText(std::wstring(s.begin(), s.end()));
				PD_Button * o = new PD_Button(hlayout->world, hlayout->scene, font, textShader, 1.f);
				o->normalLabel = std::wstring(s.begin(), s.end());
				options.push_back(o);
				optionslayout->addChild(o);
				//o->parents.at(0)->scale(50,50,1);
				o->onClickFunction = [this](NodeUI * _this) {
					this->waitingForInput = false;
					for(unsigned long int i = 0; i < this->options.size(); ++i){
						this->optionslayout->removeChild(this->options.at(i));
					}
					this->sayNext();
				};
			}
		}

		if(!waitingForInput){
			timeout->restart();
		}

		return true;
	}else{
		// if there isn't move to the next dialogue object and try again
		++currentDialogue;
		return sayNext();
	}
}

void DialogueDisplay::update(Step * _step){
	Entity::update(_step);
	timeout->update(_step);
	portraitPanel->setHeight(portraitPanel->getWidth());
}