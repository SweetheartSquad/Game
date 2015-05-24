#pragma once

#include <node\Node.h>

#include <json\json.h>

class PD_Story : public Node{
public:
	PD_Story(std::string _filepath);
	~PD_Story();
};