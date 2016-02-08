#pragma once

#include <sweet/UI.h>
#include <Room.h>

#define MAP_SIZE 3

class PD_UI_Map : public NodeUI{
private:
	VerticalLinearLayout * layout;
	std::map<std::pair<int, int>, NodeUI *> grid;

	bool enabled;
public:
	PD_UI_Map(BulletWorld * _world);


	// deletes the old map and constructs a new one based on the provided grid
	void buildMap(std::map<std::pair<int, int>, Room *> _houseGrid);
	// updates the grid with the current position
	void updateMap(glm::ivec2 _currentPosition);

	void disable();
	void enable();
	bool isEnabled();
};