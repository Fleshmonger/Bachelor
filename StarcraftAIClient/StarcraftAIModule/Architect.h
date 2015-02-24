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
	std::multimap < BWAPI::UnitType, std::pair<BWAPI::Unit,BWAPI::TilePosition> >  * buildSchedule;
	std::multimap < BWAPI::UnitType, BWAPI::Unit > * constructSchedule;
	BWAPI::Unitset * pylons;
	BWAPI::Unit depot;
public:
	Architect(WorkerManager * workerManager, Accountant * accountant);
	~Architect();
	bool scheduleBuild(BWAPI::UnitType buildingType);
	void scheduleConstruct(BWAPI::Unit building);
	void removeBuild(std::map<BWAPI::UnitType, std::pair<BWAPI::Unit, BWAPI::TilePosition>>::iterator it);
	void removeBuild(BWAPI::Unit building);
	void removeConstruct(std::map<BWAPI::UnitType, BWAPI::Unit>::iterator it);
	void removeConstruct(BWAPI::Unit building);
	void completeBuild(BWAPI::Unit building);
	void completeConstruct(BWAPI::Unit building);
	int scheduled(BWAPI::UnitType buildingType);
	void addPylon(BWAPI::Unit pylon);
	void removePylon(BWAPI::Unit pylon);
	void setDepot(BWAPI::Unit depot);
	void update();
};