#pragma once

#include <PhraseGenerator.h>
class PD_Character;
class PD_PhraseGenerator_Incidental : public PhraseGenerator {
public:
	PD_PhraseGenerator_Incidental();
	void updateNames(PD_Character * _character);
	std::string getLineNormal();
	std::string getLineLost();
	std::string getLineWon();
};
