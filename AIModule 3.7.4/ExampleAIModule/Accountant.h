#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class Accountant
{
private:
	int
		allocatedMinerals,
		allocatedGas,
		allocatedSupply;
public:
	Accountant();
	~Accountant();
	bool isAffordable(BWAPI::UnitType unitType);
	void allocate(BWAPI::UnitType unitType);
	void deallocate(BWAPI::UnitType unitType);
	int minerals();
	//int gas();
	//int supply();
};