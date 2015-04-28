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
	std::map<BWTA::Region*, utilUnit::UnitSet> regionGeysers;
	std::map<BWAPI::Unit*, BWAPI::Unit*> workerTargets;
	std::map<BWAPI::Unit*, utilUnit::UnitSet> resourceWorkers;

public:
	Gatherer(Landlord * landlord);
	~Gatherer();

	void gather();
	void addRegion(BWTA::Region * region);
	void addGeyser(BWAPI::Unit * geyser);
	void removeGeyser(BWAPI::Unit * geyser);
	void addMineral(BWAPI::Unit * mineral);
	void removeMineral(BWAPI::Unit * mineral);
	void addRefinery(BWAPI::Unit * refinery);
	void removeRefinery(BWAPI::Unit * refinery);
	void addHarvester(BWAPI::Unit * worker, BWTA::Region * region);
	void addMiner(BWAPI::Unit * worker, BWTA::Region * region);
	void removeWorker(BWAPI::Unit * worker, BWTA::Region * region);

	bool contains(BWTA::Region * region);
	bool contains(BWAPI::Unit * worker);

	utilMap::Zone harvestingZone(BWTA::Region * region);

	utilUnit::UnitSet getGeysers(BWTA::Region * region);
	utilUnit::UnitList getMinerals(BWTA::Region * region);
	utilUnit::UnitList getRefineries(BWTA::Region * region);
};

