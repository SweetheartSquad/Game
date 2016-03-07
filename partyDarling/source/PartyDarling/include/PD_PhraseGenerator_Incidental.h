#pragma once

#include <PhraseGenerator.h>
class PD_Character;
class PD_PhraseGenerator_Incidental : public PhraseGenerator {
public:
	PD_PhraseGenerator_Incidental();
	std::string getLineNormal(PD_Character * _character);
	std::string getLineLost();
	std::string getLineWon();
};
