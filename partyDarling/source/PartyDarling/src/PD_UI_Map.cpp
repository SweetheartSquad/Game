#pragma once

#include <PD_UI_Map.h>
#include <PD_TilemapGenerator.h>
#include <PD_Assets.h>
#include <PD_ResourceManager.h>

MapCell::MapCell(BulletWorld * _world, Room * _room) :
	NodeUI(_world),
	room(_room)
{
	background->mesh->setScaleMode(GL_NEAREST);
	boxSizing = kCONTENT_BOX;
	setBackgroundColour(1,1,1, 0.75f);
}

PD_UI_Map::PD_UI_Map(BulletWorld * _world, Font * _font, ComponentShaderText * _textShader) :
	NodeUI(_world),
	enabled(true),
	layout(nullptr)
{
	background->setVisible(false);

	compass = new NodeUI(world);
	compass->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("COMPASS")->texture);
	compass->background->mesh->setScaleMode(GL_NEAREST);
	compass->setPixelHeight(32);
	compass->setPixelWidth(32);
	compass->boxSizing = kCONTENT_BOX;
	for(auto & v : compass->background->mesh->vertices){
		v.x -= 0.5f;
		v.y -= 0.5f;
	}
	compass->background->mesh->dirty = true;
	childTransform->addChild(compass, false);
	compass->nodeUIParent = this;
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
	innerLayout->horizontalAlignment = kCENTER;
	innerLayout->verticalAlignment = kMIDDLE;
	//innerLayout->setRationalHeight(0.9f, layout);
	//innerLayout->setSquareWidth(1.f);

	innerLayout->setRationalHeight(0.2f, this);
	innerLayout->setSquareWidth(1.f);

	VerticalLinearLayout * innerLayout2 = new VerticalLinearLayout(world);
	innerLayout->addChild(innerLayout2);
	innerLayout2->horizontalAlignment = kRIGHT;
	innerLayout2->verticalAlignment = kTOP;
	innerLayout2->setRationalHeight(0.9f, innerLayout);
	innerLayout2->setSquareWidth(1.f);


	// find the bounds
	int x1=INT_MAX,x2=INT_MIN,y1=INT_MAX,y2=INT_MIN;
	for(auto & room : _houseGrid){
		x1 = glm::min(x1, room.first.first);
		x2 = glm::max(x2, room.first.first);
		y1 = glm::min(y1, room.first.second);
		y2 = glm::max(y2, room.first.second);
	}
	int size = glm::max(x2-x1, y2-y1)+1;
	// make sure we have a full grid for what we need
	for(int y = y1-1; y < glm::min(y2, y1+size)+2; ++y){
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(world);
		innerLayout2->addChild(hl);
		hl->setRationalWidth(1.f, innerLayout2);
		hl->setRationalHeight(1.f/size, innerLayout2);
		hl->horizontalAlignment = innerLayout2->horizontalAlignment;
		hl->verticalAlignment = innerLayout2->verticalAlignment;
		hl->horizontalAlignment = innerLayout2->horizontalAlignment;
		
		for(int x = x1-1; x < glm::min(x2, x1+size)+2; ++x){
			MapCell * cell = new MapCell(world, nullptr);
			grid[std::make_pair(x, y)] = cell;
			hl->addChild(cell);
			cell->setRationalHeight(1.f, hl);
			cell->setSquareWidth(1.f);
			cell->boxSizing = kCONTENT_BOX;
			if(x < x1 || y < y1 || x > x2 || y > y2){
				// border cell
				std::stringstream ss;
				ss << "MAPCELL-BORDER-";
				if(y < y1){
					ss << "B";
				}else if(y > y2){
					ss << "T";
				}
				if(x < x1){
					ss << "R";
				}else if(x > x2){
					ss << "L";
				}
				cell->setBackgroundColour(1,1,1, 1);
				cell->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture(ss.str())->texture);
			}
		}
	}

	// associate cells with rooms
	for(auto & room : _houseGrid){
		grid[room.first]->room = room.second;
	}
}


bool PD_UI_Map::adjacent(std::pair<int, int> _cell, glm::ivec2 _housePosition){
	return
		((glm::abs(_cell.first - _housePosition.x) == 1)
		&&
		(glm::abs(_cell.second - _housePosition.y) == 0)
		)
		||
		(
		(glm::abs(_cell.first - _housePosition.x) == 0)
		&&
		(glm::abs(_cell.second - _housePosition.y) == 1)
		);
				
}

void PD_UI_Map::updateMap(glm::ivec2 _currentPosition){
	if(compass->nodeUIParent != nullptr){
		compass->nodeUIParent->childTransform->removeChild(compass);
	}
	auto k = std::make_pair(_currentPosition.x, _currentPosition.y);
	for(auto & cell : grid){
		if(cell.second->room != nullptr){
			// hidden cells which are adjacent to the current cell become seen
			if(adjacent(cell.first, _currentPosition)){
					if(cell.second->room->visibility == Room::kHIDDEN){
						cell.second->room->visibility = Room::kSEEN;
					}
				}

			if(cell.first == k){
				cell.second->childTransform->addChild(compass, false);
				compass->childTransform->translate(glm::vec3(cell.second->getWidth()*0.5f, cell.second->getHeight()*0.5f, 0), false);
				compass->nodeUIParent = cell.second;
				compass->setRationalHeight(1.f, cell.second);
				compass->setSquareWidth(1.f);
				// the current cell becomes entered
				cell.second->room->visibility = Room::kENTERED;
			}

			// hidden cells aren't drawn
			// seen cells are drawn with icons
			// entered cells are drawn without icons
			switch(cell.second->room->visibility){
			case Room::kHIDDEN:
				break;
			case Room::kSEEN:
				cell.second->setBackgroundColour(1,1,1, 1);
				cell.second->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture(cell.second->room->locked ? "MAPCELL-LOCKED" : (cell.second->room->definition->locked ? "MAPCELL-UNLOCKED" : "MAPCELL-UNENTERED"))->texture);
				break;
			case Room::kENTERED:
				cell.second->setBackgroundColour(1,1,1, 1);
				cell.second->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("MAPCELL")->texture);
				break;
			}
		}
	}
}

void PD_UI_Map::updateCompass(float _angle){
	compass->background->childTransform->setOrientation(glm::angleAxis(_angle, glm::vec3(0,0,1)));
}
