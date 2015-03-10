#pragma once
#include <BWAPI.h>
#include <boost/foreach.hpp>
#include "workerManager.h"
#include "producer.h"
#include "architect.h"

using namespace BWAPI;

const BWAPI::UnitType
	INFANTRY_FACTORY = BWAPI::UnitTypes::Protoss_Gateway,
	INFANTRY_UNIT = BWAPI::UnitTypes::Protoss_Zealot;

class ArmyManager
{
private:
	// Managers
	WorkerManager * workerManager;
	Producer * producer;
	Architect * architect;

	// Local
	std::set<BWAPI::Unit*> troops, enemyTroops, enemyBuildings;
	std::map<BWAPI::Unit*, BWAPI::Position> enemyPositions;

public:
	ArmyManager(WorkerManager * workerManager, Producer * producer, Architect * architect);
	~ArmyManager();
	void addEnemy(BWAPI::Unit * unit);
	void removeEnemy(BWAPI::Unit * unit);
	void setEnemyPos(BWAPI::Unit * unit);
	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void update();
};