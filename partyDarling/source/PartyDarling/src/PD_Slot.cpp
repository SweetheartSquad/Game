#pragma once

#include <PD_Slot.h>
#include <RoomObject.h>

PD_Slot::PD_Slot(PD_Side _childSide, float _length, unsigned long int _maxItems) :
	childSide(_childSide),
	spaceFilled(0),
	length(_length),
	maxItems(_maxItems)
{
}