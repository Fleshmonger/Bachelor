#pragma once
#include <BWAPI.h>


// Tracks reserved resources currently scheduled for use and scheduled units.
class Accountant
{
private:
	int
		allocatedMinerals,
		allocatedGas,
		allocatedSupply;

	std::map<BWAPI::UnitType, unsigned int> schedule;

public:
	Accountant();
	~Accountant();

	void allocate(BWAPI::UnitType unitType);
	void deallocate(BWAPI::UnitType unitType);
	void addSchedule(BWAPI::UnitType unitType);
	void removeSchedule(BWAPI::UnitType unitType);

	bool isAffordable(BWAPI::UnitType unitType);
	bool isScheduled(BWAPI::UnitType unitType);

	unsigned int scheduled(BWAPI::UnitType unitType);

	int minerals();
	int gas();
	int supply();
};