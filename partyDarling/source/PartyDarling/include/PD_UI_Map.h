#pragma once

#include <sweet/UI.h>
#include <Room.h>

#define MAP_SIZE 2

class MapCell : public NodeUI{
private:
	bool visited;
public:
	Room * room;
	MapCell(BulletWorld * _world, Room * _room);

	bool isVisited();
	void setVisited(bool _visited);
};

class PD_UI_Map : public NodeUI{
private:
	VerticalLinearLayout * layout;
	VerticalLinearLayout * innerLayout;
	TextLabel * roomName;
	std::map<std::pair<int, int>, MapCell *> grid;

	bool enabled;
	bool detailed;
public:
	PD_UI_Map(BulletWorld * _world, Font * _font, ComponentShaderText * _textShader);


	// deletes the old map and constructs a new one based on the provided grid
	void buildMap(std::map<std::pair<int, int>, Room *> _houseGrid);
	// updates the grid with the current position
	void updateMap(glm::ivec2 _currentPosition);

	void disable();
	void enable();
	bool isEnabled();

	bool isDetailed();
	void setDetailed(bool _detailed);
};