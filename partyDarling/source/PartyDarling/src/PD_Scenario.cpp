#pragma once

#include <PD_Scenario.h>

PD_Scenario::PD_Scenario(std::string _jsonSrc) : Scenario(_jsonSrc),
	NodeResource(false)
{
	Json::Value root;
	Json::Reader reader;
	reader.parse(_jsonSrc, root);
	std::string jsonLoaded = sweet::FileUtils::readFile(_jsonSrc);

	name		  = root.get("name", "NO_NAME").asString();
	description   = root.get("description", "NO_DESCRIPTION").asString();
	scenarioType  = static_cast<ScenarioType>(root.get("type", 0).asInt());
	scenarioOrder = static_cast<ScenarioOrder>(root.get("order", 0).asInt());
}