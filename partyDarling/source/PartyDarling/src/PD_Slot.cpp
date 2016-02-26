#pragma once

#include <PD_Slot.h>
#include <RoomObject.h>

PD_Slot::PD_Slot(PD_Side _childSide, float _length) :
	childSide(_childSide),
	spaceFilled(0),
	length(_length)
{
}