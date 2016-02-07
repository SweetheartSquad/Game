#pragma once

#include<PD_PhraseGenerator_Incidental.h>

PD_PhraseGenerator_Incidental::PD_PhraseGenerator_Incidental()
{
	makeDatabases("assets/wordlists/incidental.json");
}

std::string PD_PhraseGenerator_Incidental::getLine(){
	return replaceWords("*phrases*");
}