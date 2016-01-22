#pragma once

#include <TextureColourTable.h>
#include <Colour.h>

typedef enum Palette_t{
	kHAT_DARK        = 0,
	kHAT_LIGHT       = 1,
	kNOSE_DARK       = 2,
	kNOSE_LIGHT      = 3,
	kEYEBROWS_DARK   = 4,
	kEYEBROWS_LIGHT  = 5,
	kEYES_DARK       = 6,
	kEYES_LIGHT      = 7,
	kPUPILS_DARK     = 8,
	kPUPILS_LIGHT    = 9,
	kSKIN_DARK       = 10,
	kSKIN_LIGHT      = 11,
	kSHIRT1_DARK     = 12,
	kSHIRT1_LIGHT    = 13,
	kSHIRT2_DARK     = 14,
	kSHIRT2_LIGHT    = 15,
	kSHIRT3_DARK     = 16,
	kSHIRT3_LIGHT    = 17,
	kPANTS1_DARK     = 18,
	kPANTS1_LIGHT    = 19,
	kPANTS2_DARK     = 20,
	kPANTS2_LIGHT    = 21,
	kPANTS3_DARK     = 22,
	kPANTS3_LIGHT    = 23,
	kSHOES1_DARK     = 24,
	kSHOES1_LIGHT    = 25,
	kSHOES2_DARK     = 26,
	kSHOES2_LIGHT    = 27,
	kSHOES3_DARK     = 28,
	kSHOES3_LIGHT    = 29,
	kBLACK           = 30,
	kTRANSPARENT     = 31,
	kSIZE
};

class PD_Palette : public TextureColourTable{
public:
	PD_Palette(bool _autorelease);
	~PD_Palette();

	virtual void generateRandomTable() override;
};