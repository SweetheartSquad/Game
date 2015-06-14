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
		// do something with optionsArray[i]["triggers"]
	}
}