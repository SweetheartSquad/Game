#pragma once

#include <PD_Item.h>


class PD_Door : public PD_Item{
public:
	PD_Door(BulletWorld * _world, Texture * _texture, Shader * _shader);
};