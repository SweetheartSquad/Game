#pragma once

#include <PD_UI_Map.h>
#include <PD_TilemapGenerator.h>

PD_UI_Map::PD_UI_Map(BulletWorld * _world) :
	NodeUI(_world),
	enabled(true),
	layout(nullptr)
{
	background->setVisible(false);
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

void PD_UI_Map::updateMap(std::map<std::pair<int, int>, Room *> _houseGrid){
	// clear out the old map
	if(layout != nullptr){
		removeChild(layout);
		delete layout;
	}

	layout = new VerticalLinearLayout(world);
	layout->setRationalWidth(1.f);
	layout->setRationalHeight(1.f);
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
		hl->setRationalWidth(1.f);
		hl->setRationalHeight(1.f/height);
		layout->addChild(hl);

		for(unsigned long int x = x1; x <= x2; ++x){
			NodeUI * cell = new NodeUI(world);
			cell->setRationalHeight(1.f);
			cell->setRationalWidth(1.f/width);
			cell->background->mesh->setScaleMode(GL_NEAREST);
			cell->setVisible(false);
			grid[std::make_pair(x, y)] = cell;
			hl->addChild(cell);
		}
	}

	// setup the textures
	for(auto & room : _houseGrid){
		NodeUI * cell = grid[room.first];
		cell->background->mesh->replaceTextures(room.second->tilemap);
		cell->setVisible(true);
	}

	invalidateLayout();
}