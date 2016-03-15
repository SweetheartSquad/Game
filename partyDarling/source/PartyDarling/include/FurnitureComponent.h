#pragma once

#include <node/Node.h>

#include <vector>

class FurnitureComponent : public Node{
public:

	std::string name;
	bool requried;
	int multiplier;
	std::vector<FurnitureComponent *> outComponents;

	FurnitureComponent();
	~FurnitureComponent();
};