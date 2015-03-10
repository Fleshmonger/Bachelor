#include "ArmyManager.h"

ArmyManager::ArmyManager(WorkerManager * workerManager, Producer * producer, Architect * architect) :
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	defender(NULL),
	homeRegion(NULL),
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
	else if (enemyPositions[unit] && Broodwar->isVisible(BWAPI::TilePosition(enemyPositions[unit])))
		enemyPositions[unit] = BWAPI::Positions::None;
}

// Designates the home region.
void ArmyManager::setHomeRegion(BWTA::Region * region)
{
	homeRegion = region;
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
	// Train new troops.
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);


	// Update enemy positions.
	BWAPI::Unit * invader = NULL;
	BOOST_FOREACH(BWAPI::Unit * u, enemyTroops)
	{
		setEnemyPos(u);
		if (BWTA::getRegion(enemyPositions[u]) == homeRegion)
			invader = u;
	}
	BOOST_FOREACH(BWAPI::Unit * u, enemyBuildings)
		setEnemyPos(u);

	// Command defense.
	if (invader)
	{
		// Scout defense.
		if (!defender)
			defender = workerManager->takeWorker();
		if (defender)
			defender->attack(invader);
	}
	else if (defender)
	{
		// Relieve defender.
		workerManager->addWorker(defender);
		defender = NULL;
	}

	// Command attack.
	if (!enemyBuildings.empty())
	{
		BWAPI::Position attackTarget = enemyPositions[*enemyBuildings.begin()];
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