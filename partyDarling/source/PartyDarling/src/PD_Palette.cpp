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

	Colour skinRoot = Colour::getRandomFromHsvRange(glm::ivec3(0, 80, 25), glm::ivec3(360, 100, 100));
	glm::ivec3 temp = Colour::rgbToHsv(skinRoot);
	if(temp.y + temp.z > 150){
		if(sweet::NumberUtils::randomBool()){
			temp.y /= 2;
		}else if(sweet::NumberUtils::randomBool()){
			temp.z /= 2;
		}else{
			temp.y /= 2;
			temp.z /= 2;
		}
	}
	skinRoot = Colour::hsvToRgb(temp);

	Colour shirtRoot = skinRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(0, 360), sweet::NumberUtils::randomInt(-25,50), sweet::NumberUtils::randomInt(-25,50)));
	Colour pantsRoot = skinRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(0, 360), sweet::NumberUtils::randomInt(-5,10), sweet::NumberUtils::randomInt(-5,10)));
	Colour shoesRoot = skinRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(0, 360), sweet::NumberUtils::randomInt(-5,10), sweet::NumberUtils::randomInt(-5,10)));
	Colour hairRoot = skinRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(0, 360), sweet::NumberUtils::randomInt(-5,10), sweet::NumberUtils::randomInt(-5,10)));

	Colour colours[kSIZE];
	// main
	colours[kHAIR_FILL        ] = hairRoot;
	colours[kNOSE_FILL        ] = skinRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(-60, 60), sweet::NumberUtils::randomInt(-50,50), sweet::NumberUtils::randomInt(-50,50)));
	colours[kEYEBROWS_FILL    ] = hairRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kEYES_FILL        ] = sweet::NumberUtils::randomFloat() > 0.9f ? Colour::getRandomFromHsvRange(glm::ivec3(0, 0, 90), glm::ivec3(360, 100, 100)) : Colour::getRandomFromHsvRange(glm::ivec3(0, 0, 90), glm::ivec3(360, 10, 100));
	colours[kPUPILS_FILL      ] = shirtRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(-10, 10), sweet::NumberUtils::randomInt(-50,50), sweet::NumberUtils::randomInt(-50,50)));
	colours[kSKIN_FILL        ] = skinRoot;
	
	// clothes
	colours[kSHIRT1_FILL      ] = shirtRoot;
	colours[kSHIRT2_FILL      ] = shirtRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(160, 200), sweet::NumberUtils::randomInt(-50,50), sweet::NumberUtils::randomInt(-50,50)));
	colours[kSHIRT3_FILL      ] = shirtRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(-60, 60), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kPANTS1_FILL      ] = pantsRoot;
	colours[kPANTS2_FILL      ] = pantsRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(120, 240), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kPANTS3_FILL      ] = pantsRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(-60, 60), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kSHOES1_FILL      ] = shoesRoot;
	colours[kSHOES2_FILL      ] = shoesRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(120, 240), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	colours[kSHOES3_FILL      ] = shoesRoot.hsvMod(glm::ivec3(sweet::NumberUtils::randomInt(-60, 60), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10)));
	
	// outlines
	glm::ivec3 outlineMod(sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(-10,10), sweet::NumberUtils::randomInt(10,20));
	colours[kHAIR_OUTLINE     ] = generateOutline(colours[kHAIR_FILL], outlineMod);
	colours[kNOSE_OUTLINE     ] = generateOutline(colours[kNOSE_FILL], outlineMod);
	colours[kEYEBROWS_OUTLINE ] = generateOutline(colours[kEYEBROWS_FILL], outlineMod);
	colours[kEYES_OUTLINE     ] = generateOutline(colours[kEYES_FILL], outlineMod);
	colours[kPUPILS_OUTLINE   ] = generateOutline(colours[kPUPILS_FILL], outlineMod);
	colours[kSKIN_OUTLINE     ] = generateOutline(colours[kSKIN_FILL], outlineMod);
	colours[kSHIRT1_OUTLINE   ] = generateOutline(colours[kSHIRT1_FILL], outlineMod);
	colours[kSHIRT2_OUTLINE   ] = generateOutline(colours[kSHIRT2_FILL], outlineMod);
	colours[kSHIRT3_OUTLINE   ] = generateOutline(colours[kSHIRT3_FILL], outlineMod);
	colours[kPANTS1_OUTLINE   ] = generateOutline(colours[kPANTS1_FILL], outlineMod);
	colours[kPANTS2_OUTLINE   ] = generateOutline(colours[kPANTS2_FILL], outlineMod);
	colours[kPANTS3_OUTLINE   ] = generateOutline(colours[kPANTS3_FILL], outlineMod);
	colours[kSHOES1_OUTLINE   ] = generateOutline(colours[kSHOES1_FILL], outlineMod);
	colours[kSHOES2_OUTLINE   ] = generateOutline(colours[kSHOES2_FILL], outlineMod);
	colours[kSHOES3_OUTLINE   ] = generateOutline(colours[kSHOES3_FILL], outlineMod);

	colours[kTRANSPARENT      ] = Colour(255,	255,	255);
	colours[kBLACK            ] = Colour(0,	0,	0);

	// transfer the generated colours to the texture
	for(unsigned long int i = 0; i < kSIZE; ++i){
		sweet::TextureUtils::setPixel(this, i, 0, glm::uvec4(colours[i].r, colours[i].g, colours[i].b, 255));
	}

	// make sure that the alpha is handled properly
	sweet::TextureUtils::setPixel(this, kTRANSPARENT, 0, glm::uvec4(0,0,0,0));
}

Colour PD_Palette::generateOutline(Colour _fill, glm::ivec3 _outlineMod){
	return _fill.hsvMod(_outlineMod * ((Colour::rgbToHsv(_fill).z > 50) ? -2 : 1));
}