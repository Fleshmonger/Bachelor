#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "WorkerManager.h"


// Collects minerals from nearby mineral fields.
class Harvester
{
private:
	WorkerManager * workerManager;

	utilUnit::UnitList minerals;
	std::map<BWAPI::Unit*, BWAPI::Unit*> minerTargets;
	std::map<BWAPI::Unit*, utilUnit::UnitSet> mineralMiners;

public:
	Harvester(BWTA::Region * region, WorkerManager * workerManager);
	~Harvester();

	void addMineral(BWAPI::Unit * mineral);
	void removeMiner(BWAPI::Unit * miner);
	void removeMineral(BWAPI::Unit * mineral);
	void harvest();

	bool contains(BWAPI::Unit * miner);

	utilUnit::UnitList getMinerals();
};

