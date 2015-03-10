#include "ArmyManager.h"

ArmyManager::ArmyManager(WorkerManager * workerManager, Producer * producer, Architect * architect) :
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	troops(std::set<BWAPI::Unit*>()),
	enemyTroops(std::set<BWAPI::Unit*>()),
	enemyBuildings(std::set<Unit*>()),
	enemyPositions(std::map<BWAPI::Unit*, BWAPI::Position>())
{
}

// Unused deconstructor
ArmyManager::~ArmyManager()
{
}

// Adds an enemy unit from the knowledge pool.
void ArmyManager::addEnemy(BWAPI::Unit * unit)
{
	if (unit->getType().isBuilding())
		enemyBuildings.insert(unit);
	else
		enemyTroops.insert(unit);
	setEnemyPos(unit);
}

// Removes an enemy unit from the knowledge pool.
void ArmyManager::removeEnemy(BWAPI::Unit * unit)
{
	if (unit->getType().isBuilding())
		enemyBuildings.erase(unit);
	else
		enemyTroops.erase(unit);
	enemyPositions.erase(unit);
}

// Updates the position of a visible enemy.
void ArmyManager::setEnemyPos(BWAPI::Unit * unit)
{
	if (unit->isVisible())
		enemyPositions[unit] = unit->getPosition();
}

// Adds a unit to the troop pool.
void ArmyManager::addUnit(BWAPI::Unit * unit)
{
	troops.insert(unit);
}

// Removes a unit from the troop pool.
void ArmyManager::removeUnit(BWAPI::Unit * unit)
{
	troops.erase(unit);
}

// Simulate the army manager AI, ordering, creating and upgrading troops.
void ArmyManager::update()
{
	// Train new troops
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);


	// Update enemy positions
	BOOST_FOREACH(BWAPI::Unit * u, enemyTroops)
		setEnemyPos(u);
	BOOST_FOREACH(BWAPI::Unit * u, enemyBuildings)
		setEnemyPos(u);

	// Command troops
	if (!enemyBuildings.empty())
	{
		BWAPI::Position attackTarget = (*enemyBuildings.begin())->getPosition();
		std::set<BWAPI::Unit*>::iterator it = troops.begin(), end = troops.end();
		while (it != end)
		{
			BWAPI::Unit * unit = *it;
			if (unit->exists())
			{
				if (unit->isIdle())
					unit->attack(attackTarget);
				++it;
			}
			else
				it = troops.erase(it);
		}
	}
}