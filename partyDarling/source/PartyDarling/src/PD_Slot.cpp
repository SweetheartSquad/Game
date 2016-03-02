#pragma once

#include <PD_Slot.h>
#include <RoomObject.h>

PD_Slot::PD_Slot(PD_Side _childSide, float _length, bool _overflow) :
	childSide(_childSide),
	spaceFilled(0),
	length(_length),
	overflow(_overflow)
{
}