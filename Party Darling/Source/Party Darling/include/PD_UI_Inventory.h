#pragma once

#include <sweet/UI.h>
#include <PD_Item.h>

#define UI_INVENTORY_GRID_SIZE_X 5
#define UI_INVENTORY_GRID_SIZE_Y 5

class PD_UI_Inventory : public HorizontalLinearLayout{
private:
	bool gridDirty;

	// the currenty selected item
	// nullptr if nothing is selected
	PD_Item * selectedItem;
	bool itemHovered;

	// sets the info panel on the side
	// to match the contents of _item
	// i.e. displays its texture, name, and description
	// if _item == nullptr, clears the panel instead
	void setInfoPanel(PD_Item * _item);
public:
	// all of the currently held inventory items
	std::vector<PD_Item *> items;

	// the grid of UI elements used to display inventory items
	// array access is [y][x]
	NodeUI * grid[UI_INVENTORY_GRID_SIZE_Y][UI_INVENTORY_GRID_SIZE_X];
	
	// scrollbar for grid
	SliderController * slider;


	NodeUI * itemImage;
	TextArea * itemDescription;
	TextLabel * itemName;
	VerticalLinearLayout * infoLayout;
	VerticalLinearLayout * gridLayout;

	// the number of rows that have been scrolled since the top of the grid
	float gridOffset;

	PD_UI_Inventory(BulletWorld * _world);

	// add an item to the inventory
	void pickupItem(PD_Item * _item);
	
	// return the inventory item stored in grid[_x][_y]
	PD_Item * getItem(unsigned long int _x, unsigned long int _y);
	
	// returns the item index for the grid cell with the provided coordinates
	// i.e. _x + (_y+gridOffset) * UI_INVENTORY_GRID_SIZE_X
	unsigned long int getItemIdx(unsigned long int _x, unsigned long int _y);
	
	// put _item in the player's "hands"
	void selectItem(PD_Item * _item);

	// updates the grid of displayed inventory items based on the list
	// of currently held items and the view offset
	void refreshGrid();

	// removes the selected item from the inventory
	// returns the selected item
	// if nullptr, does nothing
	PD_Item * removeSelected();

	// returns the currently selected item, or nullptr if nothing is selected
	PD_Item * getSelected();


	// hides the inventory and disables interaction
	void close();
	// opens the inventory and enabled interaction
	void open();

	virtual void update(Step * _step) override;
};