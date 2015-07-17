#pragma once

#include <PD_ResourceManager.h>

#include <NumberUtils.h>
#include <Easing.h>

Texture * PD_ResourceManager::crosshair = new Texture("../assets/crosshair.png", 16, 16, true, false);
Texture * PD_ResourceManager::cursor = new Texture("../assets/engine basics/cursor.png", 32, 32, true, false);
Texture * PD_ResourceManager::cheryl = new Texture("../assets/engine basics/img_cheryl.jpg", 256, 256, true, false);
Texture * PD_ResourceManager::uvs = new Texture("../assets/uvs.png", 2048, 2048, true, false);
Texture * PD_ResourceManager::uvs_alt = new Texture("../assets/uvs_alt.png", 2048, 2048, true, false);
OpenAL_Sound * PD_ResourceManager::scene = new OpenAL_SoundSimple("../assets/audio/tone3.ogg", false, false);
OpenAL_Sound * PD_ResourceManager::stream = new OpenAL_SoundStream("../assets/audio/Nu-.ogg", false, false);
//OpenAL_Sound * PD_ResourceManager::stream = new OpenAL_SoundStreamGenerative(false, false);
DatabaseConnection * PD_ResourceManager::db = nullptr;

void PD_ResourceManager::init(){
	resources.push_back(crosshair);
	resources.push_back(cursor);
	resources.push_back(cheryl);
	resources.push_back(uvs);
	resources.push_back(uvs_alt);
	resources.push_back(scene);
	resources.push_back(stream);
	
	db = new DatabaseConnection("data/test.db");
}

int PD_ResourceManager::dbCallback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

void PD_ResourceManager::testSql(std::string _sql, bool _async){
	if(_async){
		db->queryDbAsync(_sql, dbCallback);
	}else{
		db->queryDb(_sql, dbCallback);
	}
}