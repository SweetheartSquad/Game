#pragma once

#include <PD_UI_Map.h>
#include <PD_TilemapGenerator.h>
#include <PD_Assets.h>

MapCell::MapCell(BulletWorld * _world, Room * _room) :
	NodeUI(_world),
	visited(false),
	room(_room)
{
	background->mesh->setScaleMode(GL_NEAREST);
	setVisible(false);
	boxSizing = kCONTENT_BOX;
}

bool MapCell::isVisited(){
	return visited;
}

void MapCell::setVisited(bool _visited){
	visited = _visited;
	if(visited){
		setVisible(true);
	}else{
		setVisible(false);
	}
}

PD_UI_Map::PD_UI_Map(BulletWorld * _world, Font * _font, ComponentShaderText * _textShader) :
	NodeUI(_world),
	enabled(true),
	detailed(false),
	layout(nullptr)
{
	background->setVisible(false);

	roomName = new TextLabel(world, _font, _textShader);
	addChild(roomName);
	roomName->horizontalAlignment = kCENTER;
	roomName->setRationalWidth(1.f, this);
	roomName->setMarginBottom(0.05f);
	roomName->setVisible(false);

}

void PD_UI_Map::disable(){
	setVisible(false);
	enabled = false;
	invalidateLayout();
}

void PD_UI_Map::enable(){
	setVisible(true);
	enabled = true;
	invalidateLayout();
}

bool PD_UI_Map::isEnabled(){
	return enabled;
}

bool PD_UI_Map::isDetailed(){
	return detailed;
}

void PD_UI_Map::setDetailed(bool _detailed){
	detailed = _detailed;
	if(detailed){
		layout->setRationalHeight(1.f, this);
		layout->setRationalWidth(1.f, this);

		for(auto & cell : grid){
			cell.second->setMouseEnabled(true);
		}
		roomName->setVisible(true);
	}else{
		layout->setRationalHeight(0.1f, this);
		layout->setRationalWidth(0.1f, this);

		for(auto & cell : grid){
			cell.second->setMouseEnabled(false);
		}
		roomName->setVisible(false);
	}
	layout->invalidateLayout();
}

void PD_UI_Map::buildMap(std::map<std::pair<int, int>, Room *> _houseGrid){
	// clear out the old map
	if(layout != nullptr){
		removeChild(layout);
		delete layout;
	}

	layout = new VerticalLinearLayout(world);
	layout->horizontalAlignment = kRIGHT;
	layout->verticalAlignment = kTOP;
	addChild(layout);
	grid.clear();
	
	// find the bounds
	int x1=INT_MAX,x2=INT_MIN,y1=INT_MAX,y2=INT_MIN;
	for(auto & room : _houseGrid){
		x1 = glm::min(x1, room.first.first);
		x2 = glm::max(x2, room.first.first);
		y1 = glm::min(y1, room.first.second);
		y2 = glm::max(y2, room.first.second);
	}
	int width = x2-x1+1, height = y2-y1+1;
	// make sure we have a full grid for what we need
	for(unsigned long int y = y1; y <= y2; ++y){
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(world);
		hl->setRationalWidth(1.f, layout);
		hl->setRationalHeight(1.f/height, layout);
		hl->horizontalAlignment = layout->horizontalAlignment;
		hl->verticalAlignment = kMIDDLE;
		layout->addChild(hl);

		for(unsigned long int x = x1; x <= x2; ++x){
			MapCell * cell = new MapCell(world, nullptr);
			grid[std::make_pair(x, y)] = cell;
			hl->addChild(cell);
			cell->setRationalWidth(1.f/width, hl);
			cell->setRationalHeight(1.f, hl);
			cell->eventManager.addEventListener("mousein", [cell, this](sweet::Event * _event){
				if(cell->room != nullptr && cell->isVisited()){
					roomName->setText(cell->room->definition->name);
				}else{
					roomName->setText("");
				}
			});
		}
	}

	// associate cells with rooms
	for(auto & room : _houseGrid){
		grid[room.first]->room = room.second;
	}

	setDetailed(isDetailed());
}

void PD_UI_Map::updateMap(glm::ivec2 _currentPosition){
	auto k = std::make_pair(_currentPosition.x, _currentPosition.y);
	for(auto & cell : grid){
		if(cell.first == k){
			cell.second->setBackgroundColour(1,1,1, 1);
			cell.second->setVisited(true);
		}else{
			cell.second->setBackgroundColour(1,1,1, 0.5f);
		}
	}
}