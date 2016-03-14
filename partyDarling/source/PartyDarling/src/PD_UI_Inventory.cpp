#pragma once

#include <PD_UI_Inventory.h>
#include <PD_UI_DissCard.h>
#include <PD_ResourceManager.h>
#include <PD_Assets.h>

#include <NumberUtils.h>
#include <shader/ComponentShaderText.h>

PD_UI_Inventory::PD_UI_Inventory(BulletWorld * _world, Player * _player) :
	NodeUI(_world),
	gridDirty(false),
	gridOffset(0),
	selectedItem(nullptr),
	itemHovered(false),
	enabled(true)
{
	background->setVisible(false);

	ComponentShaderText * textShader = new ComponentShaderText(true);
	textShader->setColor(86/255.f, 137/255.f, 135/255.f, 1.f);

	HorizontalLinearLayout * backpackContainer = new HorizontalLinearLayout(_world);
	addChild(backpackContainer);
	backpackContainer->horizontalAlignment = kCENTER;
	backpackContainer->verticalAlignment = kMIDDLE;
	backpackContainer->setRationalWidth(1.f, this);
	backpackContainer->setRationalHeight(1.f, this);

	// this is the root element which has the backpack texture
	HorizontalLinearLayout * root = new HorizontalLinearLayout(_world);
	backpackContainer->addChild(root);
	root->background->setVisible(true);
	root->setBackgroundColour(1,1,1,1);
	root->setRationalHeight(1.f, this);
	root->setSquareWidth(1.f);
	root->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("BACKPACK")->texture);
	root->background->mesh->setScaleMode(GL_NEAREST);
	root->horizontalAlignment = kCENTER;
	root->verticalAlignment = kMIDDLE;

	HorizontalLinearLayout * layout = new HorizontalLinearLayout(world);
	layout->horizontalAlignment = kCENTER;
	layout->verticalAlignment = kMIDDLE;
	root->addChild(layout);
	layout->setRationalHeight(1.f, root);
	layout->setSquareWidth(1.f);
	layout->setMargin(0, 0.05f);
	

	// layout for grid rows
	gridLayout = new VerticalLinearLayout(world);
	layout->addChild(gridLayout);
	gridLayout->setBackgroundColour(1,1,1,1);
	gridLayout->background->setVisible(true);
	gridLayout->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GRID")->texture);
	gridLayout->background->mesh->setScaleMode(GL_NEAREST);
	gridLayout->setRationalWidth(0.5f, layout);
	gridLayout->setSquareHeight(1.f);

	// scrollwheel artificially triggers change event on scrollbar for grid
	gridLayout->setMouseEnabled(true);
	gridLayout->eventManager->addEventListener("mousewheel", [this](sweet::Event * _event){
		scrollbar->setValue(gridOffset + _event->getFloatData("delta"));
	});

	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		// individual grid row, layout for grid cells
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(world);
		gridLayout->addChild(hl);
		hl->setRationalWidth(1.f, gridLayout);
		hl->setRationalHeight(1.f/UI_INVENTORY_GRID_SIZE_Y, gridLayout);
		hl->setMargin(0, UI_INVENTORY_GRID_PADDING);
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			// individual grid cell
			HorizontalLinearLayout * cellLayout = new HorizontalLinearLayout(world);
			hl->addChild(cellLayout);
			cellLayout->horizontalAlignment = kCENTER;
			cellLayout->verticalAlignment = kMIDDLE;
			cellLayout->setRationalHeight(1.f, hl);
			cellLayout->setSquareWidth(1.f);
			cellLayout->setMargin(UI_INVENTORY_GRID_PADDING, 0);
			cellLayout->boxSizing = kCONTENT_BOX;
			cellLayout->setMouseEnabled(true);

			NodeUI * cell = new NodeUI(world);
			cellLayout->addChild(cell);
			cell->setRationalHeight(1.f, cellLayout);
			cell->setRationalWidth(1.f, cellLayout);
			cell->background->mesh->setScaleMode(GL_NEAREST);
			cell->setVisible(false);

			// event listeners
			cellLayout->eventManager->addEventListener("mousein", [this, y, x, cell](sweet::Event * _event){
				cell->setBackgroundColour(1,1,1, 0.5f);
				setInfoPanel(getItem(x, y));
			});
			cellLayout->eventManager->addEventListener("mouseout", [this, cell](sweet::Event * _event){
				cell->setBackgroundColour(1,1,1, 1.f);
				setInfoPanel(nullptr);
			});
			cellLayout->eventManager->addEventListener("click", [this, cell, y, x](sweet::Event * _event){
				selectItem(getItem(x, y));
				cell->eventManager->triggerEvent("mouseout");
			});

			// save a reference to the cell in the grid
			grid[y][x] = cell;
		}
	}

	// scrollbar
	scrollbar = new SliderController(world, &gridOffset, 0, 0, 0, false, true);
	layout->addChild(scrollbar);
	scrollbar->marginRight.setPixelSize(5);
	scrollbar->setRationalWidth(UI_INVENTORY_GRID_PADDING, layout);
	scrollbar->height.setRationalSize(1.f, &gridLayout->height);
	scrollbar->setStepped(1.f);
	scrollbar->eventManager->addEventListener("change", [this](sweet::Event * _event){
		gridDirty = true;
	});
	scrollbar->setBackgroundColour(142/255.f, 206/255.f, 213/255.f, 1.f);
	scrollbar->fill->setBackgroundColour(220/255.f, 255/255.f, 252/255.f, 1.f);

	{
		infoLayout = new VerticalLinearLayout(world);
		layout->addChild(infoLayout);
		//infoLayout->background->setVisible(true);
		//infoLayout->setBackgroundColour(1,1,1,1);
		
		infoLayout->horizontalAlignment = kCENTER;
		infoLayout->verticalAlignment = kMIDDLE;

		infoLayout->setRationalWidth(0.5f, layout);
		infoLayout->setRationalHeight(1.f, layout);

		itemName = new TextLabel(world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
		infoLayout->addChild(itemName);
		itemName->setText("");
		itemName->setRationalWidth(1.f, infoLayout);
		itemName->horizontalAlignment = kCENTER;

		HorizontalLinearLayout * itemImageLayout = new HorizontalLinearLayout(world);
		infoLayout->addChild(itemImageLayout);
		itemImageLayout->setRationalWidth(0.5f, infoLayout);
		itemImageLayout->setSquareHeight(1.f);
		itemImageLayout->horizontalAlignment = kCENTER;
		itemImageLayout->verticalAlignment = kMIDDLE;

		itemImage = new NodeUI(world);
		itemImageLayout->addChild(itemImage);
		itemImage->setRationalWidth(1.f, itemImageLayout);
		itemImage->setRationalHeight(1.f, itemImageLayout);
		itemImage->setBackgroundColour(1,1,1,1);
		itemImage->background->mesh->setScaleMode(GL_NEAREST);
		itemImage->setVisible(false);

		itemDescription = new TextArea(world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
		itemDescription->setWrapMode(kWORD);
		infoLayout->addChild(itemDescription);
		itemDescription->setText("");
		itemDescription->setRationalWidth(1.f, infoLayout);
		itemDescription->verticalAlignment = kTOP;
		
	}
	
	HorizontalLinearLayout * cardContainer = new HorizontalLinearLayout(_world);
	addChild(cardContainer);
	cardContainer->horizontalAlignment = kLEFT;
	cardContainer->verticalAlignment = kMIDDLE;
	cardContainer->setRationalWidth(0.9f, this);
	cardContainer->setRationalHeight(1.f, this);
	cardContainer->setMarginTop(0.1f);
	cardContainer->setMarginBottom(0.65f);
	cardContainer->setMarginLeft(0.1f);

	// Diss Card
	playerCard = new PD_UI_DissCard(_world, _player);
	cardContainer->addChild(playerCard);
	playerCard->setRationalHeight(1.f, cardContainer);
	playerCard->setSquareWidth(1.4f);
	playerCard->firstParent()->rotate(15.f, 0.f, 0.f, 1.f, kOBJECT);


	// disable and hide by default
	disable();
}

void PD_UI_Inventory::setInfoPanel(PD_Item * _item){
	if(_item != nullptr){
		// get the item texture
		Texture * itemTex = _item->mesh->textures.at(0);

		// update to match the item's the image, name, and description
		itemImage->setVisible(true);
		itemImage->background->mesh->replaceTextures(itemTex);
		itemName->setText(_item->definition->name);
		itemDescription->setText(_item->definition->description);

		// make sure the item is displayed at the correct size
		if(itemTex->width > itemTex->height){
			itemImage->setRationalWidth(1.f, itemImage->nodeUIParent);
			itemImage->setSquareHeight((float)itemTex->height/itemTex->width);
		}else{
			itemImage->setRationalHeight(1.f, itemImage->nodeUIParent);
			itemImage->setSquareWidth((float)itemTex->width/itemTex->height);
		}

		// flag that we have set an item for this frame so that we don't accidentally clear it out later
		itemHovered = true;
	}else if(!itemHovered){
		// no item, so just clear the panel
		itemImage->background->mesh->clearTextures();
		itemImage->setVisible(false);
		itemName->setText("");
		itemDescription->setText("");
	}
	infoLayout->invalidateLayout();
}

void PD_UI_Inventory::pickupItem(PD_Item * _item){
	items.push_back(_item);

	// update the slider maximum based on the new item size
	scrollbar->setValueMax(std::max(0.f, (float)(items.size()/UI_INVENTORY_GRID_SIZE_X) - UI_INVENTORY_GRID_SIZE_Y/2));
	scrollbar->setValue(scrollbar->getValue());

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
	if(selectedItem != nullptr){
		eventManager->triggerEvent("itemSelected");
	}
}

PD_Item * PD_UI_Inventory::getSelected(){
	return selectedItem;
}

PD_Item * PD_UI_Inventory::removeItem(PD_Item * _item){
	PD_Item * res = _item;
	// if nothing is selected, log a warning and return early
	if(res == nullptr){
		Log::warn("Tried to remove the selected inventory item but nothing was selected.");
		return nullptr;
	}

	// search the inventory for the selected item
	// when found, remove it, flag the grid as dirty
	// (assumes that there are no duplicated)
	for(signed long int i = items.size()-1; i >= 0; --i){
		if(items.at(i) == _item){
			items.erase(items.begin() + i);
			gridDirty = true;
			_item = nullptr;
			break;
		}
	}

	// if we still have a selected item at this point, it means
	// that it wasn't in the inventory in the first place
	// this shouldn't happen, so log an error
	if(_item != nullptr){
		Log::error("Tried to remove selected inventory item, but the item wasn't in the inventory?");
	}

	return res;
}

PD_Item * PD_UI_Inventory::removeSelected(){
	PD_Item * res = removeItem(selectedItem);
	selectedItem = nullptr;
	return res;
}

void PD_UI_Inventory::update(Step * _step){
	// only refresh the grid when necessary, i.e. when it is both visible and out of date
	if(isVisible() && gridDirty){
		refreshGrid();
	}

	NodeUI::update(_step);

	itemHovered = false;
}


void PD_UI_Inventory::refreshGrid(){
	unsigned long int itemIdx = gridOffset * UI_INVENTORY_GRID_SIZE_X;
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			NodeUI * cell = grid[y][x];

			// remove any existing textures on the cell
			cell->background->mesh->clearTextures();

			// if there is an item which should belong to the cell, add it's texture
			if(itemIdx < items.size()){
				PD_Item * item = items.at(itemIdx);

				Texture * itemTex = item->mesh->textures.at(0);

				cell->background->mesh->replaceTextures(itemTex);
				cell->setVisible(true);

				if(itemTex->width > itemTex->height){
					cell->setRationalWidth(1.f, cell->nodeUIParent);
					cell->setSquareHeight((float)itemTex->height/itemTex->width);
				}else{
					cell->setRationalHeight(1.f, cell->nodeUIParent);
					cell->setSquareWidth((float)itemTex->width/itemTex->height);
				}

				++itemIdx;
			}else{
				cell->setVisible(false);
			}
		}
	}

	invalidateLayout();

	// clear the flag
	gridDirty = false;
}

void PD_UI_Inventory::enable(){
	enabled = true;
	setVisible(true);
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			grid[y][x]->nodeUIParent->setMouseEnabled(true);
		}
	}
	gridLayout->setMouseEnabled(true);
	scrollbar->setMouseEnabled(true);

	playerCard->updateStats();
}

void PD_UI_Inventory::disable(){
	enabled = false;
	setVisible(false);
	for(unsigned long int y = 0; y < UI_INVENTORY_GRID_SIZE_Y; ++y){
		for(unsigned long int x = 0; x < UI_INVENTORY_GRID_SIZE_X; ++x){
			grid[y][x]->nodeUIParent->setMouseEnabled(false);
		}
	}
	gridLayout->setMouseEnabled(false);
	scrollbar->setMouseEnabled(false);
}

bool PD_UI_Inventory::isEnabled(){
	return enabled;
}