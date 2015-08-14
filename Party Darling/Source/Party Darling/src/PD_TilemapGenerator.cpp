#pragma once

#include <PD_TilemapGenerator.h>
#include <NumberUtils.h>
#include <glm\glm.hpp>
#include <TextureUtils.h>

PD_TilemapGenerator::PD_TilemapGenerator(unsigned long int _width, unsigned long int _height, bool _autorelease) :
	Texture(true, _autorelease),
	NodeResource(_autorelease)
{
	channels = 4;
	width = _width;
	height = _height;

	numPixels = width * height;
	numBytes = numPixels * channels;

	configure();
}


void PD_TilemapGenerator::configure(unsigned long int _max, unsigned long int _pixelIncrement){
	max = _max;
	pixelIncrement = _pixelIncrement;
}
void PD_TilemapGenerator::loadImageData(){
	// allocate and initialize texture data
	data = (unsigned char *)calloc(numBytes, sizeof(unsigned char));
	for(unsigned long int y = 0; y < height; ++y){
	for(unsigned long int x = 0; x < width; ++x){
		vox::TextureUtils::getPixel(this, x, y, 1) = 255;
		vox::TextureUtils::getPixel(this, x, y, 2) = 255;
		vox::TextureUtils::getPixel(this, x, y, 3) = 255;
	}
	}
	
	// center is always fully on
	std::vector<glm::ivec2> points;
	points.push_back(glm::ivec2(width/2, height/2));
	vox::TextureUtils::getPixel(this, points.front().x, points.front().y, 0) = 255;
	vox::TextureUtils::getPixel(this, points.front().x, points.front().y, 1) = 0;
	vox::TextureUtils::getPixel(this, points.front().x, points.front().y, 2) = 0;
	
	bool done = false;
	float prob = 1;
	unsigned long int maxAttempts = numPixels;

	
	float probDecrement = (float)pixelIncrement/(numPixels * max);
	while(!done){
		bool valid = true;
		glm::ivec2 newPos;
		unsigned long int attempts = 0;
		do{
			valid = true;
			glm::ivec2 pos = points.at(vox::NumberUtils::randomInt(0, points.size()-1));
		
			glm::ivec2 dir(vox::NumberUtils::randomInt(-1,1), vox::NumberUtils::randomInt(-1,1));
			
			newPos = pos+dir;

			// make sure the new point is within the image (an additional 1-pixel border is also ignored to let marching squares work properly)
			if(newPos.x <= 0){
				valid = false; continue;
			}if(newPos.x >= width-1){
				valid = false; continue;
			}if(newPos.y <= 0){
				valid = false; continue;
			}if(newPos.y >= height-1){
				valid = false; continue;
			}

			/*for(unsigned long int i = 0; i < points.size(); ++i){
				if(points.at(i) == newPos){
					valid = false; break;
				}
			}*/
			if(vox::TextureUtils::getPixel(this, newPos.x, newPos.y) >= max){
				valid = false;
			}

		}while(!valid && ++attempts < maxAttempts);
		
		if(!valid){
			break;
		}
		
		if(vox::TextureUtils::getPixel(this, newPos.x, newPos.y, 0) < pixelIncrement){
			points.push_back(newPos);
		}
		
		vox::TextureUtils::getPixel(this, newPos.x, newPos.y, 0) += pixelIncrement;
		vox::TextureUtils::getPixel(this, newPos.x, newPos.y, 1) -= pixelIncrement;
		vox::TextureUtils::getPixel(this, newPos.x, newPos.y, 2) -= pixelIncrement;

		if(prob < 0){
			done = true;
		}else{
			prob -= probDecrement;
		}
	}
	

}

MeshInterface * PD_TilemapGenerator::march(unsigned long int _thresh, bool _smooth){
	std::vector<glm::vec2> verts = vox::TextureUtils::getMarchingSquaresContour(this, _thresh, _smooth);
	MeshInterface * res = new MeshInterface(GL_LINES, GL_STATIC_DRAW);
	
	for(auto v : verts){
		res->pushVert(Vertex(v.x, v.y, 0));
	}
	return res;
}