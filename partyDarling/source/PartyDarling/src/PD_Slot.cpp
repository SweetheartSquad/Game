#pragma once

#include <PD_Slot.h>
#include <RoomObject.h>

PD_Slot::PD_Slot(PD_Side _childSide, float _loc, float _length) :
	childSide(_childSide),
	loc(_loc),
	length(_length)
{
}