#pragma once

#include <node\NodeChild.h>
#include <node\NodeUpdatable.h>
#include <AL\alure.h>
#include <iostream>

//#define NUM_BUFS 32

class OpenALTest_StreamPlayer : public virtual NodeUpdatable, public virtual NodeChild{
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

	OpenALTest_StreamPlayer(const char * _filename);
	~OpenALTest_StreamPlayer();

	void update(Step * _step) override;
};