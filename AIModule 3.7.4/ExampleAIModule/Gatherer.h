#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Taskmaster.h"


// Mines minerals and harvests gas.
//TODO Globalize this?
class Gatherer
{
private:
	Taskmaster * taskmaster;

	utilUnit::UnitList minerals;
	utilUnit::UnitList refineries;
	std::map<BWAPI::Unit*, BWAPI::Unit*> workerTargets;
	std::map<BWAPI::Unit*, utilUnit::UnitSet> resourceWorkers;

public:
	Gatherer(Taskmaster * taskmaster);
	~Gatherer();

	void gather();
	void addMineral(BWAPI::Unit * mineral);
	void addRefinery(BWAPI::Unit * refinery);
	void addWorker(BWAPI::Unit * worker, bool mining);
	void removeWorker(BWAPI::Unit * worker);
	void removeMineral(BWAPI::Unit * mineral);
	void removeRefinery(BWAPI::Unit * refinery);

	bool contains(BWAPI::Unit * worker);

	utilUnit::UnitList getMinerals();
	utilUnit::UnitList getRefineries();
};

