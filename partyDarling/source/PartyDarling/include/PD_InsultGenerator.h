#pragma once

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class PD_InsultGenerator {
public:
	PD_InsultGenerator();
	void makeInsults();

	std::string enemyInsult;
	std::string playerInsult;
	std::string playerGoodChoice;
	std::string playerBadChoice;

private:
	std::map<std::string, std::vector<std::string>> terms;
	std::string escapeChar;
	std::string replaceWords(std::string phrase);
	//std::string fixVowels(std::string phrase);
	
	void makeDatabases();
};
