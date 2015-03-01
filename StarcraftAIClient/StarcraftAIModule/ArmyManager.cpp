#include "ArmyManager.h"

ArmyManager::ArmyManager(Producer * producer, Architect * architect)
{
	// Managers
	this->producer = producer;
	this->architect = architect;
	// Local
	troops = new std::set<Unit*>();
	enemyTroops = new std::set<Unit*>();
	enemyBuildings = new std::set<Unit*>();
	enemyPositions = new std::map<BWAPI::Unit*, BWAPI::Position>();
}

// Unused deconstructor
ArmyManager::~ArmyManager()
{
}

void ArmyManager::addEnemyBuilding(BWAPI::Unit * unit)
{
	enemyBuildings->insert(unit);
	enemyPositions->insert(std::make_pair(unit, unit->getPosition()));
}

void ArmyManager::removeEnemyBuilding(BWAPI::Unit * unit)
{
	enemyBuildings->erase(unit);
	enemyPositions->erase(unit);
}

void ArmyManager::addEnemyTroop(BWAPI::Unit * unit)
{
	enemyTroops->insert(unit);
	enemyPositions->insert(std::make_pair(unit,unit->getPosition()));
}

void ArmyManager::removeEnemyTroop(BWAPI::Unit * unit)
{
	enemyTroops->erase(unit);
	enemyPositions->erase(unit);
}

// Adds a unit to the troop pool.
void ArmyManager::addUnit(BWAPI::Unit * unit)
{
	troops->insert(unit);
}

// Removes a unit from the troop pool.
void ArmyManager::removeUnit(BWAPI::Unit * unit)
{
	troops->erase(unit);
}

// Simulate the army manager AI, ordering, creating and upgrading troops.
void ArmyManager::update()
{
	// Create new troops
	/*
	if (producer->totalInfantryFacilities() > 0) // Check if we can produce troops
	{
		producer->orderInfantry(INFANTRY_UNIT);
	}
	*/
	//while (producer->orderInfantry(INFANTRY_UNIT));
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);
	/*
	else // Order a troop facility
	{
		architect->orderBuilding(INFANTRY_FACTORY);
	}
	*/

	// Update enemy positions
	for (auto &u : *enemyTroops)
		updatePos(u);
	for (auto &u : *enemyBuildings)
		updatePos(u);

	// Order existing troops around
	if (!enemyBuildings->empty())
	{
		BWAPI::Position attackTarget = (*enemyBuildings->begin())->getPosition();
		auto it = troops->begin();
		while (it != troops->end())
		{
			BWAPI::Unit * unit = *it;
			if (unit->exists())
			{
				if (unit->isIdle())
					unit->attack(attackTarget); // Some of these units are probably not completed!
				++it;
			}
			else
				it = troops->erase(it);
		}
	}
}

void ArmyManager::updatePos(BWAPI::Unit * unit)
{
	if (unit->isVisible())
		(*enemyPositions)[unit] = unit->getPosition();
}