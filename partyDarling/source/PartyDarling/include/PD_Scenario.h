#pragma once

#include <string>
#include <scenario/Scenario.h>

enum ScenarioType {
	kSIDE		= 0,
	kBEGINNING  = 1,
	kPLOT		= 2,
	kEND		= 3
}; 

class PD_Scenario : public Scenario {
public:	
	std::string  name;
	std::string  description;
	ScenarioType scenarioType;

	explicit PD_Scenario(std::string _jsonSrc);
};

