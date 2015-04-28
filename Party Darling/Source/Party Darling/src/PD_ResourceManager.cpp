#pragma once

#include <PD_ResourceManager.h>

Texture * PD_ResourceManager::crosshair = new Texture("../assets/crosshair.png", 16, 16, true, false);
Texture * PD_ResourceManager::cursor = new Texture("../assets/cursor.png", 512, 512, true, false);
Texture * PD_ResourceManager::cheryl = new Texture("../assets/img_cheryl.jpg", 256, 256, true, false);
Texture * PD_ResourceManager::uvs = new Texture("../assets/uvs.png", 2048, 2048, true, false);

void PD_ResourceManager::init(){
	resources.push_back(crosshair);
	resources.push_back(cursor);
	resources.push_back(cheryl);
	resources.push_back(uvs);
}