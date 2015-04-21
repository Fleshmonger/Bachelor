#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "utilMap.h"
#include "Vassal.h"


// Manages all vassals and controls economy related actions.
class Landlord
{
private:
	Vassal * headquarters;
	std::set<Vassal*>						vassals;
	std::map<BWAPI::Unit*, BWTA::Region*>	workerRegion;
	std::map<BWTA::Region*, Vassal*>		regionVassal;

public:
	Landlord();
	~Landlord();

	//void addHeadquarters(BWAPI::Unit * depot);
	//void addExpansion(BWAPI::Unit * depot);
	void update();
	void addDepot(BWAPI::Unit * depot);
	void addWorker(BWAPI::Unit * worker);
	void dismissWorker(BWAPI::Unit * worker);
	void removeWorker(BWAPI::Unit * worker);
	void employWorker(BWAPI::Unit * worker, Task task);
	void newVassal(BWTA::Region * region);

	bool contains(BWAPI::Unit * worker);
	bool contains(BWTA::Region * region);

	utilMap::Zone getHarvestingZone(BWTA::Region * region);

	Vassal * getHeadquarters();
	Vassal * getVassal(BWTA::Region * region);
	std::set<Vassal*> getVassals();

	BWAPI::Unit * getIdleWorker(BWTA::Region * region);
	utilUnit::UnitSet getEmployed(BWTA::Region * region, Task task);
};