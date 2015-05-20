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
	
	alGenSources(1, &src);
	stream = alureCreateStreamFromFile(_filename, 19200, NUM_BUFS, buf);
	if(stream == nullptr){
		std::cout << "Could not load file.ogg: " << alureGetErrorString() << std::endl;
		alDeleteSources(1, &src);
		throw;
	}
	ALboolean stat;
	stat = alurePlaySourceStream(src,stream,19200,0/*# of loops*/,nullptr,nullptr); // add eos_callback tie-in here
	if(stat == AL_FALSE){
		std::cout << "alure error: " << alureGetErrorString() << std::endl;
		throw;
	}
   

	// then queue up and start streaming..
	//alSourceQueueBuffers(src, NUM_BUFS, buf);
	//alSourcePlay(src);
	//int loopcount = -1;
	//ALboolean stat = alurePlaySource(src, nullptr, nullptr);
	/**/
}

OpenALTest_StreamPlayer::~OpenALTest_StreamPlayer(){
  alureDestroyStream(stream, 0, buf);
}

void OpenALTest_StreamPlayer::update(Step * _step){
	alureUpdate();

	ALint state = AL_PLAYING;
	ALint processed = 0;    
	/*alureSleep(threadDelay);
	if ( !src || src.state!=playing ){
		return;
	}*/
	alGetError();
	alGetSourcei(src, AL_SOURCE_STATE, &state);
	alGetSourcei(src, AL_BUFFERS_PROCESSED, &processed);
	//    printf( "Processed: %d\n", processed );
	//    printf( "I: %s", alureGetErrorString());
	if(processed > 0){
		ALuint * bufs = new ALuint[NUM_BUFS];
		ALsizei filled;
		alSourceUnqueueBuffers(src, processed, bufs);
		filled = alureBufferDataFromStream(stream, processed, bufs);
		if(filled <= 0) {
			if(loop) {
				alureRewindStream(stream);
				filled = alureBufferDataFromStream(stream, processed, bufs);
				if(filled <= 0) { 
					printf( "ALUREStream:DelayExpiredinThread:Bad error when attempting to loop.\n" );
					throw;
					/*badError=true;
					((GuiApplication)__thisModule).Lock(); OnError(); ((GuiApplication)__thisModule).Unlock();
					delete bufs;
					return false;*/
				}
			} else if(state != AL_PLAYING) {
				//((GuiApplication)__thisModule).Lock(); OnComplete(); ((GuiApplication)__thisModule).Unlock();
				delete bufs;
				return;
			}
		}
		alSourceQueueBuffers(src, filled, bufs);
		delete bufs;
	}
	if(state != AL_PLAYING) {
		alSourcePlay(src);
	}
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