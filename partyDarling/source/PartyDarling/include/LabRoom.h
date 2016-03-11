#pragma once

#include <Room.h>
class Scenario;
class PointLight;
class LabRoom : public Room{
public:
	std::vector<Transform *> toRotate;
	PointLight * topLight, * bottomLight;

	LabRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, Scenario * _labScenario);

	virtual void update(Step * _step) override;
};