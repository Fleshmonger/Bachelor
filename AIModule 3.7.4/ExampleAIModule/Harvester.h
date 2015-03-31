#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "WorkerManager.h"


const unsigned int MAX_MINERS_PER_MINERAL = 3;


// Collects minerals from nearby mineral fields.
class Harvester
{
private:
	WorkerManager * workerManager;

	utilUnit::UnitList minerals;
	std::map<BWAPI::Unit*, BWAPI::Unit*> minerTargets;
	std::map<BWAPI::Unit*, utilUnit::UnitSet> mineralMiners;

public:
	Harvester(WorkerManager * workerManager);
	~Harvester();

	void analyzed();
	void removeMiner(BWAPI::Unit * miner);
	void removeMineral(BWAPI::Unit * mineral);
	void update();

	unsigned int maxMiners();
};

