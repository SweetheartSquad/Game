#pragma once

#include <Room.h>
class Scenario;
class LabRoom : public Room{
public:
	LabRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, Scenario * _labScenario);
};