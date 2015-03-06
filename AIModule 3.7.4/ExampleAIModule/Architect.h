#pragma once
#include <BWAPI.h>
#include "WorkerManager.h"
#include "Accountant.h"

//using namespace BWAPI;
//using namespace Filter;

static const BWAPI::UnitType SUPPLY = BWAPI::UnitTypes::Protoss_Pylon;

class Architect
{
private:
	// Managers
	WorkerManager * workerManager;
	Accountant * accountant;
	// Local
	BWAPI::Unit * depot;
	std::set<Unit*> * pylons;
	std::multimap < BWAPI::UnitType, BWAPI::Unit* > * constructSchedule;
	std::multimap < BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition> >  * buildSchedule;
public:
	Architect(WorkerManager * workerManager, Accountant * accountant);
	~Architect();
	BWAPI::TilePosition getBuildLocation(BWAPI::Unit * builder, BWAPI::UnitType buildingType, BWAPI::TilePosition desiredLocation);
	bool scheduleBuild(BWAPI::UnitType buildingType);
	void scheduleConstruct(BWAPI::Unit * building);
	void removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildLocation);
	//void removeBuild(std::map<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>::const_iterator it);
	//void removeBuild(BWAPI::Unit * building);
	//void removeConstruct(std::map<BWAPI::UnitType, BWAPI::Unit*>::const_iterator it);
	void removeConstruct(BWAPI::Unit * building);
	void completeBuild(BWAPI::Unit * building);
	void completeConstruct(BWAPI::Unit * building);
	int scheduled(BWAPI::UnitType buildingType);
	void addPylon(BWAPI::Unit * pylon);
	void removePylon(BWAPI::Unit * pylon);
	void setDepot(BWAPI::Unit * depot);
	void update();
};