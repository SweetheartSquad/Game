#pragma once

#include <sweet/UI.h>

#define MAX_DISS_LEVEL 5

class Player;
class PD_Character;

class PD_DissStats {
private:
	int defense, insight, strength, sass;
public:

	PD_DissStats();
	~PD_DissStats();

	int lastDefense;
	int lastInsight;
	int lastStrength;
	int lastSass;

	void setLastStats();

	void incrementDefense(int _n = 1);
	void incrementInsight(int _n = 1);
	void incrementStrength(int _n = 1);
	void incrementSass(int _n = 1);

	int getDefense();
	int getInsight();
	int getStrength();
	int getSass();
	
};