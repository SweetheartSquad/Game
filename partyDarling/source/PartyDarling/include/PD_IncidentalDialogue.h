#pragma once

#include <scenario/Conversation.h>
#include <string>

class PD_IncidentalDialogue {
public:	

	static std::string createDialogue(std::string _charId, Scenario * _scenario);
};
