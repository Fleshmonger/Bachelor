#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "WorkerManager.h"


// Harvests minerals and gas.
//TODO Rename this and related functions to "gatherer".
class Harvester
{
private:
	WorkerManager * workerManager;

	utilUnit::UnitList minerals;
	utilUnit::UnitList refineries;
	std::map<BWAPI::Unit*, BWAPI::Unit*> minerTargets;
	std::map<BWAPI::Unit*, utilUnit::UnitSet> resourceMiners;

public:
	Harvester(WorkerManager * workerManager);
	~Harvester();

	void addMineral(BWAPI::Unit * mineral);
	void addRefinery(BWAPI::Unit * refinery);
	void removeMiner(BWAPI::Unit * miner);
	void removeMineral(BWAPI::Unit * mineral);
	void removeRefinery(BWAPI::Unit * refinery);
	void harvest();

	bool contains(BWAPI::Unit * miner);

	utilUnit::UnitList getMinerals();
	utilUnit::UnitList getRefineries();
};

