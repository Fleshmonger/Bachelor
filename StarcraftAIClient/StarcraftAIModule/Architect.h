#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"
#include "Accountant.h"

using namespace BWAPI;
using namespace Filter;

class Architect
{
private:
	//Constants
	const BWAPI::UnitType SUPPLY = BWAPI::UnitTypes::Protoss_Pylon;
	// Managers
	WorkerManager * workerManager;
	Accountant * accountant;
	// Local
	std::map < BWAPI::UnitType, std::pair<BWAPI::Unit,BWAPI::TilePosition> >  * buildOrders;
	std::map < BWAPI::UnitType, BWAPI::Unit > * constructOrders;
	BWAPI::Unitset * pylons; // When playing Protoss, these are used when placing buildings.
	BWAPI::Unit depot;
public:
	Architect(WorkerManager * workerManager, Accountant * accountant);
	~Architect();
	bool orderBuilding(BWAPI::UnitType unitType);
	bool hasOrder(BWAPI::UnitType buildingType);
	int incompleteCount(BWAPI::UnitType buildingType);
	void removeBuildOrder(BWAPI::UnitType buildingType); // TODO Build order is ambigious; rename.
	void updateBuildOrder(BWAPI::Unit building);
	void updateConstructOrder(BWAPI::UnitType buildingType);
	void addPylon(BWAPI::Unit pylon);
	void removePylon(BWAPI::Unit pylon);
	void setDepot(BWAPI::Unit depot);
	void update();
};