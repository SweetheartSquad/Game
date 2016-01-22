#pragma once

#include <TextureColourTable.h>
#include <Colour.h>

typedef enum Palette_t{
	kHAIR_OUTLINE       = 0,
	kHAIR_FILL      = 1,
	kNOSE_OUTLINE       = 2,
	kNOSE_FILL      = 3,
	kEYEBROWS_OUTLINE   = 4,
	kEYEBROWS_FILL  = 5,
	kEYES_OUTLINE       = 6,
	kEYES_FILL      = 7,
	kPUPILS_OUTLINE     = 8,
	kPUPILS_FILL    = 9,
	kSKIN_OUTLINE       = 10,
	kSKIN_FILL      = 11,
	kSHIRT1_OUTLINE     = 12,
	kSHIRT1_FILL    = 13,
	kSHIRT2_OUTLINE     = 14,
	kSHIRT2_FILL    = 15,
	kSHIRT3_OUTLINE     = 16,
	kSHIRT3_FILL    = 17,
	kPANTS1_OUTLINE     = 18,
	kPANTS1_FILL    = 19,
	kPANTS2_OUTLINE     = 20,
	kPANTS2_FILL    = 21,
	kPANTS3_OUTLINE     = 22,
	kPANTS3_FILL    = 23,
	kSHOES1_OUTLINE     = 24,
	kSHOES1_FILL    = 25,
	kSHOES2_OUTLINE     = 26,
	kSHOES2_FILL    = 27,
	kSHOES3_OUTLINE     = 28,
	kSHOES3_FILL    = 29,
	kBLACK           = 30,
	kTRANSPARENT     = 31,
	kSIZE
};

class PD_Palette : public TextureColourTable{
public:
	PD_Palette(bool _autorelease);
	~PD_Palette();

	virtual void generateRandomTable() override;

	Colour generateOutline(Colour _fill, glm::ivec3 _outlineMod);
};