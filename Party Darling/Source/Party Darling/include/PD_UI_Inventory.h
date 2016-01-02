#pragma once

#include <sweet/UI.h>
#include <PD_Item.h>

#define UI_INVENTORY_GRID_SIZE_X 5
#define UI_INVENTORY_GRID_SIZE_Y 5

class PD_UI_Inventory : public HorizontalLinearLayout{
private:
	bool gridDirty;
public:
	// all of the currently held inventory items
	std::vector<PD_Item *> items;

	// the grid of UI elements used to display inventory items
	// array access is [y][x]
	NodeUI * grid[UI_INVENTORY_GRID_SIZE_Y][UI_INVENTORY_GRID_SIZE_X];
	
	// scrollbar for grid
	SliderController * slider;

	// the number of rows that have been scrolled since the top of the grid
	float gridOffset;

	PD_UI_Inventory(BulletWorld * _world);

	// add an item to the inventory l
	void pickupItem(PD_Item * _item);
	
	// updates the grid of displayed inventory items based on the list
	// of currently held items and the view offset
	void refreshGrid();


	virtual void update(Step * _step) override;
};