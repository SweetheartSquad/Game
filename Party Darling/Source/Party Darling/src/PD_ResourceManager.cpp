#pragma once

#include <PD_ResourceManager.h>

Scenario * PD_ResourceManager::scenario = nullptr;
DatabaseConnection * PD_ResourceManager::db = nullptr;

void PD_ResourceManager::init(){
	scenario = new Scenario("assets/scenario.json");
	db = new DatabaseConnection("data/test.db");

	resources.push_back(scenario);
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