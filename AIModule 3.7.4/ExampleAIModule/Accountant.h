#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class Accountant
{
private:
	int mineralsAlloc, gasAlloc, supplyAlloc; // How much of each resource that has been allocated.
public:
	Accountant();
	~Accountant();
	bool isAffordable(BWAPI::UnitType unitType);
	void allocUnit(BWAPI::UnitType unitType);
	void deallocUnit(BWAPI::UnitType unitType);
	int minerals();
	/*
	int gas();
	int supply();
	*/
};