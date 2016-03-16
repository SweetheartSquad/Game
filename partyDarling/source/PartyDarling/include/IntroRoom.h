#pragma once

#include <Room.h>
class Scenario;
class IntroRoom : public Room{
public:
	IntroRoom(BulletWorld * _world, Shader * _toonShader, Shader * _characterShader, Shader * _emoteShader, Scenario * _introScenario);
	~IntroRoom();

	Transform * visibleMesh;
	TriMesh * colliderMesh;

	// rotates the room such that the exterior of the house faces _edge
	void setEdge(PD_Door::Door_t _edge);
};