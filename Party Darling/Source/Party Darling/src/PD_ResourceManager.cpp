#pragma once

#include <PD_ResourceManager.h>

Texture * PD_ResourceManager::crosshair = new Texture("../assets/crosshair.png", 16, 16, true, false);

void PD_ResourceManager::init(){
	resources.push_back(crosshair);
}