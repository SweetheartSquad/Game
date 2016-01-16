#pragma once
#include <vector>

class FurnitureComponent{

public:

	std::string name;
	bool requried;
	int multiplier;
	std::vector<FurnitureComponent*> outComponents;

	FurnitureComponent();
	~FurnitureComponent();

};