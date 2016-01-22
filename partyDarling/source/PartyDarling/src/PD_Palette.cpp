#pragma once

#include <PD_Palette.h>
#include <TextureUtils.h>
#include <NumberUtils.h>

PD_Palette::PD_Palette(bool _autorelease) :
	TextureColourTable(_autorelease),
	NodeResource(_autorelease)
{
	// initialize everything to hot pink so that we can easily see if we missed something later
	for(unsigned long int i = 0; i < numBytes; i += 4){
		data[i+0] = 255;
		data[i+1] = 0;
		data[i+2] = 255;
		data[i+3] = 255;
	}
}

PD_Palette::~PD_Palette(){

}

void PD_Palette::generateRandomTable(){
	// TODO: put some RNG and actual logic into this
	glm::uvec3 colours[kSIZE];
	colours[kHAT_DARK      ] = glm::uvec3(0,	255,	255);
	colours[kHAT_LIGHT     ] = glm::uvec3(0,	255,	0);
	colours[kNOSE_DARK     ] = glm::uvec3(255,	255,	255);
	colours[kNOSE_LIGHT    ] = glm::uvec3(255,	255,	255);
	colours[kEYEBROWS_DARK ] = glm::uvec3(255,	255,	255);
	colours[kEYEBROWS_LIGHT] = glm::uvec3(255,	255,	255);
	colours[kEYES_DARK     ] = glm::uvec3(255,	255,	255);
	colours[kEYES_LIGHT    ] = glm::uvec3(0,	0,	255);
	colours[kPUPILS_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kPUPILS_LIGHT  ] = glm::uvec3(255,	0,	0);
	colours[kSKIN_DARK     ] = glm::uvec3(255,	255,	255);
	colours[kSKIN_LIGHT    ] = glm::uvec3(255,	255,	255);
	colours[kSHIRT1_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kSHIRT1_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kSHIRT2_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kSHIRT2_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kSHIRT3_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kSHIRT3_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kPANTS1_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kPANTS1_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kPANTS2_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kPANTS2_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kPANTS3_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kPANTS3_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kSHOES1_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kSHOES1_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kSHOES2_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kSHOES2_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kSHOES3_DARK   ] = glm::uvec3(255,	255,	255);
	colours[kSHOES3_LIGHT  ] = glm::uvec3(255,	255,	255);
	colours[kTRANSPARENT   ] = glm::uvec3(255,	255,	255);
	colours[kBLACK         ] = glm::uvec3(0,	0,	0);

	// transfer the generated colours to the texture
	for(unsigned long int i = 0; i < kSIZE; ++i){
		sweet::TextureUtils::setPixel(this, i, 0, glm::uvec4(colours[i].r, colours[i].g, colours[i].b, 255));
	}

	// make sure that the alpha is handled properly
	sweet::TextureUtils::setPixel(this, kTRANSPARENT, 0, glm::uvec4(0,0,0,0));
}