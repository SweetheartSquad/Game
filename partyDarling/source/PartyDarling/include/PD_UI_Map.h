#pragma once

#include <sweet/UI.h>
#include <Room.h>

class PD_UI_Map : public NodeUI{
private:
	VerticalLinearLayout * layout;
	std::map<std::pair<int, int>, NodeUI *> grid;

	bool enabled;
public:
	PD_UI_Map(BulletWorld * _world);


	void updateMap(std::map<std::pair<int, int>, Room *> _houseGrid);

	void disable();
	void enable();
	bool isEnabled();
};