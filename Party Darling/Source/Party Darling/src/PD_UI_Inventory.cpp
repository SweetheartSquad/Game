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
			cell->setMouseEnabled(true);
			cell->setVisible(false);
			hl->addChild(cell);

			// event listeners
			cell->eventManager.addEventListener("mousein", [cell](sweet::Event * _event){
				cell->setBackgroundColour(1,1,1,0.5f);
			});
			cell->eventManager.addEventListener("mouseout", [cell](sweet::Event * _event){
				cell->setBackgroundColour(1,1,1,1.f);
			});
			cell->eventManager.addEventListener("click", [this, y, x](sweet::Event * _event){
				selectItem(getItem(x, y));
			});

			// save a reference to the cell in the grid
			grid[y][x] = cell;
		}
	}

	// scrollbar
	slider = new SliderController(world, &gridOffset, 0, 0, 0, false, true);
	slider->setRationalHeight(0.75f);
	slider->setWidth(10);
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

PD_Item * PD_UI_Inventory::getItem(unsigned long int _x, unsigned long int _y){
	// reverse-engineer the item index based on the cell coordinates and grid offset
	unsigned long int itemIdx = _x + (_y+gridOffset) * UI_INVENTORY_GRID_SIZE_X;

	// return the item at the calculated index, or nullptr if the cell is empty
	return itemIdx < items.size() ? items.at(itemIdx) : nullptr;
}

void PD_UI_Inventory::selectItem(PD_Item * _item){
	std::cout << "hey gj you clicked an inventory item: " << _item <<  std::endl;
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
				cell->setVisible(true);
				++itemIdx;
			}else{
				cell->setVisible(false);
			}
		}
	}

	// clear the flag
	gridDirty = false;
}