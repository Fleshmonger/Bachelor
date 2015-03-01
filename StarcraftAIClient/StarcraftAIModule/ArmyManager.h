#pragma once
#include <BWAPI.h>
#include "producer.h"
#include "architect.h"

using namespace BWAPI;

class ArmyManager
{
private:
	// Constants
	const BWAPI::UnitType
		INFANTRY_FACTORY = BWAPI::UnitTypes::Protoss_Gateway,
		INFANTRY_UNIT = BWAPI::UnitTypes::Protoss_Zealot;
	// Managers
	Producer * producer;
	Architect * architect;
	// Local
	std::set<BWAPI::Unit*> * troops, * enemyTroops, * enemyBuildings;
	std::map<BWAPI::Unit*, BWAPI::Position> * enemyPositions;
public:
	ArmyManager(Producer * producer, Architect * architect);
	~ArmyManager();
	void addEnemyTroop(BWAPI::Unit * unit);
	void removeEnemyTroop(BWAPI::Unit * unit);
	void addEnemyBuilding(BWAPI::Unit * unit);
	void removeEnemyBuilding(BWAPI::Unit * unit);
	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void update();
	void updatePos(BWAPI::Unit * unit); // Rename this
};