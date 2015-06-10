#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilMap.h"
#include "UtilUnit.h"
#include "Vassal.h"
#include "Landlord.h"


enum ResourceType { RESOURCE_MINERAL, RESOURCE_GAS };


// Commands workers to mine minerals and harvest gas.
class Gatherer
{
private:
	Landlord	* landlord;

	std::set<BWTA::Region*> regions;
	std::map<BWTA::Region*, utilUnit::UnitList>
		regionMinerals,
		regionRefineries;
	std::map<BWAPI::Unit*, ResourceType> workerResourceType;
	std::map<BWAPI::Unit*, BWTA::Region*> workerRegion;
	std::map<BWAPI::Unit*, BWAPI::Unit*> workerTargets;
	std::map<BWAPI::Unit*, utilUnit::UnitSet> resourceWorkers;
	//std::map<BWAPI::Unit*, utilUnit::UnitList*> workerResources;

public:
	Gatherer(Landlord * landlord);
	~Gatherer();

	void gather();
	void commandGather(BWAPI::Unit * worker, BWTA::Region * region, ResourceType resourceType);
	void addRegion(BWTA::Region * region);
	void removeMineral(BWAPI::Unit * mineral);
	void addRefinery(BWAPI::Unit * refinery);
	void removeRefinery(BWAPI::Unit * refinery);
	void removeResource(BWAPI::Unit * resource, ResourceType resourceType);
	//void addWorker(BWAPI::Unit * worker, utilUnit::UnitList * resources);
	void addWorker(BWAPI::Unit * worker, BWTA::Region * region, ResourceType resourceType);
	void removeWorker(BWAPI::Unit * worker);

	bool contains(BWTA::Region * region);
	bool contains(BWAPI::Unit * worker);

	utilUnit::UnitSet getResourceWorkers(BWAPI::Unit * resource);
	utilUnit::UnitList * getResources(BWTA::Region * region, ResourceType resourceType);
	utilUnit::UnitList getMinerals(BWTA::Region * region);
	utilUnit::UnitList getRefineries(BWTA::Region * region);
};

