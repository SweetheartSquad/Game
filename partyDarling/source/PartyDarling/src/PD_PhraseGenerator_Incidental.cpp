#pragma once

#include<PD_PhraseGenerator_Incidental.h>
#include <PD_ResourceManager.h>
#include <PD_Listing.h>

PD_PhraseGenerator_Incidental::PD_PhraseGenerator_Incidental()
{
	makeDatabases("assets/wordlists/incidental.json");
}

void PD_PhraseGenerator_Incidental::updateNames(PD_Character * _character){
	sweet::ShuffleVector<std::string> names;
	for(auto l : PD_Listing::listingsById) {
		for(auto c : l.second->characters) {
			if(c.second != nullptr){
				// ignore the speaker and any non-incidental characters
				if(c.second != _character && c.second->definition->id.find("RANDOM_CHARACTER") != std::string::npos){
					names.push(c.second->definition->name);
				}
			}
		}
	}
	// omar should always be an option, along with a few non-specific names
	names.push("Omar Clean");
	if(names.size() == 1){
		names.push("some*some-*");
		names.push("a *friend* of mine");
	}

	terms["name"] = names;
}

std::string PD_PhraseGenerator_Incidental::getLineNormal(){
	return replaceWords("*phrases*");
}

std::string PD_PhraseGenerator_Incidental::getLineWon(){
	return replaceWords("*gloats*");
}

std::string PD_PhraseGenerator_Incidental::getLineLost(){
	return replaceWords("*mopes*");
}