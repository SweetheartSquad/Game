#pragma once

#include<PD_PhraseGenerator_Incidental.h>
#include <PD_ResourceManager.h>
#include <PD_Listing.h>

PD_PhraseGenerator_Incidental::PD_PhraseGenerator_Incidental()
{
	makeDatabases("assets/wordlists/incidental.json");
}

std::string PD_PhraseGenerator_Incidental::getLineNormal(PD_Character * _character){
	sweet::ShuffleVector<std::string> names;
	for(auto l : PD_Listing::listingsById) {
		for(auto c : l.second->characters) {
			if(c.second != nullptr){
				if(c.second != _character){
					std::string n = c.second->definition->name;
					names.push(n);
				}
			}
		}
	}
	terms["name"] = names;
	return replaceWords("*phrases*");
}

std::string PD_PhraseGenerator_Incidental::getLineWon(){
	sweet::ShuffleVector<std::string> names;
	names.push("NAME");
	terms["name"] = names;
	return replaceWords("*gloats*");
}

std::string PD_PhraseGenerator_Incidental::getLineLost(){
	sweet::ShuffleVector<std::string> names;
	names.push("NAME");
	terms["name"] = names;
	return replaceWords("*mopes*");
}