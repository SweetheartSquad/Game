#pragma once

#include <string>
#include <scenario/Scenario.h>

enum ScenarioType {
	kSIDE		= 0,
	kOMAR		= 1,
	kPLOT		= 2,
	kLAB		= 3
}; 

enum ScenarioOrder {
	kRANDOM        = 0,
	kBEGINNING     = 1,
	kMIDDLE_ONE    = 2,
	kMIDDLE_TWO    = 3,
	kMIDDLE_THREE  = 4,
	kEND           = 5
};

class PD_Scenario : public Scenario {
public:	
	std::string   name;
	std::string   description;
	ScenarioType  scenarioType;
	ScenarioOrder scenarioOrder;

	explicit PD_Scenario(std::string _jsonSrc);
};

