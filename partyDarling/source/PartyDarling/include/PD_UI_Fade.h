#pragma once

#include <NodeUI.h>
#include <Timeout.h>

class PD_UI_Fade : public NodeUI{
public:
	Timeout * fadeInTimeout;
	Timeout * fadeOutTimeout;
	glm::vec3 color;

	PD_UI_Fade(BulletWorld * _world);
	~PD_UI_Fade();

	void fadeIn(glm::uvec3 _color = glm::uvec3(0), float _length = 1.f);
	void fadeOut(glm::uvec3 _color = glm::uvec3(0), float _length = 1.f);
};