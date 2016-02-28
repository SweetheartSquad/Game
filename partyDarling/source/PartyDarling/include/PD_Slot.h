#pragma once

#include <vector>

class RoomObject;
enum class PD_Side;

class PD_Slot{
public:
	float spaceFilled;
	float length;
	unsigned long int maxItems;
	PD_Side childSide; // The side the child can be aligned with this side

	std::vector<RoomObject *> children;

	PD_Slot(PD_Side _childSide, float _length, unsigned long int _maxItems = 100);
};