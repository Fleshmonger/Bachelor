#pragma once
#include <BWAPI.h>

using namespace BWAPI;

// Tracks reserved resources currently scheduled for use.
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

	void allocate(BWAPI::UnitType unitType);
	void deallocate(BWAPI::UnitType unitType);

	bool isAffordable(BWAPI::UnitType unitType);

	int minerals();
	int gas();
	int supply();
};