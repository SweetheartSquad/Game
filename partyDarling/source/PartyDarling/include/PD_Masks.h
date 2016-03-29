#pragma once

typedef enum{
	// 1-5 are reserved by bullet for stuff, 6-15 are available
	kPLAYER = (1<<6),
	kPD_ITEM = (1<<7),
	kPD_PROP = (1<<8),
	kPD_CHARACTER = (1<<9),
	kENVIRONMENT = (1<<10)
} PD_Masks;

#define kPD_INTERACTIVE (kPD_ITEM | kPD_CHARACTER | kPD_PROP)