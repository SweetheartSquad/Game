#pragma once

#include <ResourceManager.h>

#include <scenario\Scenario.h>
#include <DatabaseConnection.h>
#include <PD_FurnitureDefinition.h>
#include <PD_FurnitureParser.h>

class PD_ResourceManager : public ResourceManager{
public:
	static Scenario * scenario;
	static Scenario * itemTextures;
	static Scenario * componentTextures;
	static DatabaseConnection * db;
	static std::vector<PD_FurnitureDefinition*> * furnitureDefinitions;
	static PD_FurnitureComponentContainer * furnitureComponents;

	static void init();

	static int dbCallback(void *NotUsed, int argc, char **argv, char **azColName);
	static void testSql(std::string _sql, bool _async);
};