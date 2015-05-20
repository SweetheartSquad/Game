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
	ALuint bufferID;

	alGenBuffers(1, &bufferID);
	if ((error = alGetError()) != AL_NO_ERROR)
	std::cout << "alGenBuffers : " << error << std::endl;
	ALboolean alureStat = alureBufferDataFromFile(_filename, bufferID);
	if(alureStat == AL_FALSE){
		std::cout << "alureBufferDataFromFile: " << alureGetErrorString() << std::endl;
	}

	
	alGenSources(1, &src);
	if ((error = alGetError()) != AL_NO_ERROR)
	std::cout << "alGenSources : " << error << std::endl;
	// attach the buffer to the source
	alSourcei(src, AL_BUFFER, bufferID); 

	// turn off looping
	alSourcei(src, AL_LOOPING, AL_FALSE);
	// Start playing source
	alSourcePlay(src);
	if ((error = alGetError()) != AL_NO_ERROR)
	std::cout << "alSourcePlay source 0 : " << error << std::endl;
}

OpenALTest_StreamPlayer::~OpenALTest_StreamPlayer(){
  alureDestroyStream(stream, 0, buf);
}

void OpenALTest_StreamPlayer::update(Step * _step){
	//alureUpdate();
	//
	//ALint state = AL_PLAYING;
	//ALint processed = 0;    
	///*alureSleep(threadDelay);
	//if ( !src || src.state!=playing ){
	//	return;
	//}*/
	//alGetError();
	//alGetSourcei(src, AL_SOURCE_STATE, &state);
	//alGetSourcei(src, AL_BUFFERS_PROCESSED, &processed);
	////    printf( "Processed: %d\n", processed );
	////    printf( "I: %s", alureGetErrorString());
	//if(processed > 0){
	//	ALuint * bufs = new ALuint[NUM_BUFS];
	//	ALsizei filled;
	//	alSourceUnqueueBuffers(src, processed, bufs);
	//	filled = alureBufferDataFromStream(stream, processed, bufs);
	//	if(filled <= 0) {
	//		if(loop) {
	//			alureRewindStream(stream);
	//			filled = alureBufferDataFromStream(stream, processed, bufs);
	//			if(filled <= 0) { 
	//				printf( "ALUREStream:DelayExpiredinThread:Bad error when attempting to loop.\n" );
	//				throw;
	//				/*badError=true;
	//				((GuiApplication)__thisModule).Lock(); OnError(); ((GuiApplication)__thisModule).Unlock();
	//				delete bufs;
	//				return false;*/
	//			}
	//		} else if(state != AL_PLAYING) {
	//			//((GuiApplication)__thisModule).Lock(); OnComplete(); ((GuiApplication)__thisModule).Unlock();
	//			delete bufs;
	//			return;
	//		}
	//	}
	//	alSourceQueueBuffers(src, filled, bufs);
	//	delete bufs;
	//}
	//if(state != AL_PLAYING) {
	//	alSourcePlay(src);
	//}
	//((GuiApplication)__thisModule).Lock(); BetweenFrames(); ((GuiApplication)__thisModule).Unlock();



	/*ALint state = AL_PLAYING;
	ALint processed = 0;
	 
	alGetSourcei(src, AL_SOURCE_STATE, &state);
	alGetSourcei(src, AL_BUFFERS_PROCESSED, &processed);
	if(processed > 0){
	    ALuint bufs[NUM_BUFS];
	    alSourceUnqueueBuffers(src, processed, bufs);
	 
	    processed = alureBufferDataFromStream(stream, processed, bufs);
	    if(processed <= 0){
			std::cout << "done streaming" << std::endl;
	        return; // done streaming
		}
	 
	    alSourceQueueBuffers(src, processed, bufs);
	}
	if(state != AL_PLAYING){
		std::cout << "PLAY!" << std::endl;
	    alSourcePlay(src);
	}
	//std::cout << "woo" << std::endl;
	return;*/
}