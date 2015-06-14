#pragma once

#include <node\Node.h>
#include <json\json.h>

class DialogueSay : public Node{
public:
	std::string speaker;
	std::string portrait;
	std::vector<std::string> text;
	unsigned long int currentText;
	
	// this constructor doesn't call init
	DialogueSay();
	// parses the string into a json object and uses it to call init
	DialogueSay(std::string _jsonString);
	DialogueSay(Json::Value _json);
	virtual void init(Json::Value _json);

	std::string getCurrentText();
};


// a dialog object which prompts for user selection
class DialogueAsk : public DialogueSay{
public:
	std::vector<std::string> options;

	DialogueAsk(std::string _jsonString);
	DialogueAsk(Json::Value _json);
	virtual void init(Json::Value _json) override;
};