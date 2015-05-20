#pragma once

#include <OpenALTest.h>

/*ALCcontext * OpenALTest_StreamPlayer::context = nullptr; 
ALCdevice * OpenALTest_StreamPlayer::device = nullptr; */

bool OpenALTest_StreamPlayer::inited = false;

void OpenALTest_StreamPlayer::initOpenAL(){
	// initialize OpenAL
	if(!inited){
		alureInitDevice(nullptr, nullptr);
		inited = true;
	}
	/*if(context == nullptr){
		device = alcOpenDevice(nullptr); 
		if(device == nullptr){
			std::cout<<"error!"<<std::endl;
			alcCloseDevice(device);
			return;
		}
		context = alcCreateContext(device, nullptr); 
		alcMakeContextCurrent(context);

		alGetError();
	}*/

}

void OpenALTest_StreamPlayer::uninitOpenAL(){
	if(inited){
		alureShutdownDevice();
		inited = false;
	}
}

OpenALTest_StreamPlayer::OpenALTest_StreamPlayer(const char * _filename) :
	loop(false)
{
	initOpenAL();
	ALenum error;

	alGenBuffers(1, &bufferId);
	if ((error = alGetError()) != AL_NO_ERROR)
	std::cout << "alGenBuffers : " << error << std::endl;
	ALboolean alureStat = alureBufferDataFromFile(_filename, bufferId);
	if(alureStat == AL_FALSE){
		std::cout << "alureBufferDataFromFile: " << alureGetErrorString() << std::endl;
	}

	
	// generate source
	alGenSources(1, &sourceId);
	if ((error = alGetError()) != AL_NO_ERROR)
	std::cout << "alGenSources : " << error << std::endl;
	// attach the buffer to the source
	alSourcei(sourceId, AL_BUFFER, bufferId); 
	// turn off looping
	alSourcei(sourceId, AL_LOOPING, AL_FALSE);

	// Start playing source
	alSourcePlay(sourceId);
	if ((error = alGetError()) != AL_NO_ERROR)
	std::cout << "alSourcePlay source 0 : " << error << std::endl;
}

OpenALTest_StreamPlayer::~OpenALTest_StreamPlayer(){
  alureDestroyStream(stream, 0, &bufferId);
}

void OpenALTest_StreamPlayer::update(Step * _step){
}