#pragma once

#include <PD_UI_Inventory.h>
#include <PD_ResourceManager.h>
#include <NumberUtils.h>

PD_UI_Inventory::PD_UI_Inventory(BulletWorld * _world) :
	HorizontalLinearLayout(_world),
	gridDirty(false),
	gridOffset(0)
{
	// this is the root element which has the backpack texture
	setBackgroundColour(1,1,1,1);
	background->setVisible(true);
	background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("BACKPACK")->texture);
	background->mesh->setScaleMode(GL_NEAREST);
	setRationalHeight(0.25);
	setRationalWidth(0.25);
	horizontalAlignment = kCENTER;
	verticalAlignment = kMIDDLE;

	// layout for grid rows
	VerticalLinearLayout * vl = new VerticalLinearLayout(world);
	vl->setBackgroundColour(1,1,1,1);
	vl->background->setVisible(true);
	vl->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GRID")->texture);
	vl->background->mesh->setScaleMode(GL_NEAREST);
	vl->setRationalHeight(0.75f);
	vl->setRationalWidth(0.5f);
	vl->setPadding(0.01f, 0.01f);

	// scrollwheel artificially triggers change event on scrollbar for grid
	vl->setMouseEnabled(true);
	vl->eventManager.addEventListener("mousewheel", [this](sweet::Event * _event){
		slider->setValue(gridOffset + _event->getFloatData("delta"));
	});

	addChild(vl);
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		// individual grid row, layout for grid cells
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(world);
		hl->setRationalWidth(1.f);
		hl->setRationalHeight(1.f/UI_INVENTORY_GRID_SIZE_Y);
		hl->setMargin(0, 0.01f);
		vl->addChild(hl);
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			// individual grid cell
			NodeUI * cell = new NodeUI(world);
			cell->setRationalWidth(1.f/UI_INVENTORY_GRID_SIZE_X);
			cell->setRationalHeight(1.f);
			cell->setMargin(0.01f, 0);
			cell->background->mesh->setScaleMode(GL_NEAREST);
			cell->boxSizing = kBORDER_BOX;
			hl->addChild(cell);
			grid[y][x] = cell;
		}
	}

	// scrollbar
	slider = new SliderController(world, &gridOffset, 0, 0, 0, false, true);
	slider->setRationalHeight(0.75f);
	slider->setWidth(50);
	slider->setStepped(1.f);
	slider->eventManager.addEventListener("change", [this](sweet::Event * _event){
		gridDirty = true;
	});
	addChild(slider);
}

void PD_UI_Inventory::pickupItem(PD_Item * _item){
	items.push_back(_item);

	// update the slider maximum based on the new item size
	slider->setValueMax(std::max(0.f, (float)(items.size()/UI_INVENTORY_GRID_SIZE_X) - UI_INVENTORY_GRID_SIZE_Y/2));
	slider->setValue(slider->getValue());

	// set flag to let us know we need to refresh the grid
	gridDirty = true;
}


void PD_UI_Inventory::update(Step * _step){
	// only refresh the grid when necessary, i.e. when it is both visible and out of date
	if(isVisible() && gridDirty){
		refreshGrid();
	}

	HorizontalLinearLayout::update(_step);
}


void PD_UI_Inventory::refreshGrid(){
	unsigned long int itemIdx = gridOffset * UI_INVENTORY_GRID_SIZE_X;
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			NodeUI * cell = grid[y][x];

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

	// clear the flag
	gridDirty = false;
}