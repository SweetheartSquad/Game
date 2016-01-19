#pragma once

#include<PD_InsultGenerator.h>
#include <json/json.h>
#include <FileUtils.h>
#include <Log.h>
#include <regex>
#include <NumberUtils.h>

PD_InsultGenerator::PD_InsultGenerator() :
	escapeChar("*")
{
	makeDatabases();
}

void PD_InsultGenerator::makeDatabases() {
	Json::Value root;
	Json::Reader reader;

	std::string jsonLoaded = sweet::FileUtils::readFile("assets/insults/wordlist.json");
	bool parsingSuccessful = reader.parse(jsonLoaded, root);

	//error check
	if (!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages());
	}
	else {
		Json::Value::Members termLists = root.getMemberNames();
		for (auto memberName : termLists){
			std::vector<std::string> termList;
			for (Json::Value::ArrayIndex i = 0; i < root[memberName].size(); ++i) {
				std::string p = root[memberName].get(i, "plplp").asString();
				termList.push_back(p);
			}
			terms[memberName] = termList;
		}
	}
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


std::string PD_InsultGenerator::replaceWords(std::string phrase) {
	while (phrase.find(escapeChar) != std::string::npos) {
		std::regex r("\\*(.+?)\\*");
		std::smatch match;
		const std::string constPhrase = phrase;

		std::string word;
		std::string replacer;
		std::string replacee;
		int choice;

		if (std::regex_search(constPhrase, match, r))
		{
			word = match[1];
		}

		choice = sweet::NumberUtils::randomInt(0, terms[word].size() - 1);
		replacee = "\\*" + word + "\\*";
		replacer = terms[word][choice];
		std::regex rg(replacee);

		phrase = std::regex_replace(constPhrase, rg, replacer);
	}

	return phrase;
}

/*std::string PD_InsultGenerator::fixVowels() {
	//do this later. i'm so tired
}*/
