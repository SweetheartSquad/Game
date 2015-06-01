#pragma once

#include <PD_ResourceManager.h>

Texture * PD_ResourceManager::crosshair = new Texture("../assets/crosshair.png", 16, 16, true, false);
Texture * PD_ResourceManager::cursor = new Texture("../assets/cursor.png", 32, 32, true, false);
Texture * PD_ResourceManager::cheryl = new Texture("../assets/img_cheryl.jpg", 256, 256, true, false);
Texture * PD_ResourceManager::uvs = new Texture("../assets/uvs.png", 2048, 2048, true, false);
OpenAL_Sound * PD_ResourceManager::scene = new OpenAL_Sound("../assets/audio/tone3.ogg", true);
OpenAL_Stream * PD_ResourceManager::stream = new OpenAL_Stream("../assets/audio/tone2.ogg", false);

void PD_ResourceManager::init(){
	resources.push_back(crosshair);
	resources.push_back(cursor);
	resources.push_back(cheryl);
	resources.push_back(uvs);
	resources.push_back(scene);
	resources.push_back(stream);
}