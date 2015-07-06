#pragma once

#include <PD_ResourceManager.h>

#include <NumberUtils.h>
#include <Easing.h>

Texture * PD_ResourceManager::crosshair = new Texture("../assets/crosshair.png", 16, 16, true, false);
Texture * PD_ResourceManager::cursor = new Texture("../assets/engine basics/cursor.png", 32, 32, true, false);
Texture * PD_ResourceManager::cheryl = new Texture("../assets/img_cheryl.jpg", 256, 256, true, false);
Texture * PD_ResourceManager::uvs = new Texture("../assets/uvs.png", 2048, 2048, true, false);
Texture * PD_ResourceManager::uvs_alt = new Texture("../assets/uvs_alt.png", 2048, 2048, true, false);
OpenAL_Sound * PD_ResourceManager::scene = new OpenAL_SoundSimple("../assets/audio/tone3.ogg", false, false);
OpenAL_Sound * PD_ResourceManager::stream = new OpenAL_SoundStream("../assets/audio/Nu-.ogg", false, false);
//OpenAL_Sound * PD_ResourceManager::stream = new OpenAL_SoundStreamGenerative(false, false);

void PD_ResourceManager::init(){
	resources.push_back(crosshair);
	resources.push_back(cursor);
	resources.push_back(cheryl);
	resources.push_back(uvs);
	resources.push_back(uvs_alt);
	resources.push_back(scene);
	resources.push_back(stream);
	
	/*stream->source->buffer->sampleRate = 8000;
	stream->source->buffer->format = AL_FORMAT_STEREO8;
	dynamic_cast<OpenAL_SoundStreamGenerative *>(stream)->generativeFunction = [](unsigned long int t){
		// a oneliner from http://countercomplex.blogspot.ca/2011/10/algorithmic-symphonies-from-one-line-of.html
		// sounds pretty much the same as in the video, so I think the system works
		//return (t>>6|t|t>>(t>>16))*10+((t>>11)&7); 
		
		/////////////////
		// experiments //
		/////////////////
		// kind of sounds like record scratching?
		//return OpenAL_SoundStreamGenerative::compressFloat(Easing::easeInOutElastic(t % 3500, 0, 0.8f, 3500) + Easing::easeInOutElastic(t % 7500, 0, 0.8f, 7500), Easing::easeInOutElastic(t % 2000, 0, 0.8f, 2000));
	
		// loud droning and quiet sliding
		//return t*t&t<<2|t>>(8+t^3);

		// this one's REALLY annoying
		//return t/9+t>>9*t|9^t&13-t<<3;

		// give it a couple seconds to warm up
		return (t|t*t>>8)*sqrt(t>>10);
	};*/
}