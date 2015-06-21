#pragma once

#include <string>
#include <json\json.h>

class PD_JsonObject {
public:
	Json::Value root;

	PD_JsonObject(std::string _src);
};