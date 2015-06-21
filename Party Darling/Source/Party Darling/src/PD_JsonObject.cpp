#pragma once

#include <PD_JsonObject.h>
#include <Log.h>

PD_JsonObject::PD_JsonObject(std::string _jsonString){
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( _jsonString, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages() + "\n" + _jsonString);
	}	
}