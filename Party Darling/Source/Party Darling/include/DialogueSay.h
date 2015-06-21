#pragma once

#include <node\Node.h>
#include <json\json.h>

#include <functional>

class Trigger abstract : public Node{
public:
	static Trigger * getTrigger(Json::Value _json);
	
	virtual void trigger() = 0;
};

class TriggerStateChange : public Trigger{
public:
	std::string target;
	std::string newState;

	TriggerStateChange(Json::Value _json);
	virtual void trigger() override;
};

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
	std::vector<std::vector<Trigger *>> optionsResults;

	DialogueAsk(std::string _jsonString);
	DialogueAsk(Json::Value _json);
	~DialogueAsk();
	virtual void init(Json::Value _json) override;
};