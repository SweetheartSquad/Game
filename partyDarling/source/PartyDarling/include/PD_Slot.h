#pragma once

#include <vector>

class RoomObject;
enum class PD_Side;

class PD_Slot{
public:
	float loc;
	float length;
	PD_Side childSide; // The side the child can be aligned with this side

	std::vector<RoomObject *> children;

	PD_Slot(PD_Side _childSide, float loc, float length);
};