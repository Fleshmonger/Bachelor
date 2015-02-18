#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"

using namespace BWAPI;
using namespace Filter;

class Architect
{
private:
	//Constants
	const BWAPI::UnitType SUPPLY = BWAPI::UnitTypes::Protoss_Pylon;
	// Other Managers
	WorkerManager * workerManager;
	// Local variables
	std::map < BWAPI::UnitType, std::pair<BWAPI::Unit,BWAPI::TilePosition> >  * buildOrders;
	std::map < BWAPI::UnitType, BWAPI::Unit > * constructOrders;
	BWAPI::Unitset * pylons; // When playing Protoss, these are used when placing buildings.
	BWAPI::Unit depot;
public:
	Architect(WorkerManager * workerManager);
	~Architect();
	bool orderBuilding(BWAPI::UnitType unitType);
	bool hasOrder(BWAPI::UnitType buildingType);
	bool canAfford(BWAPI::UnitType buildingType);
	void removeBuildOrder(BWAPI::UnitType buildingType);
	void updateBuildOrder(BWAPI::Unit building);
	void updateConstructOrder(BWAPI::UnitType buildingType);
	void addPylon(BWAPI::Unit pylon);
	void removePylon(BWAPI::Unit pylon);
	void setDepot(BWAPI::Unit depot);
	void update();
};