#pragma once

#include <PhraseGenerator.h>

class PD_PhraseGenerator_Incidental : public PhraseGenerator {
public:
	PD_PhraseGenerator_Incidental();
	std::string getLine();
};
