#pragma once

#include<PD_PhraseGenerator_Incidental.h>
#include <PD_ResourceManager.h>
#include <PD_Listing.h>

PD_PhraseGenerator_Incidental::PD_PhraseGenerator_Incidental()
{
	makeDatabases("assets/wordlists/incidental.json");
}

std::string PD_PhraseGenerator_Incidental::getLine(){
	std::vector<std::string> names;
	/* Uncomment once characters aren't deleted by RoomBuilder 
	for(auto l : PD_Listing::listingsById) {
		for(auto c : l.second->characters) {
			if(c.second != nullptr){
				std::string n = c.second->definition->name;
				names.push_back(n);
			}
		}
	}*/
	names.push_back("NAME");
	terms["name"] = names;
	return replaceWords("*phrases*");
}