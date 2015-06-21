#pragma once

#include <DialogueSay.h>
#include <iostream>

DialogueSay::DialogueSay() :
	currentText(-1)
{
}
DialogueSay::DialogueSay(std::string _jsonString) :
	currentText(-1)
{
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( _jsonString, root );
	if(!parsingSuccessful){
		std::cout << "JSON parse failed: " << reader.getFormattedErrorMessages() << std::endl << _jsonString;
	}
	init(root);
}

DialogueSay::DialogueSay(Json::Value _json) :
	currentText(-1)
{
	init(_json);
}

void DialogueSay::init(Json::Value _json){
	speaker = _json.get("speaker", "NO_SPEAKER_DEFINED").asString();
	portrait = _json.get("portrait", "NO_PORTRAIT_DEFINED").asString();
	const Json::Value textArray = _json["text"];
	for(int i = 0; i < textArray.size(); ++i){
		text.push_back(textArray[i].asString());
	}
}

std::string DialogueSay::getCurrentText(){
	return text.at(currentText);
}





DialogueAsk::DialogueAsk(Json::Value _json){
	init(_json);
}
DialogueAsk::DialogueAsk(std::string _jsonString){
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( _jsonString, root );
	if(!parsingSuccessful){
		std::cout << "JSON parse failed: " << reader.getFormattedErrorMessages();
	}
	init(root);
}

void DialogueAsk::init(Json::Value _json){
	DialogueSay::init(_json);
	
	const Json::Value optionsArray = _json["options"];
	for(int i = 0; i < optionsArray.size(); ++i){
		options.push_back(optionsArray[i]["text"].asString());
		optionsResults.push_back(std::vector<Trigger *>());

		const Json::Value triggersArray = optionsArray[i]["triggers"];
		for(int j = 0; j < triggersArray.size(); ++j){
			optionsResults.back().push_back(Trigger::getTrigger(triggersArray[j]));
		}
	}
}

DialogueAsk::~DialogueAsk(){
	while(optionsResults.size() > 0){
		while(optionsResults.back().size() > 0){
			delete optionsResults.back().back();
			optionsResults.back().pop_back();
		}
		optionsResults.pop_back();
	}
}

Trigger * Trigger::getTrigger(Json::Value _json){
	Trigger * res = nullptr;
	std::string type = _json.get("type", "NO_TYPE").asString();
	// create a different type of Trigger depending on the value of type
	if(type == "stateChange"){
		res = new TriggerStateChange(_json);
	}
	return res;
}

TriggerStateChange::TriggerStateChange(Json::Value _json) :
	target(_json.get("target", "NO_TARGET").asString()),
	newState(_json.get("newState", "NO_STATE").asString())
{
}

void TriggerStateChange::trigger(){
	// retrieve pointer to trigger's target
	std::cout << "target: " << target << std::endl;
	// update target's state with newState
	std::cout << "newState: " << newState << std::endl;
}