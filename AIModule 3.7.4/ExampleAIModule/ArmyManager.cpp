#include "ArmyManager.h"

ArmyManager::ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect) :
	archivist(archivist),
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	defender(NULL),
	troops(std::set<BWAPI::Unit*>())
{
}

// Unused deconstructor
ArmyManager::~ArmyManager()
{
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
	std::set<BWAPI::Unit*>
		enemyBuildings = archivist->getBuildings(),
		invaders = archivist->getInvaders();

	// Train new troops.
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);

	// Command defense.

	if (!invaders.empty())
	{
		// Scout defense.
		if (!defender)
			defender = workerManager->takeWorker();
		if (defender)
			defender->attack(*invaders.begin());
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
		BWAPI::Position attackTarget = archivist->getPosition(*enemyBuildings.begin());
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