#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"

using namespace BWAPI;
using namespace Filter;

class Architect
{
private:
	WorkerManager * workerManager;
	std::map < BWAPI::UnitType, std::pair<BWAPI::Unit,BWAPI::TilePosition> >  * orders;
	BWAPI::Unitset * pylons;
	BWAPI::Unit depot;
	int incompletePylons;
public:
	Architect(WorkerManager * workerManager);
	~Architect();
	bool orderBuilding(BWAPI::UnitType unitType);
	//void buildingCompleted(BWAPI::Unit building);
	bool canAfford(BWAPI::UnitType buildingType);
	void removeOrder(BWAPI::UnitType buildingType);
	void addIncompletePylon();
	void addPylon(BWAPI::Unit pylon);
	void removePylon(BWAPI::Unit pylon);
	void setDepot(BWAPI::Unit depot);
	void update();
};