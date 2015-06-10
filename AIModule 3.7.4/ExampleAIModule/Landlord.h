#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "utilMap.h"
#include "Vassal.h"


// Manages all vassals and controls economy related actions.
//TODO Idleworker should check all regions in order of closest. it should also be possible to simply only take workers from one region.

class Landlord
{
private:
	Vassal
		* main,
		* natural;
	std::set<Vassal*>						vassals;
	std::map<BWAPI::Unit*, BWTA::Region*>	unitRegion;
	std::map<BWTA::Region*, Vassal*>		regionVassal;

public:
	Landlord();
	~Landlord();

	void newVassal(BWTA::Region * region);
	void addDepot(BWAPI::Unit * depot);
	void addWorker(BWAPI::Unit * worker);
	void addWorker(BWAPI::Unit * worker, BWTA::Region * region);
	void moveWorker(BWAPI::Unit * worker, BWTA::Region * region);
	void dismissWorker(BWAPI::Unit * worker);
	void removeDepot(BWAPI::Unit * depot);
	void employWorker(BWAPI::Unit * worker, Task task);
	void removeWorker(BWAPI::Unit * worker);

	bool contains(BWAPI::Unit * unit);
	bool contains(BWTA::Region * region);

	utilMap::Zone getHarvestingZone(BWTA::Region * region);
	Vassal * getMain();
	Vassal * getNatural();
	Vassal * getVassal(BWTA::Region * region);
	std::set<Vassal*> getVassals();

	BWAPI::Unit * getDepot(BWTA::Region * region);
	BWAPI::Unit * getIdleWorker(BWTA::Region * region);
	utilUnit::UnitSet getEmployed(BWTA::Region * region, Task task);
};