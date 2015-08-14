#pragma once

#include <Texture.h>
#include <MeshInterface.h>


class MarchingSquare{
public:
	unsigned long int x, y, size, p0, p1, p2, p3, code;
};

class PD_TilemapGenerator : public Texture{
public:

	PD_TilemapGenerator(unsigned long int _width, unsigned long int _height, bool _autorelease);

	virtual void loadImageData() override;

	MeshInterface * march(unsigned long int _thresh, bool _smooth);
private:
	unsigned long int thresh;
};