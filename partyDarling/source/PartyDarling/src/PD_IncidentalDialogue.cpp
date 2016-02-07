#pragma once

#include <PD_IncidentalDialogue.h>
#include <scenario/Scenario.h>
#include <uuids.h>
#include <NumberUtils.h>

std::string PD_IncidentalDialogue::createDialogue(std::string _charId, Scenario * _scenario) {

	std::string id = std::to_string(sweet::NumberUtils::randomFloat(100000, 999999));
	id += std::to_string(sweet::step.time);
	
	Json::Value convo;
	convo["id"] = id;
	convo["name"] = id;

	std::string dialogue = "TESST";

	Json::Value dialougeDef;
	dialougeDef["text"].append(dialogue);
	dialougeDef["speaker"] = _charId;
	convo["dialogue"].append(dialougeDef);
	
	_scenario->assets["conversation"][id] = AssetConversation::create(convo, _scenario);
	
	return id;
}
