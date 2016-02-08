#pragma once

#include<PD_PhraseGenerator_Insults.h>

PD_PhraseGenerator_Insults::PD_PhraseGenerator_Insults() :
	playerBlank("____")
{
	makeDatabases("assets/wordlists/insults.json");
}

void PD_PhraseGenerator_Insults::makeInsults() {
	std::string eInsult = replaceWords("*phrases*");
	std::string pInsult = replaceWords("*playerPhrases*");
	std::string pGood   = replaceWords("*goodAdj*");
	std::string pBad    = replaceWords("*badAdj*");

	enemyInsult      = eInsult;
	playerInsult     = pInsult;
	playerGoodChoice = pGood;
	playerBadChoice  = pBad;
}