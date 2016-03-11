#pragma once

#include <PD_DissStats.h>

PD_DissStats::PD_DissStats() :
	defense(0),
	insight(0),
	strength(0),
	sass(0),
	lastDefense(0),
	lastInsight(0),
	lastStrength(0),
	lastSass(0)
{
}

PD_DissStats::~PD_DissStats(){
}

void PD_DissStats::incrementDefense(int _n){
	lastDefense = defense;
	int res = defense + _n;
	defense = res > MAX_DISS_LEVEL ? MAX_DISS_LEVEL : res < 0 ? 0 : res;
}

void PD_DissStats::incrementInsight(int _n){
	lastInsight = insight;
	int res = insight + _n;
	insight = res > MAX_DISS_LEVEL ? MAX_DISS_LEVEL : res < 0 ? 0 : res;
}

void PD_DissStats::incrementStrength(int _n){
	lastStrength = strength;
	int res = strength + _n;
	strength = res > MAX_DISS_LEVEL ? MAX_DISS_LEVEL : res < 0 ? 0 : res;
}

void PD_DissStats::incrementSass(int _n){
	lastSass = sass;
	int res = sass + _n;
	sass = res > MAX_DISS_LEVEL ? MAX_DISS_LEVEL : res < 0 ? 0 : res;
}

int PD_DissStats::getDefense(){
	return defense;
}

int PD_DissStats::getInsight(){
	return insight;
}

int PD_DissStats::getStrength(){
	return strength;
}

int PD_DissStats::getSass(){
	return sass;
}