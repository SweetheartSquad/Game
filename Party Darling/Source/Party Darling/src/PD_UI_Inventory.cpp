#pragma once

#include <PD_UI_Inventory.h>
#include <PD_ResourceManager.h>
#include <NumberUtils.h>
#include <shader/ComponentShaderText.h>

PD_UI_Inventory::PD_UI_Inventory(BulletWorld * _world) :
	HorizontalLinearLayout(_world),
	gridDirty(false),
	gridOffset(0),
	selectedItem(nullptr),
	itemHovered(false)
{
	// this is the root element which has the backpack texture
	setBackgroundColour(1,1,1,1);
	background->setVisible(true);
	background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("BACKPACK")->texture);
	background->mesh->setScaleMode(GL_NEAREST);
	setRationalHeight(1.f);
	setRationalWidth(1.f);
	horizontalAlignment = kCENTER;
	verticalAlignment = kMIDDLE;

	// layout for grid rows
	gridLayout = new VerticalLinearLayout(world);
	gridLayout->setBackgroundColour(1,1,1,1);
	gridLayout->background->setVisible(true);
	gridLayout->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GRID")->texture);
	gridLayout->background->mesh->setScaleMode(GL_NEAREST);
	gridLayout->setRationalHeight(0.75f);
	gridLayout->setRationalWidth(0.5f);
	gridLayout->setPadding(0.01f, 0.01f);

	// scrollwheel artificially triggers change event on scrollbar for grid
	gridLayout->setMouseEnabled(true);
	gridLayout->eventManager.addEventListener("mousewheel", [this](sweet::Event * _event){
		slider->setValue(gridOffset + _event->getFloatData("delta"));
	});

	addChild(gridLayout);
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		// individual grid row, layout for grid cells
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(world);
		hl->setRationalWidth(1.f);
		hl->setRationalHeight(1.f/UI_INVENTORY_GRID_SIZE_Y);
		hl->setMargin(0, 0.01f);
		gridLayout->addChild(hl);
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
			cell->eventManager.addEventListener("mousein", [this, y, x, cell](sweet::Event * _event){
				cell->setBackgroundColour(1,1,1, 0.5f);
				while(itemImage->background->mesh->textureCount() > 0){
					itemImage->background->mesh->popTexture2D();
				}
				PD_Item * item = getItem(x, y);
				if(item != nullptr){
					itemImage->background->mesh->pushTexture2D(PD_ResourceManager::scenario->defaultTexture->texture);
					//itemImage->background->mesh->pushTexture2D(item->mesh->textures.at(0));
					
					itemName->setText(item->name);
					itemDescription->setText(item->description);
				}
				infoLayout->invalidateLayout();
				itemHovered = true;
			});
			cell->eventManager.addEventListener("mouseout", [this, cell](sweet::Event * _event){
				cell->setBackgroundColour(1,1,1, 1.f);
				if(!itemHovered){
					while(itemImage->background->mesh->textureCount() > 0){
						itemImage->background->mesh->popTexture2D();
					}
					itemName->setText("");
					itemDescription->setText("");
				}
			});
			cell->eventManager.addEventListener("click", [this, cell, y, x](sweet::Event * _event){
				selectItem(getItem(x, y));
				cell->eventManager.triggerEvent("mouseout");
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

	{
		infoLayout = new VerticalLinearLayout(world);
		infoLayout->background->setVisible(true);
		infoLayout->setBackgroundColour(1,1,1,1);
		
		infoLayout->horizontalAlignment = kCENTER;
		infoLayout->verticalAlignment = kTOP;

		infoLayout->setHeight(0.75f);
		infoLayout->setWidth(300);

		addChild(infoLayout);

		ComponentShaderText * textShader = new ComponentShaderText(true);
		textShader->load();

		itemName = new TextLabel(world, PD_ResourceManager::scenario->defaultFont->font, textShader, 1.f);
		itemName->setText("test");
		itemName->setRationalWidth(1.f);

		itemImage = new NodeUI(world);
		itemImage->setRationalWidth(1.f);
		itemImage->setHeight(300);
		itemImage->setRationalWidth(1.f);
		itemImage->setBackgroundColour(1,1,1,1);

		itemDescription = new TextArea(world, PD_ResourceManager::scenario->defaultFont->font, textShader, 1.f);
		itemDescription->setText("test");
		itemDescription->setRationalWidth(1.f);
		itemDescription->verticalAlignment = kTOP;
		
		infoLayout->addChild(itemName);
		infoLayout->addChild(itemImage);
		infoLayout->addChild(itemDescription);
	}
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
	unsigned long int itemIdx = getItemIdx(_x, _y);
	// return the item at the calculated index, or nullptr if the cell is empty
	return itemIdx < items.size() ? items.at(itemIdx) : nullptr;
}
unsigned long int PD_UI_Inventory::getItemIdx(unsigned long int _x, unsigned long int _y){
	// reverse-engineer the item index based on the cell coordinates and grid offset
	return _x + (_y+gridOffset) * UI_INVENTORY_GRID_SIZE_X;
}

void PD_UI_Inventory::selectItem(PD_Item * _item){
	selectedItem = _item;
	eventManager.triggerEvent("itemSelected");
}

PD_Item * PD_UI_Inventory::getSelected(){
	return selectedItem;
}

PD_Item * PD_UI_Inventory::removeSelected(){
	PD_Item * res = selectedItem;
	// if nothing is selected, log a warning and return early
	if(res == nullptr){
		Log::warn("Tried to remove the selected inventory item but nothing was selected.");
		return nullptr;
	}

	// search the inventory for the selected item
	// when found, remove it, flag the grid as dirty
	// (assumes that there are no duplicated)
	for(signed long int i = items.size()-1; i >= 0; --i){
		if(items.at(i) == res){
			items.erase(items.begin() + i);
			gridDirty = true;
			selectedItem = nullptr;
			break;
		}
	}

	// if we still have a selected item at this point, it means
	// that it wasn't in the inventory in the first place
	// this shouldn't happen, so log an error
	if(selectedItem != nullptr){
		Log::error("Tried to remove selected inventory item, but the item wasn't in the inventory?");
	}

	return res;
}

void PD_UI_Inventory::update(Step * _step){
	// only refresh the grid when necessary, i.e. when it is both visible and out of date
	if(isVisible() && gridDirty){
		refreshGrid();
	}

	HorizontalLinearLayout::update(_step);

	itemHovered = false;
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

void PD_UI_Inventory::open(){
	setVisible(true);
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			grid[y][x]->setMouseEnabled(true);
		}
	}
	gridLayout->setMouseEnabled(true);
}

void PD_UI_Inventory::close(){
	setVisible(false);
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			grid[y][x]->setMouseEnabled(false);
		}
	}
	gridLayout->setMouseEnabled(false);
}