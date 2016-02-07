#pragma once

#include <PhraseGenerator.h>

class PD_InsultGenerator : public PhraseGenerator {
public:
	PD_InsultGenerator();
	void makeInsults();

	std::string enemyInsult;
	std::string playerInsult;
	std::string playerGoodChoice;
	std::string playerBadChoice;

	std::string playerBlank;
};
