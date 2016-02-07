#pragma once

#include<PD_InsultGenerator.h>

PD_InsultGenerator::PD_InsultGenerator() :
	playerBlank("____")
{
	makeDatabases("assets/insults/wordlist.json");
}

void PD_InsultGenerator::makeInsults() {
	std::string eInsult = replaceWords("*phrases*");
	std::string pInsult = replaceWords("*playerPhrases*");
	std::string pGood   = replaceWords("*goodAdj*");
	std::string pBad    = replaceWords("*badAdj*");

	enemyInsult      = eInsult;
	playerInsult     = pInsult;
	playerGoodChoice = pGood;
	playerBadChoice  = pBad;
}