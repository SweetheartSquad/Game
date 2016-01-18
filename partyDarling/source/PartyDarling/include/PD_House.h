#pragma once

#include <RoomBuilder.h>

#include <vector>

class NodeUI;

class PD_House {
public:
	std::vector<Room *> rooms;
	NodeUI * map;

	PD_House();
	~PD_House();


};