#pragma once

#include <PD_Story.h>
#include <FileUtils.h>

PD_Story::PD_Story(std::string _path){
	std::string file = FileUtils::voxReadFile(_path);

	std::cout << file << std::endl;
}

PD_Story::~PD_Story(){

}