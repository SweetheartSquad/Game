#pragma once

#include <Room.h>

class IntroRoom : public Room{
public:
	IntroRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, AssetRoom * const _definition);
};