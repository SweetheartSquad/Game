#pragma once

#include <PhraseGenerator.h>

class PD_PhraseGenerator_Insults : public PhraseGenerator {
public:
	PD_PhraseGenerator_Insults();
	void makeInsults();

	std::string enemyInsult;
	std::string playerInsult;
	std::string playerGoodChoice;
	std::string playerBadChoice;

	std::string playerBlank;
};
