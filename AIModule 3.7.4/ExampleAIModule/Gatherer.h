#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilMap.h"
#include "UtilUnit.h"
#include "Vassal.h"
#include "Landlord.h"


// Commands workers to mine minerals and harvest gas.
class Gatherer
{
private:
	Landlord	* landlord;

	std::set<BWTA::Region*> regions;
	std::map<BWTA::Region*, utilUnit::UnitList>
		regionMinerals,
		regionRefineries;
	std::map<BWAPI::Unit*, BWAPI::Unit*> workerTargets;
	std::map<BWAPI::Unit*, utilUnit::UnitSet> resourceWorkers;
	std::map<BWAPI::Unit*, utilUnit::UnitList*> workerResources;

public:
	Gatherer(Landlord * landlord);
	~Gatherer();

	void gather();
	void commandGather(BWAPI::Unit * worker, utilUnit::UnitList * resources, BWTA::Region * region);
	void addRegion(BWTA::Region * region);
	void removeMineral(BWAPI::Unit * mineral);
	void addRefinery(BWAPI::Unit * refinery);
	void removeRefinery(BWAPI::Unit * refinery);
	void addWorker(BWAPI::Unit * worker, utilUnit::UnitList * resources);
	void removeWorker(BWAPI::Unit * worker);

	bool contains(BWTA::Region * region);
	bool contains(BWAPI::Unit * worker);

	utilUnit::UnitSet getResourceWorkers(BWAPI::Unit * resource);
	utilUnit::UnitList getMinerals(BWTA::Region * region);
	utilUnit::UnitList getRefineries(BWTA::Region * region);
};

