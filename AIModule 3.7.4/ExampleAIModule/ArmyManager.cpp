#include "ArmyManager.h"

// Constructor
ArmyManager::ArmyManager(Archivist * archivist, WorkerManager * workerManager, Producer * producer, Architect * architect) :
archivist(archivist),
workerManager(workerManager),
producer(producer),
architect(architect),
//defender(NULL),
troops(std::set<BWAPI::Unit*>()),
invaders(std::map<BWAPI::Unit*, int>()),
defenders(std::map<BWAPI::Unit*, BWAPI::Unit*>())
{
}

// Deconstructor
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

// Assigns a defender to attack a specific invader
void ArmyManager::assignDefender(BWAPI::Unit * defender, BWAPI::Unit * invader)
{

}

// Simulate the army manager AI, ordering, creating and upgrading troops.
void ArmyManager::update()
{
	// Initialize.
	std::set<BWAPI::Unit*> enemyBuildings = archivist->getBuildings();

	// Train new troops.
	producer->trainUnit(INFANTRY_UNIT);
	architect->scheduleBuild(INFANTRY_FACTORY);

	// Add new invaders.
	std::set<BWAPI::Unit*> newInvaders = archivist->invaders();
	BOOST_FOREACH(BWAPI::Unit * invader, newInvaders)
		if (invaders.count(invader) == 0)
			invaders[invader] = 0;

	// Validate invaders.
	{
		std::map<BWAPI::Unit*, int>::iterator it = invaders.begin();
		while (it != invaders.end())
		{
			BWAPI::Unit * invader = (*it).first;
			++it;
			// Check validity.
			if (invader &&
				(!invader->isVisible() || invader->exists()) &&
				archivist->getPosition(invader))
			{
				// Assign required defenders.
				while (invaders[invader] < DEFENDERS_PER_INVADER)
				{
					BWAPI::Unit * defender = workerManager->takeWorker();
					if (defender &&
						defender->exists())
					{
						defenders[defender] = invader;
						++invaders[invader];
					}
					else
						break;
				}
			}
			else
			{
				// Clear invader.
				invaders.erase(invader);
			}
		}
	}

	// Command defenders.
	{
		std::map<BWAPI::Unit*, BWAPI::Unit*>::iterator it = defenders.begin();
		while (it != defenders.end())
		{
			BWAPI::Unit * defender = (*it).first;
			BWAPI::Unit * invader = (*it).second;
			++it;
			// Check validity.
			if (defender &&
				defender->exists() &&
				invaders.count(invader) > 0)
			{
				// Intercept invader.
				defender->attack(archivist->getPosition(invader));
			}
			else
			{
				// Unassign defender.
				defenders.erase(defender);
				if (defender &&
					defender->exists() &&
					defender->getType().isWorker())
					workerManager->addWorker(defender);
				--invaders[invader];
			}
		}
	}

	/*
	// Command defense.
	if (!invaders.empty())
	{
		// Scout defense.
		if (!defender || !defender->exists())
			defender = workerManager->takeWorker();
		if (defender && defender->exists() && !defender->isAttacking())
			defender->attack(*invaders.begin());
	}
	else if (defender)
	{
		// Relieve defender.
		workerManager->addWorker(defender);
		defender = NULL;
	}
	*/

	// Command attackers.
	if (!enemyBuildings.empty())
	{
		BWAPI::Position attackTarget = archivist->getPosition(*enemyBuildings.begin());
		std::set<BWAPI::Unit*>::iterator it = troops.begin();
		while (it != troops.end())
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