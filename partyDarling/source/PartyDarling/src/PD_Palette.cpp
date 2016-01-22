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

	Colour root = Colour::getRandomInRgbRange();

	Colour colours[kSIZE];
	colours[kHAT_LIGHT     ] = Colour::getRandomInRgbRange();

	colours[kHAT_DARK      ] = colours[kHAT_LIGHT].darker();
	colours[kNOSE_LIGHT    ] = root;
	colours[kNOSE_DARK     ] = colours[kNOSE_LIGHT].darker();
	colours[kEYEBROWS_LIGHT] = root;
	colours[kEYEBROWS_DARK ] = colours[kEYEBROWS_LIGHT].darker();
	colours[kEYES_LIGHT    ] = root;
	colours[kEYES_DARK     ] = colours[kEYES_LIGHT].darker();
	colours[kPUPILS_LIGHT  ] = root;
	colours[kPUPILS_DARK   ] = colours[kPUPILS_LIGHT].darker();
	colours[kSKIN_LIGHT    ] = root;
	colours[kSKIN_DARK     ] = colours[kSKIN_LIGHT].darker();

	Colour shirtRoot = root.hsvMod(glm::uvec3(0));
	Colour pantsRoot = root.hsvMod(glm::uvec3(0));
	Colour shoesRoot = root.hsvMod(glm::uvec3(0));
	

	colours[kSHIRT1_LIGHT  ] = shirtRoot;
	colours[kSHIRT1_DARK   ] = colours[kSHIRT1_LIGHT].darker();
	colours[kSHIRT2_LIGHT  ] = shirtRoot.hsvMod(glm::uvec3(sweet::NumberUtils::randomInt(120, 240), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kSHIRT2_DARK   ] = colours[kSHIRT2_LIGHT].darker();
	colours[kSHIRT3_LIGHT  ] = shirtRoot.hsvMod(glm::uvec3(sweet::NumberUtils::randomInt(-60, 60), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kSHIRT3_DARK   ] = colours[kSHIRT3_LIGHT].darker();
	colours[kPANTS1_LIGHT  ] = pantsRoot;
	colours[kPANTS1_DARK   ] = colours[kPANTS1_LIGHT].darker();
	colours[kPANTS2_LIGHT  ] = pantsRoot.hsvMod(glm::uvec3(sweet::NumberUtils::randomInt(120, 240), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kPANTS2_DARK   ] = colours[kPANTS2_LIGHT].darker();
	colours[kPANTS3_LIGHT  ] = pantsRoot.hsvMod(glm::uvec3(sweet::NumberUtils::randomInt(-60, 60), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kPANTS3_DARK   ] = colours[kPANTS3_LIGHT].darker();
	colours[kSHOES1_LIGHT  ] = shoesRoot;
	colours[kSHOES1_DARK   ] = colours[kSHOES1_LIGHT].darker();
	colours[kSHOES2_LIGHT  ] = shoesRoot.hsvMod(glm::uvec3(sweet::NumberUtils::randomInt(120, 240), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));;
	colours[kSHOES2_DARK   ] = colours[kSHOES2_LIGHT].darker();
	colours[kSHOES3_LIGHT  ] = shoesRoot.hsvMod(glm::uvec3(sweet::NumberUtils::randomInt(-60, 60), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));;
	colours[kSHOES3_DARK   ] = colours[kSHOES3_LIGHT].darker();
	colours[kTRANSPARENT   ] = Colour(255,	255,	255);
	colours[kBLACK         ] = Colour(0,	0,	0);

	// transfer the generated colours to the texture
	for(unsigned long int i = 0; i < kSIZE; ++i){
		sweet::TextureUtils::setPixel(this, i, 0, glm::uvec4(colours[i].r, colours[i].g, colours[i].b, 255));
	}

	// make sure that the alpha is handled properly
	sweet::TextureUtils::setPixel(this, kTRANSPARENT, 0, glm::uvec4(0,0,0,0));
}