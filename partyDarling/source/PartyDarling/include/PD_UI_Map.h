#pragma once

#include <sweet/UI.h>
#include <Room.h>

#define MAP_SIZE 2

class MapCell : public NodeUI{
public:
	Room * room;
	MapCell(BulletWorld * _world, Room * _room);
};

class PD_UI_Map : public NodeUI{
private:
	ComponentShaderBase * mapCellShader;

	VerticalLinearLayout * layout;
	VerticalLinearLayout * innerLayout;
	TextLabel * roomName;
	NodeUI * compass;
	std::map<std::pair<int, int>, MapCell *> grid;

	bool enabled;

	bool adjacent(std::pair<int, int> _cell, glm::ivec2 _housePosition);
public:
	PD_UI_Map(BulletWorld * _world, Font * _font, ComponentShaderText * _textShader);
	~PD_UI_Map();

	// deletes the old map and constructs a new one based on the provided grid
	void buildMap(std::map<std::pair<int, int>, Room *> _houseGrid);
	// updates the grid with the current position
	void updateMap(glm::ivec2 _currentPosition);
	void updateCompass(float _angle);

	void disable();
	void enable();
	bool isEnabled();
};