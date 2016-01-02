#pragma once

#include <sweet/UI.h>
#include <PD_Item.h>

#define UI_INVENTORY_GRID_SIZE_X 5
#define UI_INVENTORY_GRID_SIZE_Y 5

class PD_UI_Inventory : public VerticalLinearLayout{
private:
	bool gridDirty;
public:
	// all of the currently held inventory items
	std::vector<PD_Item *> items;

	// the grid of UI elements used to display inventory items
	// array access is [x][y]
	NodeUI * grid[UI_INVENTORY_GRID_SIZE_X][UI_INVENTORY_GRID_SIZE_Y];

	PD_UI_Inventory(BulletWorld * _world);

	// add an item to the inventory l
	void pickupItem(PD_Item * _item);
	
	// updates the grid of displayed inventory items based on the list
	// of currently held items and the view offset
	void refreshGrid();


	virtual void update(Step * _step) override;
};