#pragma once

#include <Room.h>

class LabRoom : public Room{
public:
	LabRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, AssetRoom * const _definition);
};