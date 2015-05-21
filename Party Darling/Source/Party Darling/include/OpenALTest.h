#pragma once

#include <node\NodeChild.h>
#include <node\NodeUpdatable.h>
#include <AL\alure.h>
#include <iostream>

//#define NUM_BUFS 32

class OpenAL_Sound : public virtual NodeUpdatable, public virtual NodeChild{
private:
	static bool inited;

	static void initOpenAL();
	static void uninitOpenAL();
public:
    /*static ALCcontext * context;
    static ALCdevice * device;*/

	ALuint sourceId;
	ALuint bufferId;
	alureStream * stream;
	bool loop;

	OpenAL_Sound(const char * _filename, bool _loop);
	~OpenAL_Sound();

	virtual void update(Step * _step) override;
	
	void setPosition(glm::vec3 _pos);
	void setDirection(glm::vec3 _forward, glm::vec3 _up);

	static void setListenerPos(glm::vec3 _pos);
	static void setListenerOrientation(glm::vec3 _forward, glm::vec3 _up);
};
class OpenAL_Stream : public OpenAL_Sound{
public:
	OpenAL_Stream(const char * _filename);
	~OpenAL_Stream();
	void update(Step * _step) override;
};