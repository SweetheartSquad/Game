#pragma once

#include <PD_UI_Inventory.h>
#include <PD_ResourceManager.h>
#include <NumberUtils.h>

PD_UI_Inventory::PD_UI_Inventory(BulletWorld * _world) :
	VerticalLinearLayout(_world),
	gridDirty(false)
{
	setBackgroundColour(1,1,1,1);
	background->setVisible(true);
	background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("BACKPACK")->texture);
	background->mesh->setScaleMode(GL_NEAREST);
	setRationalHeight(0.25);
	setRationalWidth(0.25);
	horizontalAlignment = kCENTER;
	verticalAlignment = kMIDDLE;

	VerticalLinearLayout * vl = new VerticalLinearLayout(world);
	vl->setBackgroundColour(1,1,1,1);
	vl->background->setVisible(true);
	vl->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GRID")->texture);
	vl->background->mesh->setScaleMode(GL_NEAREST);
	vl->setRationalHeight(0.75f);
	vl->setRationalWidth(0.5f);
	vl->setPadding(0.01f, 0.01f);

	addChild(vl);
	for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(world);
		hl->setRationalWidth(1.f);
		hl->setRationalHeight(1.f/UI_INVENTORY_GRID_SIZE_Y);
		hl->setMargin(0, 0.01f);
		vl->addChild(hl);
		for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
			NodeUI * cell = new NodeUI(world);
			cell->setRationalWidth(1.f/UI_INVENTORY_GRID_SIZE_X);
			cell->setRationalHeight(1.f);
			cell->setMargin(0.01f, 0);
			//cell->setBackgroundColour(sweet::NumberUtils::randomFloat(), 0, 0, 1);
			cell->background->mesh->setScaleMode(GL_NEAREST);
			cell->boxSizing = kBORDER_BOX;
			hl->addChild(cell);
			grid[x][y] = cell;
		}
	}
}

void PD_UI_Inventory::pickupItem(PD_Item * _item){
	items.push_back(_item);

	gridDirty = true;
}


void PD_UI_Inventory::update(Step * _step){
	if(isVisible() && gridDirty){
		refreshGrid();
	}

	VerticalLinearLayout::update(_step);
}


void PD_UI_Inventory::refreshGrid(){
	unsigned long int itemIdx = 0;
	for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
		for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
			NodeUI * cell = grid[x][y];

			// remove any existing textures on the cell
			while(cell->background->mesh->textures.size() > 0){
				cell->background->mesh->popTexture2D();
			}

			// if there is an item which should belong to the cell, add it's texture
			if(itemIdx < items.size()){
				PD_Item * item = items.at(itemIdx);

				cell->background->mesh->pushTexture2D(item->mesh->textures.at(0));
				++itemIdx;
			}
		}
	}

	gridDirty = false;
}