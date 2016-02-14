#pragma once

#include <PD_UI_Map.h>
#include <PD_TilemapGenerator.h>
#include <PD_Assets.h>
#include <PD_ResourceManager.h>

MapCell::MapCell(BulletWorld * _world, Room * _room) :
	NodeUI(_world),
	visited(false),
	room(_room)
{
	background->mesh->setScaleMode(GL_NEAREST);
	boxSizing = kCONTENT_BOX;

	setVisited(false);
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
		innerLayout->setRationalHeight(1.f, this);
		innerLayout->setSquareWidth(1.f);

		for(auto & cell : grid){
			if(cell.second->room != nullptr){
				cell.second->setMouseEnabled(true);
			}
		}
		roomName->setVisible(true);
	}else{
		innerLayout->setRationalHeight(0.1f, this);
		innerLayout->setSquareWidth(1.f);

		for(auto & cell : grid){
			cell.second->setMouseEnabled(false);
		}
		roomName->setVisible(false);
	}
	invalidateLayout();
}

void PD_UI_Map::buildMap(std::map<std::pair<int, int>, Room *> _houseGrid){
	// clear out the old map
	if(layout != nullptr){
		removeChild(layout);
		delete layout;
	}
	grid.clear();

	layout = new VerticalLinearLayout(world);
	addChild(layout);
	layout->horizontalAlignment = kRIGHT;
	layout->verticalAlignment = kTOP;
	layout->setRationalHeight(1.f, this);
	layout->setRationalWidth(1.f, this);
	
	innerLayout = new VerticalLinearLayout(world);
	layout->addChild(innerLayout);
	innerLayout->horizontalAlignment = kRIGHT;
	innerLayout->verticalAlignment = kTOP;
	innerLayout->setRationalHeight(1.f, this);
	innerLayout->setSquareWidth(1.f);
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
		innerLayout->addChild(hl);
		hl->setRationalWidth(1.f, innerLayout);
		hl->setRationalHeight(1.f/height, innerLayout);
		hl->horizontalAlignment = innerLayout->horizontalAlignment;
		hl->verticalAlignment = innerLayout->verticalAlignment;
		hl->horizontalAlignment = innerLayout->horizontalAlignment;

		for(unsigned long int x = x1; x <= x2; ++x){
			MapCell * cell = new MapCell(world, nullptr);
			grid[std::make_pair(x, y)] = cell;
			hl->addChild(cell);
			cell->setRationalHeight(1.f, hl);
			cell->setSquareWidth(1.f);
			cell->boxSizing = kCONTENT_BOX;
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
		if(cell.second->room != nullptr){ // cells without rooms are always ignored
			if(cell.first == k){
				cell.second->setBackgroundColour(1,1,1, 1);
			}else{
				cell.second->setBackgroundColour(1,1,1, 0.5f);
			}
			cell.second->setVisited(true);

			cell.second->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture(cell.second->room->locked ? "MAPCELL-LOCKED" : "MAPCELL")->texture);
			
		}
	}
}